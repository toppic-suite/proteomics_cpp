// Copyright (c) 2014 - 2017, The Trustees of Indiana University.
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


#include <cmath>

#include "base/prot_mod.hpp"
#include "base/mod_base.hpp"
#include "base/change.hpp"
#include "base/algorithm.hpp"
#include "oneptmsearch/diagonal_header.hpp"

namespace prot {

DiagonalHeaderPtr DiagonalHeader::clone() {
  DiagonalHeaderPtr cloned = std::make_shared<DiagonalHeader>(prot_N_term_shift_, n_strict_, c_strict_, 
                                                              prot_N_term_match_, prot_C_term_match_,
                                                              pep_N_term_match_, pep_C_term_match_);
  cloned->setId(id_);
  cloned->setTruncFirstResPos(trunc_first_res_pos_);
  cloned->setMatchFirstBpPos(match_first_bp_pos_);
  cloned->setPepNTermShift(pep_N_term_shift_);
  cloned->setProtNTermShift(prot_N_term_shift_);
  cloned->setAlignPrefix(is_align_prefix_);
  cloned->setTruncLastResPos(trunc_last_res_pos_);
  cloned->setMatchLastBpPos(match_last_bp_pos_);
  cloned->setPepCTermShift(pep_C_term_shift_);
  cloned->setProtCTermShift(prot_C_term_shift_);
  cloned->setAlignSuffix(is_align_suffix_);
  return cloned;
}

DiagonalHeaderPtr geneDiagonalHeaderPtr(int bgn, int end, 
                                        DiagonalHeaderPtr header_ptr) {
  DiagonalHeaderPtr new_header_ptr = header_ptr->clone();
  new_header_ptr->setMatchFirstBpPos(bgn);
  new_header_ptr->setMatchLastBpPos(end);
  return new_header_ptr;
}

void DiagonalHeader::initHeader(double c_shift, ProteoformPtr proteo_ptr, 
                                double align_pref_suff_shift_thresh) {
  // set protein c term shift
  prot_C_term_shift_ = c_shift;
  std::vector<double> prm_masses = proteo_ptr->getBpSpecPtr()->getPrmMasses();

  trunc_first_res_pos_ = getFirstResPos(prot_N_term_shift_,
                                        prm_masses);

  trunc_last_res_pos_ = getLastResPos(prot_C_term_shift_, prm_masses);
  pep_N_term_shift_ = prot_N_term_shift_ + prm_masses[trunc_first_res_pos_];
  pep_C_term_shift_ = prot_C_term_shift_ + prm_masses[prm_masses.size() - 1]
      - prm_masses[trunc_last_res_pos_ + 1];

  if (std::abs(prot_N_term_shift_) <= align_pref_suff_shift_thresh) {
    is_align_prefix_ = true;    
  } else {
    is_align_prefix_ = false;
  }

  if (std::abs(prot_C_term_shift_) <= align_pref_suff_shift_thresh) {
    is_align_suffix_ = true;
  } else {
    is_align_suffix_ = false;
  }
}

ChangePtrVec getDiagonalMassChanges(const DiagonalHeaderPtrVec &header_ptrs, 
                                    int first_res_pos, int last_res_pos, ChangeTypePtr change_type_ptr) {
  ChangePtrVec change_list;
  ModPtr none_ptr = ModBase::getNoneModPtr();
  if (!header_ptrs[0]->isPepNTermMatch() && !header_ptrs[0]->isProtNTermMatch()) {
    change_list.push_back(std::make_shared<Change>(0, header_ptrs[0]->getMatchFirstBpPos()-first_res_pos,
                                                   change_type_ptr, header_ptrs[0]->getPepNTermShift(), none_ptr));
  }
  for (size_t i = 0; i < header_ptrs.size() - 1; i++) {
    ChangePtr change_ptr = std::make_shared<Change>(header_ptrs[i]->getMatchLastBpPos()-first_res_pos,
                                                    header_ptrs[i + 1]->getMatchFirstBpPos() - first_res_pos,
                                                    change_type_ptr,
                                                    header_ptrs[i + 1]->getProtNTermShift() - header_ptrs[i]->getProtNTermShift(),
                                                    none_ptr);
    change_list.push_back(change_ptr);
  }
  DiagonalHeaderPtr last_header_ptr = header_ptrs[header_ptrs.size() - 1];
  if (!last_header_ptr->isPepCTermMatch() && !last_header_ptr->isProtCTermMatch()) {
    ChangePtr change_ptr = std::make_shared<Change>(last_header_ptr->getMatchLastBpPos()-first_res_pos, 
                                                    (last_res_pos + 1) - first_res_pos,
                                                    change_type_ptr, last_header_ptr->getPepCTermShift(), 
                                                    none_ptr);
    change_list.push_back(change_ptr);
  }
  return change_list;
}

ChangePtrVec getDiagonalMassChanges(const DiagonalHeaderPtrVec &header_ptrs, 
                                    int first_res_pos, int last_res_pos, ChangeTypePtrVec &change_types) {
  ChangePtrVec change_list;
  ModPtr none_ptr = ModBase::getNoneModPtr();
  if (!header_ptrs[0]->isPepNTermMatch() && !header_ptrs[0]->isProtNTermMatch()) {
    change_list.push_back(std::make_shared<Change>(0, header_ptrs[0]->getMatchFirstBpPos()-first_res_pos,
                                                   change_types[0], 
                                                   header_ptrs[0]->getPepNTermShift(), none_ptr));
  }
  for (size_t i = 0; i < header_ptrs.size() - 1; i++) {
    ChangePtr change_ptr = std::make_shared<Change>(header_ptrs[i]->getMatchLastBpPos()-first_res_pos,
                                                    header_ptrs[i + 1]->getMatchFirstBpPos() - first_res_pos,
                                                    change_types[i + 1],
                                                    header_ptrs[i + 1]->getProtNTermShift() - header_ptrs[i]->getProtNTermShift(),
                                                    none_ptr);
    change_list.push_back(change_ptr);
  }
  DiagonalHeaderPtr last_header_ptr = header_ptrs[header_ptrs.size() - 1];
  if (!last_header_ptr->isPepCTermMatch() && !last_header_ptr->isProtCTermMatch()) {
    ChangePtr change_ptr = std::make_shared<Change>(last_header_ptr->getMatchLastBpPos()-first_res_pos, 
                                                    (last_res_pos + 1) -first_res_pos,
                                                    ChangeType::UNEXPECTED, last_header_ptr->getPepCTermShift(), 
                                                    none_ptr);
    change_list.push_back(change_ptr);
  }
  return change_list;
}

}  // namespace prot
