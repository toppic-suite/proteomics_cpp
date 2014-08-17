#include <cmath>

#include "base/prot_mod.hpp"
#include "base/change.hpp"
#include "base/algorithm.hpp"
#include "ptmsearch/diagonal_header.hpp"

namespace prot {

DiagonalHeader::DiagonalHeader(double n_term_shift,
                               bool n_strict, bool c_strict,
                               bool prot_n_match,bool prot_c_match,
                               bool pep_n_match, bool pep_c_match) {
  prot_N_term_shift_ = n_term_shift;
  n_strict_ = n_strict;
  c_strict_ = c_strict;
  prot_N_term_match_ = prot_n_match;
  prot_C_term_match_ = prot_c_match;
  pep_N_term_match_ = pep_n_match;
  pep_C_term_match_ = pep_c_match;
}
DiagonalHeaderPtr DiagonalHeader::clone() {
  DiagonalHeaderPtr cloned(new DiagonalHeader(prot_N_term_shift_, n_strict_, c_strict_, 
                                              prot_N_term_match_, prot_C_term_match_,
                                              pep_N_term_match_, pep_C_term_match_));
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

void DiagonalHeader::initData(double c_shift, ProteoformPtr proteo_ptr, 
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
  }
  else {
    is_align_prefix_ = false;
  }

  if (std::abs(prot_C_term_shift_) <= align_pref_suff_shift_thresh) {
    is_align_suffix_ = true;
  }
  else {
    is_align_suffix_ = false;
  }
}

ChangePtrVec getUnexpectedChanges(const DiagonalHeaderPtrVec &header_ptrs, 
                                  int first_res_pos, int last_res_pos) {
  ChangePtrVec change_list;
  if (!header_ptrs[0]->isPepNTermMatch() && !header_ptrs[0]->isProtNTermMatch()) {
    change_list.push_back(
        ChangePtr(new Change(0, header_ptrs[0]->getMatchFirstBpPos()-first_res_pos,
                             UNEXPECTED_CHANGE, header_ptrs[0]->getPepNTermShift(), nullptr)));
  }
  for (size_t i = 0; i < header_ptrs.size() - 1; i++) {
    ChangePtr change_ptr(new Change(header_ptrs[i]->getMatchLastBpPos()-first_res_pos,
                                    header_ptrs[i + 1]->getMatchFirstBpPos()-first_res_pos,
                                    UNEXPECTED_CHANGE,
                                    header_ptrs[i + 1]->getProtNTermShift()
                                    - header_ptrs[i]->getProtNTermShift(),
                                    nullptr));
    change_list.push_back(change_ptr);
  }
  DiagonalHeaderPtr last_header_ptr = header_ptrs[header_ptrs.size() - 1];
  if (!last_header_ptr->isPepCTermMatch() && !last_header_ptr->isProtCTermMatch()) {
    ChangePtr change_ptr(new Change(last_header_ptr->getMatchLastBpPos()-first_res_pos, 
                                    (last_res_pos + 1) -first_res_pos,
                                    UNEXPECTED_CHANGE, last_header_ptr->getPepCTermShift(), 
                                    nullptr));
    change_list.push_back(change_ptr);
  }
  return change_list;
}

/*
DiagonalHeaderPtrVec getNTermShiftListTruncPrefix(ProteoformPtr seq) {
  DiagonalHeaderPtrVec extend_n_term_shift;
  std::vector<double> seq_masses = seq->getBpSpecPtr()->getPrmMasses();
  double shift;
  for (size_t i = 1; i < seq_masses.size(); i++) {
    shift = -seq_masses[i];
    extend_n_term_shift.push_back(
        DiagonalHeaderPtr(new DiagonalHeader(shift, true, false, true, false)));
  }

  return extend_n_term_shift;
}

DiagonalHeaderPtrVec getNTermShiftListTruncsuffix(PrmMsPtr ms,
                                                  ProteoformPtr seq) {
  DiagonalHeaderPtrVec extend_n_term_shift;
  std::vector<double> ms_masses = prot::getMassList(ms);
  std::vector<double> seq_masses = seq->getBpSpecPtr()->getPrmMasses();
  double shift;
  for (size_t i = 1; i < seq_masses.size(); i++) {
    shift = ms_masses[ms_masses.size() - 1] - seq_masses[i];
    extend_n_term_shift.push_back(
        DiagonalHeaderPtr(new DiagonalHeader(shift, true, false, true, false)));
  }
  return extend_n_term_shift;
}
DiagonalHeaderPtrVec get1dHeaders(DiagonalHeaderPtrVec2D headers) {
  DiagonalHeaderPtrVec header_list;
  for (size_t i = 0; i < headers.size(); i++) {
    for (size_t j = 0; j < headers[i].size(); j++) {
      header_list.push_back(headers[i][j]);
    }
  }
  return header_list;
}

*/

} /* namespace prot */
