#include "base/logger.hpp"
#include "feature/raw_ms_util.hpp"
#include "feature/deconv_data_base.hpp" 

namespace prot {

DeconvDataPtr DeconvDataBase::getDataPtr(PeakPtrVec &peak_list, FeatureMngPtr mng_ptr) {

  double max_mz = RawMsUtil::findMaxPos(peak_list);
  if (max_mz > mng_ptr->max_mass_) {
    LOG_WARN("Max mz is too large: " << max_mz);
    return nullptr;
  }
  return DeconvDataPtr(new DeconvData(peak_list, mng_ptr->max_mass_, 
                                      mng_ptr->max_charge_, mng_ptr->window_size_));
}

	
// generate deconvolution data using given max mass, max charge
DeconvDataPtr DeconvDataBase::getDataPtr(PeakPtrVec &peak_list, double max_mass,
                                 int max_charge, FeatureMngPtr mng_ptr) {

  if (max_charge < 1) {
    LOG_WARN("Max charge < 1");
    max_charge = mng_ptr->max_charge_;
  }
  if (max_mass <= 0) {
    LOG_WARN("Max mass <= 0");
    max_mass = mng_ptr->max_mass_;
  }
  if (max_mass > mng_ptr->max_mass_) {
    LOG_WARN("Max mass is greater than default max mass " << max_mass);
    max_mass = mng_ptr->max_mass_;
  }
  double max_mz = RawMsUtil::findMaxPos(peak_list);
  if (max_mz > mng_ptr->max_mass_) {
    LOG_WARN("Max mz is too large: " << max_mz);
    return nullptr;
  }
  for (size_t i = 0; i < peak_list.size(); i++) {
    if (peak_list[i]->getPosition() < 0 || peak_list[i]->getIntensity() < 0) {
      LOG_WARN("mz intensity are negative values");
      return nullptr;
    }
  }
  return DeconvDataPtr(new DeconvData(peak_list, max_mass, 
                                      max_charge, mng_ptr->window_size_));
}

}
