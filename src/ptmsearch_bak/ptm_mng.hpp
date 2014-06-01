/*
 * ptm_mng.hpp
 *
 *  Created on: Dec 19, 2013
 *      Author: xunlikun
 */

#ifndef PROT_PTM_MNG_HPP_
#define PROT_PTM_MNG_HPP_

#include "prsm/prsm_para.hpp"

namespace prot {

class PtmMng {
 public :
  PtmMng(PrsmParaPtr prsm_para_ptr, int n_report, int shift_num,
         double align_max_shift, std::string input_file_ext, 
         std::string output_file_ext) {
    prsm_para_ptr_ = prsm_para_ptr;
    n_report_ = n_report;
    n_unknown_shift_ = shift_num;
    align_max_shift_ = align_max_shift;
    if (align_large_shift_thresh_ < align_max_shift_) {
      align_large_shift_thresh_ = align_max_shift_;
    }
    input_file_ext_ = input_file_ext;
    output_file_ext_ = output_file_ext;
  }
  
  PrsmParaPtr prsm_para_ptr_;

  /* parameters for ptm search */
  int n_report_ = 1;
  int n_unknown_shift_ =2;
  int n_known_shift_ = 0;

  int ptm_fast_filter_scale_ = 100;
  int n_top_diagonals_ = 20;
  double min_double_gap=0.25;
  int min_diagonal_gap_ = (int)(ptm_fast_filter_scale_ * min_double_gap);

  /* used in determining if a diagonal is n_term_match or c_term_match,
   * when tolerance is very large, some diagonal is "refined" based on
   * precursor mass, which is not good. So we keep a small value. */

  // ??? This may be increased to ppo * precursor_mass so that best shift
  // diagaonls can be assigned as n-term or c-term matches
  double term_match_error_tolerance = 0.05;

  /* used in determining if a diagonal is similar to a trunc diagonal */
  double extend_trunc_error_tolerance_ = 0.5;

  double align_prefix_suffix_shift_thresh_ = 300;

  double align_min_shift_ = 0.5;
  double align_large_shift_thresh_ = 300;
  // set panelty to 3
  double align_large_shift_panelty_ = 3;

  double align_max_shift_ = 1000000;

  double adjust_prec_step_width_ = 0.005;

  std::string input_file_ext_;
  std::string output_file_ext_;
};

typedef std::shared_ptr<PtmMng> PtmMngPtr;

} /* namespace prot */

#endif /* PTM_MNG_HPP_ */
