
#ifndef PROT_ONE_PTM_FILTER_MNG_HPP_
#define PROT_ONE_PTM_FILTER_MNG_HPP_

#include "prsm/prsm_para.hpp"

namespace prot {

class OnePtmFilterMng {
 public:

  OnePtmFilterMng(PrsmParaPtr prsm_para_ptr, int filtering_result_num,
          std::string output_file_ext) {
    prsm_para_ptr_ = prsm_para_ptr;
    one_ptm_filter_result_num_ = filtering_result_num;
    output_file_ext_ = output_file_ext;
  }

  PrsmParaPtr prsm_para_ptr_;

  /** parameters for fast filteration */
  int max_proteoform_mass = 20000;

  //Candidate protein number for each spectrum
  unsigned int one_ptm_filter_result_num_ = 20;

  int ptm_fast_filter_scale_ = 100;

  std::string output_file_ext_;

};

typedef std::shared_ptr<OnePtmFilterMng> OnePtmFilterMngPtr;

} /* namespace tools */

#endif /* ONE_PTM_FILTER_MNG_HPP_ */
