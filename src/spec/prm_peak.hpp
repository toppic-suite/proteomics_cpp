/*
 * prm_peak.hpp
 *
 *  Created on: Dec 4, 2013
 *      Author: xunlikun
 */

#ifndef PROT_PRM_PEAK_HPP_
#define PROT_PRM_PEAK_HPP_

#include <memory>
#include <vector>

#include "spec/deconv_peak.hpp"
#include "spec/support_peak.hpp"
#include "spec/ms.hpp"
#include "spec/sp_para.hpp"
#include "deconv_ms.hpp"

namespace prot {

#define PRM_PEAK_TYPE_ORIGINAL 0
#define PRM_PEAK_TYPE_REVERSED 1

class PrmPeak : public Peak {
public:
    PrmPeak(DeconvPeakPtr base_peak,int base_type,double mono_mass,
          double score);
    void addNghbEdge(DeconvPeakPtr peak,double offset,
                   SPTypePtr peak_type,double score);
    int getNeighborSize(){return neighbor_list_.size();}
    DeconvPeakPtr getBasePeak(){return base_peak_;}
    double getMonoMass(){return mono_mass_;}
    double getScr(){return score_;}
    double getStrictTolerance(){return strict_tolerance_;}
    int getBaseType(){return base_type_;}
    double getNStrictCRelaxTolerance(){return n_strict_c_relax_tolerance_;}
    double getNRelaxCStrictTolerance(){return n_relax_c_strict_tolerance_;}
    int getBreakType();
    void setStrictTolerance(double tolerance){strict_tolerance_ = tolerance;}

    void setNStrictCRelacTolerance(double tolerance){
    n_strict_c_relax_tolerance_ = tolerance;}

    void setNRelaxCStrictTolerance(double tolerance){
    n_relax_c_strict_tolerance_ = tolerance;}

private:
    DeconvPeakPtr base_peak_;
    double mono_mass_;
    double score_;
    int base_type_;
    double strict_tolerance_;
    double n_strict_c_relax_tolerance_;
    double n_relax_c_strict_tolerance_;
    SupportPeakPtrVec neighbor_list_;
};

typedef std::shared_ptr<PrmPeak> PrmPeakPtr;
typedef std::vector<PrmPeakPtr> PrmPeakPtrVec;
typedef std::shared_ptr<Ms<PrmPeakPtr>> PrmMsPtr;

inline bool prmPeakUp(const PrmPeakPtr p,PrmPeakPtr n){
  return p->getPosition() < n->getPosition();
}

PrmMsPtr getMsTwo(DeconvMsPtr deconv_ms,double delta, SpParaPtr sp_para);

PrmMsPtr getSpSix(DeconvMsPtr deconv_ms,double delta, SpParaPtr sp_para); 

PrmMsPtr getShiftSpSix(DeconvMsPtr deconv_ms,double delta,double shift,
                       SpParaPtr sp_para);

std::vector<std::vector<int>> getIntMassErrorList(PrmMsPtr ms,double scale,
                                                  bool n_strict,
                                                  bool c_strict);
std::vector<double> getMassList(PrmMsPtr ms);
std::vector<double> getScoreList(PrmMsPtr ms);

} /* namespace prot */

#endif /* PRM_PEAK_HPP_ */
