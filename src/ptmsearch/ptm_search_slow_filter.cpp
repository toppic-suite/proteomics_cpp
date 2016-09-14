// Copyright (c) 2014 - 2016, The Trustees of Indiana University.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation and/or
// other materials provided with the distribution.
//
// Indiana University provides no reassurances that the source code provided does
// not infringe the patent or any other intellectual property rights of any other
// entity. Indiana University disclaims any liability to any recipient for claims
// brought by any other entity based on infringement of intellectual property
// rights or otherwise.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
// ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


#include "base/prot_mod_base.hpp"
#include "base/proteoform_factory.hpp"
#include "ptmsearch/ptm_search_slow_filter.hpp"

namespace prot {

PtmSearchSlowFilter::PtmSearchSlowFilter(
    SpectrumSetPtr spectrum_set_ptr,
    SimplePrsmPtrVec simple_prsm_ptrs,
    CompShiftLowMem comp_shift,
    PtmSearchMngPtr mng_ptr){

  std::string db_file_name = mng_ptr->prsm_para_ptr_->getSearchDbFileName();
  FastaIndexReaderPtr reader_ptr(new FastaIndexReader(db_file_name));
  ModPtrVec fix_mod_list = mng_ptr->prsm_para_ptr_->getFixModPtrVec();
  ProtModPtrVec prot_mod_ptr_vec = mng_ptr->prsm_para_ptr_->getProtModPtrVec();
  ProteoformPtrVec proteoform_ptr_vec;
  for (size_t i = 0; i < simple_prsm_ptrs.size(); i++) {
    std::string seq_name = simple_prsm_ptrs[i]->getSeqName();
    std::string seq_desc = simple_prsm_ptrs[i]->getSeqDesc();
    ProteoformPtr proteo_ptr = ProteoformFactory::readFastaToProteoformPtr(
        reader_ptr, seq_name, seq_desc, fix_mod_list);
    ProteoformPtrVec mod_form_ptr_vec = ProteoformFactory::geneProtModProteoform(
        proteo_ptr, prot_mod_ptr_vec);
    for (size_t j = 0; j < mod_form_ptr_vec.size(); j++) {
      // this slow match is used for four type of alignment, INTERNAL is used to
      // include more diagonals. 
      PtmSlowMatchPtr ptm_slow_match_ptr(new PtmSlowMatch(mod_form_ptr_vec[j],spectrum_set_ptr,
                                                          AlignType::INTERNAL,
                                                          comp_shift, mng_ptr));
      complete_prefix_slow_match_ptrs_.push_back(ptm_slow_match_ptr);
    }
  }

  // init suffix_internal_slow_prsm_ptrs
  for(size_t i=0; i<complete_prefix_slow_match_ptrs_.size();i++){
    ProtModPtr prot_mod_ptr = complete_prefix_slow_match_ptrs_[i]->getProteoform()->getProtModPtr();
    if (prot_mod_ptr == ProtModBase::getProtModPtr_NONE()) {
      suffix_internal_slow_match_ptrs_.push_back(complete_prefix_slow_match_ptrs_[i]);
    }
  }

  // compute complete and prefix prsms 
  for(size_t i=0; i<complete_prefix_slow_match_ptrs_.size();i++){
    PrsmPtrVec comp_ptrs;
    //LOG_DEBUG("Compute complete prsm " << i);
    complete_prefix_slow_match_ptrs_[i]->compute(AlignType::COMPLETE, comp_ptrs);
    complete_prsm_2d_ptrs_.push_back(comp_ptrs);
    PrsmPtrVec prefix_ptrs;
    //LOG_DEBUG("Compute prefix prsm " << i);
    complete_prefix_slow_match_ptrs_[i]->compute(AlignType::PREFIX, prefix_ptrs);
    prefix_prsm_2d_ptrs_.push_back(prefix_ptrs);
    //LOG_DEBUG("compute prefi completed");
  }
  //LOG_DEBUG("complete prefix completed");

  // compute suffix and internal prsms 
  for(size_t i=0; i< suffix_internal_slow_match_ptrs_.size();i++){
    PrsmPtrVec suffix_ptrs;
    suffix_internal_slow_match_ptrs_[i]->compute(AlignType::SUFFIX, suffix_ptrs);
    suffix_prsm_2d_ptrs_.push_back(suffix_ptrs);
    PrsmPtrVec internal_ptrs;
    suffix_internal_slow_match_ptrs_[i]->compute(AlignType::INTERNAL, internal_ptrs);
    internal_prsm_2d_ptrs_.push_back(internal_ptrs);
  }
  //LOG_DEBUG("suffix internal completed");
}

PrsmPtrVec PtmSearchSlowFilter::getPrsms(int shift_num, AlignTypePtr type_ptr){
  PrsmPtrVec prsm_ptrs;
  if (type_ptr == AlignType::COMPLETE) {
    for (size_t i = 0; i < complete_prsm_2d_ptrs_.size(); i++) {
      if (complete_prsm_2d_ptrs_[i][shift_num] != nullptr) {
        prsm_ptrs.push_back(complete_prsm_2d_ptrs_[i][shift_num]);
      }
    }
  }
  else if (type_ptr == AlignType::PREFIX) {
    for (size_t i = 0; i < prefix_prsm_2d_ptrs_.size(); i++) {
      if (prefix_prsm_2d_ptrs_[i][shift_num] != nullptr) {
        prsm_ptrs.push_back(prefix_prsm_2d_ptrs_[i][shift_num]);
      }
    }
  }
  else if (type_ptr == AlignType::SUFFIX) {
    for (size_t i = 0; i < suffix_prsm_2d_ptrs_.size(); i++) {
      if (suffix_prsm_2d_ptrs_[i][shift_num] != nullptr) {
        prsm_ptrs.push_back(suffix_prsm_2d_ptrs_[i][shift_num]);
      }
    }
  }
  else if (type_ptr == AlignType::INTERNAL) {
    for (size_t i = 0; i < internal_prsm_2d_ptrs_.size(); i++) {
      if (internal_prsm_2d_ptrs_[i][shift_num] != nullptr) {
        prsm_ptrs.push_back(internal_prsm_2d_ptrs_[i][shift_num]);
      }
    }
  }
  return prsm_ptrs;
}

} /* namespace prot */
