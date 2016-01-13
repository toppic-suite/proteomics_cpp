#ifndef PROT_ONE_PTM_FILTER_MNG_HPP_
#define PROT_ONE_PTM_FILTER_MNG_HPP_

#include "prsm/prsm_para.hpp"

namespace prot {

class OnePtmFilterMng {
 public:
  OnePtmFilterMng(PrsmParaPtr prsm_para_ptr, 
                  std::string output_file_ext): 
      prsm_para_ptr_(prsm_para_ptr),
      output_file_ext_(output_file_ext) {
      }

  PrsmParaPtr prsm_para_ptr_;

  /** parameters for fast filteration */
  int max_proteoform_mass_ = 20000;

  //Candidate protein number for each spectrum
  unsigned int comp_num_ = 10;
  unsigned int pref_suff_num_ = 5;
  unsigned int inte_num_ = 10;
  unsigned int shift_num_ = 10;
  int filter_scale_ = 100;

  std::string output_file_ext_;

};

typedef std::shared_ptr<OnePtmFilterMng> OnePtmFilterMngPtr;

} /* namespace tools */

#endif /* ONE_PTM_FILTER_MNG_HPP_ */
