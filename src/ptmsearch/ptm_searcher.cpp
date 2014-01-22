/*
 * ptm_searcher.cpp
 *
 *  Created on: Dec 23, 2013
 *      Author: xunlikun
 */

#include "ptm_searcher.hpp"
#include "ptm_slow_match.hpp"
#include "ptmsearch/ptm_slow_filter.hpp"

namespace prot {
PtmSearcher::PtmSearcher(PtmMngPtr mng){
	mng_=mng;
	comp_shift_ = CompShiftLowMemPtr(new CompShiftLowMem());
}

void PtmSearcher::search(SpectrumSetPtr spectrum_set,SimplePrSMPtrVec matches,PrSMPtrVec3D &prsms){
	PtmSlowFilterPtr slow_filter = PtmSlowFilterPtr(new PtmSlowFilter(spectrum_set,matches,comp_shift_,mng_));
	search(spectrum_set,matches,prsms,slow_filter);
}

void PtmSearcher::search(SpectrumSetPtr spectrum_set,SimplePrSMPtrVec matches,PrSMPtrVec3D &prsms,PtmSlowFilterPtr slow_filter){
	for(int s=1;s<=mng_->n_unknown_shift_;s++){
		for(int t=0;t<4;t++){
			PtmSlowMatchPtrVec slow_match = slow_filter->getBestMatch(s,t);
//			std::cout<<slow_match.size()<<std::endl;
//			for(int i=0;i<slow_match.size();i++){
//				std::cout<<slow_match[i]->getScr(s,t)<<std::endl;
//			}
			for(int r = 0;r<mng_->n_report_;r++){
				prsms[s-1][t][r] = nullptr;
			}
			if(slow_match.size()!=0){
				for(int r=0;r<mng_->n_report_;r++){
					if(r >= slow_match.size()){
						break;
					}
					if(slow_match[r]->getScr(s,t)>0){
//						std::cout<<slow_match[r]->getScr(s,t)<<std::endl;
						prsms[s-1][t][r]=slow_match[r]->geneResult(s,t);
//						std::cout<<(s-1)<<":"<<t<<":"<<r<<std::endl;
					}
				}
			}
		}
	}
}
} /* namespace prot */
