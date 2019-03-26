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

#include <set>
#include <algorithm>

#include "common/util/logger.hpp"
#include "common/util/file_util.hpp"
#include "common/util/str_util.hpp"
#include "spec/msalign_reader.hpp"
#include "spec/msalign_writer.hpp"
#include "spec/msalign_frac_combine.hpp"

namespace toppic {

MsalignFracCombine::MsalignFracCombine(
    const std::vector<std::string> &spec_file_names,
    const std::string &output_file_name):
    spec_file_names_(spec_file_names),
    output_file_name_(output_file_name) {
    }

void mergeMsAlignFiles(const std::vector<std::string> & spec_file_lst,
                       int MAX_SPEC_NUM, const std::string & output_file) {
  std::ofstream outfile; 
  outfile.open(output_file.c_str());

  for (size_t i = 0; i < spec_file_lst.size(); i++) {
    MsAlignReader sp_reader(spec_file_lst[i], 1, nullptr, std::set<std::string>());
    std::vector<std::string> ms_lines = sp_reader.readOneStrSpectrum();
    while (ms_lines.size() > 0) {
      for (size_t k = 0; k< ms_lines.size(); k++) {
        if (ms_lines[k].substr(0, 3) == "ID=") {
          outfile << "ID=" << (MAX_SPEC_NUM * i + std::stoi(ms_lines[k].substr(3))) 
              << std::endl;
        } else if (ms_lines[k].substr(0, 10) == "MS_ONE_ID=") {
          outfile << "MS_ONE_ID=" 
              << (MAX_SPEC_NUM * i + std::stoi(ms_lines[k].substr(10))) << std::endl;
        } else {
          outfile << ms_lines[k] << std::endl;
        }
      }
      outfile << std::endl;
      ms_lines = sp_reader.readOneStrSpectrum();
    }
    sp_reader.close();
  }

  outfile.close();
}

void MsalignFracCombine::process() {
  std::vector<std::string> ms1_file_names;
  std::vector<std::string> ms2_file_names;
  for (size_t i = 0; i < spec_file_names_.size(); i++) { 
    std::string base_name = file_util::basename(spec_file_names_[i]);
    std::string ms1_name = base_name + "_ms1.msalign";
    ms1_file_names.push_back(ms1_name);
    std::string ms2_name = base_name + "_ms2.msalign";
    ms2_file_names.push_back(ms2_name);
  }
  
  std::string ms1_output_name = output_file_name_ + "_ms1.msalign";
  std::string ms2_output_name = output_file_name_ + "_ms2.msalign";

  mergeMsAlignFiles(ms1_file_names, MAX_SPEC_NUM_PER_FILE, ms1_output_name); 
  mergeMsAlignFiles(ms2_file_names, MAX_SPEC_NUM_PER_FILE, ms2_output_name); 
}

/*

  // open files
  MsAlignReaderPtrVec reader_ptrs;
  DeconvMsPtrVec ms_ptrs;
  for (size_t i = 0; i < input_num; i++) {
    std::string input_file_name = base_name + "." + input_file_exts_[i];
    MsAlignReaderPtr reader_ptr
        = std::make_shared<MsAlignReader>(input_file_name);
    LOG_DEBUG("input file name " << input_file_name);
    DeconvMsPtr ms_ptr = reader_ptr->getNextMs();
    reader_ptrs.push_back(reader_ptr);
    ms_ptrs.push_back(ms_ptr);
  }

  std::string output_filename = base_name + "." + output_file_ext_;
  std::ofstream out_stream; 
  out_stream.open(output_filename.c_str());

  // combine
  int spec_id = 0;
  bool finish = false;
  while (!finish) {
    // LOG_DEBUG("spec id " << spec_id << " input num " << input_num);
    finish = true;
    int cur_ms_idx = getCurMsIndex(ms_ptrs);
    if (cur_ms_idx < 0) {
      finish = true;
      break;
    }
    else { 
      DeconvMsPtr cur_ms_ptr = ms_ptrs[cur_ms_idx];
      cur_ms_ptr->getMsHeaderPtr()->setId(spec_id);
      spec_id++;
      msalign_writer::write(out_stream, cur_ms_ptr);
      ms_ptrs[cur_ms_idx] = reader_ptrs[cur_ms_idx]->getNextMs();
    }
  }

  // close files
  for (size_t i = 0; i < input_num; i++) {
    reader_ptrs[i]->close();
  }
  out_stream.close();
}
*/

/*
inline int getCurMsIndex(DeconvMsPtrVec &ms_ptrs) {
  int index = -1;
  int scan_num = std::numeric_limits<int>::max();
  for (size_t i = 0; i < ms_ptrs.size(); i++) {
    if (ms_ptrs[i] != nullptr) {
      if (ms_ptrs[i]->getMsHeaderPtr()->getFirstScanNum() < scan_num) {
        scan_num = ms_ptrs[i]->getMsHeaderPtr()->getFirstScanNum();
        index = i;
      }
    }
  }
  return index;
}
*/

} /* namespace toppic */