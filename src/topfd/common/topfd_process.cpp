//Copyright (c) 2014 - 2019, The Trustees of Indiana University.
//
//Licensed under the Apache License, Version 2.0 (the "License");
//you may not use this file except in compliance with the License.
//You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
//Unless required by applicable law or agreed to in writing, software
//distributed under the License is distributed on an "AS IS" BASIS,
//WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//See the License for the specific language governing permissions and
//limitations under the License.
//

#include "common/util/logger.hpp"
#include "common/util/file_util.hpp"
#include "common/util/str_util.hpp"
#include "common/base/base_data.hpp"
#include "ms/spec/msalign_frac_merge.hpp"
#include "ms/spec/deconv_json_merge.hpp"
#include "ms/feature/feature_merge.hpp"
#include "ms/env/env_base.hpp"
#include "topfd/deconv/deconv_process.hpp"
#include "topfd/feature_detect/feature_detect.hpp"
#include "topfd/common/topfd_para.hpp"

namespace toppic {

namespace topfd_process {

void processOneFile(TopfdParaPtr para_ptr,  
                    const std::string &spec_file_name, 
                   int frac_id) {
  try {
    int thread_number = std::stoi(para_ptr->thread_number_);

    std::cout << "Processing " << spec_file_name << " started." << std::endl;
    std::cout << "Deconvolution started." << std::endl;

    DeconvProcess processor(para_ptr, spec_file_name, frac_id, thread_number, &processor);
    
    processor.process();
    std::cout << "Deconvolution finished." << std::endl;

    std::cout << "Deleting temporary files - started." << std::endl;
    std::string base_name_ms1 = file_util::basename(spec_file_name) + "_ms1.msalign";
    std::string base_name_ms2 = file_util::basename(spec_file_name) + "_ms2.msalign";
    std::string base_name_concat = "concat_" + file_util::basename(spec_file_name);

    std::string fa_base_ms1 = file_util::absoluteName(base_name_ms1);
    std::string fa_base_ms2 = file_util::absoluteName(base_name_ms2);
    std::string fa_base_concat = file_util::absoluteName(base_name_concat);

    std::replace(fa_base_ms1.begin(), fa_base_ms1.end(), '\\', '/');
    std::replace(fa_base_ms2.begin(), fa_base_ms2.end(), '\\', '/');
    std::replace(fa_base_concat.begin(), fa_base_concat.end(), '\\', '/');

    for (int i = 0; i < thread_number; i++){
      file_util::cleanPrefix(base_name_ms1 + std::to_string(i), fa_base_ms1 + std::to_string(i));
      file_util::cleanPrefix(base_name_ms2 + std::to_string(i), fa_base_ms2 + std::to_string(i));
    }
    file_util::cleanPrefix(base_name_concat, fa_base_concat);
    std::cout << "Deleting temporary files - finished." << std::endl; 

    std::cout << "Feature detection started." << std::endl;
    feature_detect::process(frac_id, 
                            spec_file_name,
                            para_ptr->missing_level_one_, 
                            para_ptr->resource_dir_);
    std::cout << "Feature detection finished." << std::endl;
    
    std::cout << "Processing " << spec_file_name << " finished." << std::endl;

    

  } catch (const char* e) {
    std::cout << "[Exception]" << std::endl;
    std::cout << e << std::endl;
    exit(EXIT_FAILURE);
  }
}

void moveFiles(std::string &spec_file_name, bool move_mzrt) {
  std::string base_name = file_util::basename(spec_file_name);
  std::string file_dir =  base_name + "_file";
  file_util::createFolder(file_dir);
  std::string file_name = base_name + "_ms1.msalign";
  file_util::moveFile(file_name, file_dir);
  //file_name = base_name + "_feature.xml";
  //file_util::moveFile(file_name, file_dir);
  if (move_mzrt) {
    file_name = base_name + "_frac.mzrt.csv";
    file_util::moveFile(file_name, file_dir);
  }
  /*
  if (move_sample_feature) {
    file_name = base_name + "_ms1.feature";
    file_util::moveFile(file_name, file_dir);
  }
  */
}

bool isValidFile(std::string &file_name) {
  if (str_util::endsWith(file_name, "mzML")
      || str_util::endsWith(file_name, "mzXML")
      || str_util::endsWith(file_name, "mzml")
      || str_util::endsWith(file_name, "mzxml")) {
    return true;
  }
  else {
    return false;
  }
}

void mergeFiles(TopfdParaPtr para_ptr, 
                std::vector<std::string> &spec_file_lst) {
  std::string para_str = para_ptr->getParaStr("#");
  std::string merged_file_name = para_ptr->merged_file_name_;
  std::cout << "Merging files started." << std::endl;
  MsAlignFracMergePtr msalign_merger 
      = std::make_shared<MsAlignFracMerge>(spec_file_lst, merged_file_name);
  msalign_merger->process(para_str);
  msalign_merger = nullptr;
  DeconvJsonMergePtr json_merger 
      = std::make_shared<DeconvJsonMerge>(spec_file_lst, merged_file_name);
  json_merger->process();
  json_merger = nullptr;
  FeatureMergePtr feature_merger 
      = std::make_shared<FeatureMerge>(spec_file_lst, merged_file_name);
  feature_merger->process(para_str);
  feature_merger = nullptr;
  std::cout << "Merging files finished." << std::endl;
}

int process(TopfdParaPtr para_ptr,  std::vector<std::string> spec_file_lst) {
  base_data::init();
  std::string print_str = para_ptr->getParaStr("");
  std::cout << print_str;

  EnvBase::initBase(para_ptr->resource_dir_);
  for (size_t k = 0; k < spec_file_lst.size(); k++) {
    if (isValidFile(spec_file_lst[k])) {
      processOneFile(para_ptr, spec_file_lst[k], k);
    }
  }

  // merge files
  if (para_ptr->merge_files_) {
    mergeFiles(para_ptr, spec_file_lst);
  }

  // Move some files to the folder basename_file
  for (size_t k = 0; k < spec_file_lst.size(); k++) {
    if (isValidFile(spec_file_lst[k])) {
      bool move_mzrt = true;
      moveFiles(spec_file_lst[k], move_mzrt); 
    }
  }

  if (para_ptr->merge_files_) {
    bool move_mzrt = false;
    moveFiles(para_ptr->merged_file_name_, move_mzrt);
  }

  

  std::cout << "TopFD finished." << std::endl << std::flush;
  return 0;
}


} // namespace topfd_process 

}  // namespace toppic

