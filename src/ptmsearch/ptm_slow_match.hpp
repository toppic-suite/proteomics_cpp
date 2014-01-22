/*
 * ptm_slow_match.hpp
 *
 *  Created on: Dec 27, 2013
 *      Author: xunlikun
 */

#ifndef PTM_SLOW_MATCH_HPP_
#define PTM_SLOW_MATCH_HPP_

#include <memory>
#include <vector>
#include "prsm/prsm.hpp"
#include "spec/deconv_ms.hpp"
#include "ptmsearch/diagonal_header.hpp"
#include "ptmsearch/ptm_mng.hpp"
#include "spec/prm_peak.hpp"
#include "base/proteoform.hpp"
#include "spec/spectrum_set.hpp"
#include "ptmsearch/comp_shift_low_mem.hpp"
#include "ptmsearch/ptm_mng.hpp"
#include "ptmsearch/diagonal.hpp"
#include "ptmsearch/basic_diag_pair.hpp"
#include "ptmsearch/ps_align.hpp"

namespace prot {

class PtmSlowMatch {
public:
	PtmSlowMatch(ProteoformPtr seq,SpectrumSetPtr spectrum_set,CompShiftLowMemPtr comp_shift,PtmMngPtr mng);
	ProteoformPtr getSeq(){return seq_;};
	double getScr(int shiftnum,int type);
	PrSMPtr geneResult(int shift_num,int type);

	int getShift() const {
		return shift_;
	}

	void setShift(int shift) {
		shift_ = shift;
	}

	int getType() const {
		return type_;
	}

	void setType(int type) {
		type_ = type;
	}

protected:
	PtmMngPtr mng_;
	ProteoformPtr seq_;
	DeconvMsPtr deconv_ms_;
	PrmMsPtr ms_six_;
	ExtendMsPtr ms_three_;
	DiagonalHeaderPtrVec3D result_headers_;
	std::vector<std::vector<double>> result_deltas_;
	std::vector<std::vector<double>> result_scores_;
private:
	int shift_;
	int type_;

	void comp(CompShiftLowMemPtr comp_shift);
	DiagonalHeaderPtrVec getNTermShiftList(std::vector<double> best_shift,PrmMsPtr ms_six,ProteoformPtr seq,PtmMngPtr mng);
	bool found(double shift,DiagonalHeaderPtrVec headerlist,PtmMngPtr mng);
};

typedef std::shared_ptr<PtmSlowMatch> PtmSlowMatchPtr;
typedef std::vector<PtmSlowMatchPtr> PtmSlowMatchPtrVec;

bool inline psm_down(PtmSlowMatchPtr f,PtmSlowMatchPtr n){
	return f->getScr(f->getShift(),f->getType())>n->getScr(n->getShift(),n->getType());
}
} /* namespace prot */

#endif /* PTM_SLOW_MATCH_HPP_ */
