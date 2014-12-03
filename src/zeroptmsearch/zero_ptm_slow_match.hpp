#ifndef ZERO_PTM_SLOW_MATCH_HPP_
#define ZERO_PTM_SLOW_MATCH_HPP_

#include "spec/deconv_peak.hpp"
#include "spec/theo_peak.hpp"
#include "zeroptmsearch/zero_ptm_mng.hpp"
#include "zeroptmsearch/zero_ptm_fast_match.hpp"
#include "prsm/prsm.hpp"

namespace prot {

class ZeroPtmSlowMatch {
 public:
  ZeroPtmSlowMatch(const DeconvMsPtrVec &deconv_ms_ptr_vec, 
                   ZpFastMatchPtr fast_match_ptr,
                   ZeroPtmMngPtr mng_ptr);

  double getScore() {return score_;}

  PrsmPtr geneResult();

 private:
  ZeroPtmMngPtr mng_ptr_;
  ZpFastMatchPtr fast_match_ptr_;
  DeconvMsPtrVec deconv_ms_ptr_vec_;
  ProteoformPtr proteoform_ptr_;

  double refine_prec_mass_;
  ExtendMsPtrVec refine_ms_ptr_vec_;

  double score_ = 0;

  void compScore (const ExtendMsPtrVec &refine_ms_ptr_vec);

  //double recal_ = 0;
  //bool isValid (double recal, double ppo);
};

typedef std::shared_ptr<ZeroPtmSlowMatch> ZpSlowMatchPtr;
typedef std::vector<ZpSlowMatchPtr> ZpSlowMatchPtrVec;

inline bool compareZeroPtmSlowMatchDown(const ZpSlowMatchPtr &a, 
                                        const ZpSlowMatchPtr &b) {
  return a->getScore() > b->getScore();
}

ZpSlowMatchPtrVec zeroPtmSlowFilter(const DeconvMsPtrVec &deconv_ms_ptr_vec,
                                    const ZpFastMatchPtrVec &fast_match_ptrs,
                                    ZeroPtmMngPtr mng_ptr); 

}

#endif
