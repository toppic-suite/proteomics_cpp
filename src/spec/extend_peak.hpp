/*
 * extend_peak.hpp
 *
 *  Created on: Dec 4, 2013
 *      Author: xunlikun
 */

#ifndef PROT_EXTEND_PEAK_HPP_
#define PROT_EXTEND_PEAK_HPP_

#include <vector>
#include <memory>
#include <algorithm>

#include "spec/deconv_peak.hpp"
#include "spec/ms.hpp"
#include "spec/sp_para.hpp"
#include "spec/deconv_ms.hpp"

namespace prot {

class ExtendPeak : public Peak{
public:
	ExtendPeak();
	ExtendPeak(DeconvPeakPtr base_peak,double mono_mass,double score);
	DeconvPeakPtr getBasePeakPtr(){return base_peak_;}
	double getMonoMass(){return mono_mass_;}
	double getScore(){return score_;}
	double getOrigTolerance(){return orig_tolerance_;}
	double getReverseTolerance(){return reverse_tolerance_;}
	void setOrigTolerance(double orig_tolerance){orig_tolerance_ = orig_tolerance;}
	void setReverseTolerance(double reverse_tolerance){reverse_tolerance_ = reverse_tolerance;}
	void appendXml(XmlDOMDocument* xml_doc,xercesc::DOMElement* parent);
private:
	DeconvPeakPtr base_peak_;
	double mono_mass_;
	double score_;
	double orig_tolerance_;
	double reverse_tolerance_;
};

typedef std::shared_ptr<ExtendPeak> ExtendPeakPtr;
typedef std::vector<ExtendPeakPtr> ExtendPeakPtrVec;
typedef std::shared_ptr<Ms<ExtendPeakPtr>> ExtendMsPtr;

inline bool extendpeak_up(const ExtendPeakPtr p,ExtendPeakPtr n){
  return p->getPosition() < n->getPosition();
}

inline void getExtendMassVec (ExtendMsPtr &refine_ms_ptr, std::vector<double> &masses) {
  ExtendPeakPtrVec peak_ptr_list = refine_ms_ptr->getPeakPtrVec();
  for (unsigned int i = 0; i < peak_ptr_list.size(); i++) {
    masses.push_back(peak_ptr_list[i]->getPosition());
  }
}

ExtendMsPtr getMsThree(DeconvMsPtr deconv_ms,double delta,SpParaPtr sp_para);

} /* namespace prot */

#endif /* EXTEND_PEAK_HPP_ */
