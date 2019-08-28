//Copyright (c) 2014 - 2018, The Trustees of Indiana University.
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
#include <iomanip>

#include "common/util/version.hpp"
#include "common/util/logger.hpp"
#include "common/util/time_util.hpp"
#include "common/util/file_util.hpp"
#include "common/util/str_util.hpp"
#include "common/base/base_data.hpp"
#include "spec/msalign_frac_merge.hpp"
#include "feature_detect/feature_detect.hpp"
#include "feature/feature_merge.hpp"
#include "deconv/env/env_base.hpp"
#include "deconv/deconv/deconv_process.hpp"
#include "deconv/deconv/deconv_json_merge.hpp"

namespace toppic {

namespace topfd_process {

std::string geneArgumentStr(std::map<std::string, std::string> arguments, 
                            const std::string & prefix) {
  std::stringstream output;
  output << prefix << "TopFD " << version_number << std::endl;
  // TIME_STAMP_STR is replaced later
  output << prefix << "Timestamp: " << time_util::TIME_STAMP_STR << std::endl;
  output << prefix << "###################### Parameters ######################" << std::endl;
  output << prefix << std::setw(40) << std::left 
      << "Data type: " << "centroid" << std::endl;
  output << prefix << std::setw(40) << std::left 
      << "Maximum charge: " << arguments["maxCharge"] << std::endl;
  output << prefix << std::setw(40) << std::left 
      << "Maximum monoisotopic mass: " << arguments["maxMass"] << " Dalton" << std::endl;
  output << prefix << std::setw(40) << std::left 
      << "Error tolerance: " << arguments["mzError"] << " m/z" << std::endl;
  output << prefix << std::setw(40) << std::left 
      << "MS1 signal/noise ratio: " << arguments["msOneSnRatio"] << std::endl;
  output << prefix << std::setw(40) << std::left 
      << "MS/MS signal/noise ratio: " << arguments["msTwoSnRatio"] << std::endl;
  output << prefix << std::setw(40) << std::left 
      << "Precursor window size: " << arguments["precWindow"] << " m/z" << std::endl;
  //output << prefix << std::setw(40) << std::left 
  //    << "Do final filtering: " << para_ptr->do_final_filtering_ << std::endl;
  output << prefix << "###################### Parameters ######################" << std::endl;
  return output.str();
}



int processOneFile(std::map<std::string, std::string> arguments, 
                   const std::string &argument_str, 
                   const std::string &spec_file_name, int frac_id) {
  try {
    time_t start = time(0);

    base_data::init();
    DeconvParaPtr para_ptr = std::make_shared<DeconvPara>(arguments, argument_str, 
        spec_file_name, frac_id);
    LOG_DEBUG("deconv para");
    DeconvProcess process(para_ptr);
    LOG_DEBUG("init process");
    process.process();

    std::string argu_str = para_ptr->getArgumentStr();
    std::string sp_file_name = para_ptr->getDataFileName();
    feature_detect::process(frac_id, sp_file_name, arguments["resourceDir"], 
        para_ptr->missing_level_one_, argu_str);

    time_t end = time(0);
    std::cout << "Runing time: "
      << str_util::toString(static_cast<int>(difftime(end, start)))
      << " seconds." << std::endl;
  } catch (const char* e) {
    std::cout << "[Exception]" << std::endl;
    std::cout << e << std::endl;
  }
  std::cout << "TopFD finished." << std::endl << std::flush;
  return 0;
}


void moveFiles(std::string &spec_file_name, bool combined) {
  std::string base_name = file_util::basename(spec_file_name);
  std::string file_dir =  base_name + "_file";
  file_util::createFolder(file_dir);
  std::string file_name = base_name + "_ms1.msalign";
  file_util::moveFile(file_name, file_dir);
  file_name = base_name + "_frac.feature";
  file_util::moveFile(file_name, file_dir);
  file_name = base_name + "_sample.feature";
  file_util::moveFile(file_name, file_dir);
  if (!combined) {
    file_name = base_name + "_frac.mzrt.csv";
    file_util::moveFile(file_name, file_dir);
  }
}

int process(std::map<std::string, std::string> arguments, 
            std::vector<std::string> spec_file_lst) {
  std::string argument_str = geneArgumentStr(arguments, "#");
  EnvBase::initBase(arguments["resourceDir"]);
  for (size_t k = 0; k < spec_file_lst.size(); k++) {
    if (str_util::endsWith(spec_file_lst[k], "mzML")
        || str_util::endsWith(spec_file_lst[k], "mzXML")
        || str_util::endsWith(spec_file_lst[k], "mzml")
        || str_util::endsWith(spec_file_lst[k], "mzxml")) {
      int result = processOneFile(arguments, argument_str, spec_file_lst[k], k);
      if (result != 0) {
        return 1;
      }
    }
  }

  if (spec_file_lst.size() > 1) {
    time_util::addTimeStamp(argument_str);
    std::string sample_name = arguments["sampleName"];
    std::cout << "Merging files started." << std::endl;
    MsAlignFracMergePtr msalign_merger = std::make_shared<MsAlignFracMerge>(spec_file_lst, sample_name);
    msalign_merger->process(argument_str);
    msalign_merger = nullptr;
    DeconvJsonMergePtr json_merger = std::make_shared<DeconvJsonMerge>(spec_file_lst, sample_name);
    json_merger->process();
    json_merger = nullptr;
    FeatureMergePtr feature_merger = std::make_shared<FeatureMerge>(spec_file_lst, sample_name);
    feature_merger->process(argument_str);
    feature_merger = nullptr;
    std::cout << "Merging files ended." << std::endl;
  }


  for (size_t k = 0; k < spec_file_lst.size(); k++) {
    if (str_util::endsWith(spec_file_lst[k], "mzML")
        || str_util::endsWith(spec_file_lst[k], "mzXML")
        || str_util::endsWith(spec_file_lst[k], "mzml")
        || str_util::endsWith(spec_file_lst[k], "mzxml")) {
      bool combined = false;
      moveFiles(spec_file_lst[k], combined); 
    }
  }

  if (spec_file_lst.size() > 1) {
    std::string sample_name = arguments["sampleName"];
    bool combined = true;
    moveFiles(sample_name, combined);
  }

  return 0;
}


} // namespace topfd_process 

}  // namespace toppic

