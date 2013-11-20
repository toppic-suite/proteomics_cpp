/*
 * author  Xiaowen Liu
 * date    2013-11-1
 */

#include "component.hpp"
#include "residue_util.hpp"

namespace proteomics {

Component::Component (const char* acid_file_name,
                      const char* ptm_file_name,
                      const char* residue_file_name) {
  acid_ptr_vec_ = getAcidPtrVecInstance(acid_file_name);
  ptm_ptr_vec_ = getPtmPtrVecInstance(acid_ptr_vec_, ptm_file_name);
  residue_ptr_vec_ = getResiduePtrVecInstance(acid_ptr_vec_,
                                             ptm_ptr_vec_,
                                             residue_file_name);
}

}

