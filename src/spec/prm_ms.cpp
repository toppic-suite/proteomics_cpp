#include <algorithm>
#include <iostream>

#include "base/logger.hpp"
#include "spec/prm_ms.hpp"

namespace prot {

inline bool massErrorUp(const std::pair<int, int> &a, const std::pair<int,int> b) {
  return a.first < b.first;
}

inline std::pair<int, int> getMassError(PrmPeakPtr peak_ptr, double scale, 
                                        bool n_strict, bool c_strict) {
  int m = (int)std::round(peak_ptr->getPosition()*scale);
  int e = 0;
  if(n_strict && c_strict){
    e = (int) std::ceil(peak_ptr->getStrictTolerance()*scale);
  }
  else if(n_strict && !c_strict){
    e = (int) std::ceil(peak_ptr->getNStrictCRelaxTolerance()*scale);
  }
  else if(!n_strict && c_strict){
    e = (int) std::ceil(peak_ptr->getNRelaxCStrictTolerance()*scale);
  }
  std::pair<int, int> mass_error(m, e);
  return mass_error;
}

std::vector<std::pair<int, int>> PrmMs::getIntMassErrorList(
    const PrmMsPtrVec &prm_ms_ptr_vec, PeakTolerancePtr tole_ptr,
    double scale, bool n_strict, bool c_strict){
  std::vector<std::pair<int,int>> mass_errors;
  for (size_t i = 0; i < prm_ms_ptr_vec.size(); i++) {
    PrmMsPtr prm_ms_ptr = prm_ms_ptr_vec[i];
    std::pair<int,int> last_mass_error(-1, 0);
    for(size_t j=0; j<prm_ms_ptr->size(); j++){
      std::pair<int, int> cur_m_e = getMassError(prm_ms_ptr->getPeakPtr(j), scale, n_strict, c_strict);
      if(cur_m_e.first != last_mass_error.first){
        mass_errors.push_back(cur_m_e);
        last_mass_error = cur_m_e;
      }
      else if(cur_m_e.second > last_mass_error.second){
        mass_errors.pop_back();
        mass_errors.push_back(cur_m_e);
        last_mass_error = cur_m_e;
      }
    }
    //add zero mass for each spectrum to increase the score for zero mass
    double prec_mass = prm_ms_ptr_vec[i]->getMsHeaderPtr()->getPrecMonoMass();
    PrmPeakPtr zero_prm_ptr = getZeroPeakPtr(i, prec_mass, tole_ptr, 1);
    mass_errors.push_back(getMassError(zero_prm_ptr, scale, n_strict, c_strict));
    //add prec mass for each spectrum 
    PrmPeakPtr prec_prm_ptr = getPrecPeakPtr(i, prec_mass, tole_ptr, 1);
    mass_errors.push_back(getMassError(prec_prm_ptr, scale, true, true));
  }

  std::sort(mass_errors.begin(), mass_errors.end(),massErrorUp);
  return mass_errors;
}

PrmPeakPtrVec PrmMs::getPrmPeakPtrs(const PrmMsPtrVec &prm_ms_ptr_vec, 
                                    PeakTolerancePtr tole_ptr) {
  PrmPeakPtrVec peak_list;
  for (size_t i = 0; i < prm_ms_ptr_vec.size(); i++) {
    for(size_t j= 0;j<prm_ms_ptr_vec[i]->size() ;j++){
      peak_list.push_back(prm_ms_ptr_vec[i]->getPeakPtr(j));
    }
  }
  // add zero 
  double prec_mass = prm_ms_ptr_vec[0]->getHeaderPtr()->getPrecMonoMass();
  // use spec_id = 0 and score = group_spec_num (size of prm_ms_ptr_vec)
  PrmPeakPtr zero_prm_ptr = getZeroPeakPtr(0, prec_mass, tole_ptr, prm_ms_ptr_vec.size());
  peak_list.push_back(zero_prm_ptr);
  // add prec mass  
  PrmPeakPtr prec_prm_ptr = getPrecPeakPtr(0, prec_mass, tole_ptr, prm_ms_ptr_vec.size());
  peak_list.push_back(prec_prm_ptr);
  std::sort(peak_list.begin(), peak_list.end(), prmPeakUp);
  for (size_t i = 0; i < peak_list.size(); i++) {
    peak_list[i]->setPeakId(i);
  }
  return peak_list;
}

} /* namespace prot */
