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

#include <string>

#include "base/proteoform.hpp"
#include "base/proteoform_factory.hpp"
#include "base/file_util.hpp"
#include "base/thread_pool.hpp"
#include "spec/msalign_reader.hpp"
#include "spec/msalign_util.hpp"
#include "spec/spectrum_set.hpp"
#include "prsm/simple_prsm_xml_writer.hpp"
#include "prsm/simple_prsm_str_combine.hpp"
#include "zeroptmfilter/zero_ptm_filter_processor.hpp"
#include "zeroptmfilter/mass_zero_ptm_filter.hpp"

namespace prot {

void ZeroPtmFilterProcessor::process() {
  PrsmParaPtr prsm_para_ptr = mng_ptr_->prsm_para_ptr_;
  std::string db_file_name = prsm_para_ptr->getSearchDbFileName();
  std::string sp_file_name = prsm_para_ptr->getSpectrumFileName();
  DbBlockPtrVec db_block_ptr_vec = DbBlock::readDbBlockIndex(db_file_name);
  SpParaPtr sp_para_ptr = prsm_para_ptr->getSpParaPtr();
  int group_spec_num = prsm_para_ptr->getGroupSpecNum();
  MsAlignReader ms_reader(prsm_para_ptr->getSpectrumFileName(),
                          group_spec_num,
                          prsm_para_ptr->getSpParaPtr()->getActivationPtr(),
                          prsm_para_ptr->getSpParaPtr()->getSkipList());

  FastaReader fasta_reader(db_file_name);

  FastaSeqPtr f_seq = fasta_reader.getNextSeq();

  std::vector<std::string> seq_name_vec;

  std::vector<std::string> seq_desc_vec;

  while (f_seq != nullptr) {
    seq_name_vec.push_back(f_seq->getName());
    seq_desc_vec.push_back(f_seq->getDesc());
    f_seq = fasta_reader.getNextSeq(); 
  }

  fasta_reader.close();

  SimplePrsmXmlWriter comp_writer(prot::file_util::basename(sp_file_name) + ".NO_FILTER_COMPLETE");
  SimplePrsmXmlWriter pref_writer(prot::file_util::basename(sp_file_name) + ".NO_FILTER_PREFIX");
  SimplePrsmXmlWriter inte_writer(prot::file_util::basename(sp_file_name) + ".NO_FILTER_INTERNAL");
  SimplePrsmXmlWriter suff_writer(prot::file_util::basename(sp_file_name) + ".NO_FILTER_SUFFIX");

  SpectrumSetPtr spec_set_ptr = ms_reader.getNextSpectrumSet(sp_para_ptr)[0];

  while (spec_set_ptr != nullptr) {
    MsHeaderPtr header_ptr = spec_set_ptr->getDeconvMsPtrVec()[0]->getMsHeaderPtr();
    for (size_t k = 0; k < seq_desc_vec.size(); k++) {
      SimplePrsmPtr sim_prsm = std::make_shared<SimplePrsm>(header_ptr, group_spec_num,
                                                            seq_name_vec[k], seq_desc_vec[k], 0);
      comp_writer.write(sim_prsm);
      pref_writer.write(sim_prsm);
      inte_writer.write(sim_prsm);
      suff_writer.write(sim_prsm);
    }
    spec_set_ptr = ms_reader.getNextSpectrumSet(sp_para_ptr)[0];
  }

  ms_reader.close();
  comp_writer.close();
  pref_writer.close();
  inte_writer.close();
  suff_writer.close();
}

} /* namespace prot */
