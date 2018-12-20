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


#ifndef PROT_PRSM_PRSM_FDR_HPP_
#define PROT_PRSM_PRSM_FDR_HPP_

#include <map>
#include <string>

#include "util/str_util.hpp"
#include "seq/fasta_reader.hpp"
#include "prsm/prsm_xml_writer.hpp"

namespace toppic {

class PrsmFdr {
 public:
  PrsmFdr(const std::string &db_file_name,
          const std::string &spec_file_name,
          const std::string &input_file_ext,
          const std::string &output_file_ext): 
      db_file_name_(db_file_name),
      spec_file_name_(spec_file_name),
      input_file_ext_(input_file_ext),
      output_file_ext_(output_file_ext) {}
  void process();
 private:
  std::string db_file_name_;
  std::string spec_file_name_;
  std::string input_file_ext_;
  std::string output_file_ext_;

  void computeFdr(PrsmStrPtrVec &target, PrsmStrPtrVec &decoy);

  void computeProteoformFdr(PrsmStrPtrVec2D &target, PrsmStrPtrVec2D &decoy);
};
typedef std::shared_ptr<PrsmFdr> PrsmFdrPtr;

}  // namespace toppic

#endif /* PRSMFDR_HPP_ */
