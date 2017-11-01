//Copyright (c) 2014 - 2017, The Trustees of Indiana University.
//
//Licensed under the Apache License, Version 2.0 (the "License");
//you may not use this file except in compliance with the License.
//You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
//Unless required by applicable law or agreed to in writing, software
//distributed under the License is distributed on an "AS IS" BASIS,
//WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//See the License for the specific language governing permissions and
//limitations under the License.


#include "base/logger.hpp"
#include "spec/peak.hpp"
#include "feature/raw_ms_util.hpp"
#include "feature/real_env.hpp" 

namespace prot {

RealEnv::RealEnv(std::vector<PeakPtr> &peak_list, EnvelopePtr theo_env, 
                 double tolerance, double min_inte) {
		// copy 
		refer_idx_ = theo_env->getReferIdx();
		charge_ = theo_env->getCharge();
		mono_mz_ = theo_env->getMonoMz();

		// map peaks in theo_env to the peaks in sp 
		mapPeakList(peak_list, theo_env, tolerance, min_inte);
		// remove duplicated matches 
		remvDuplMatch(theo_env);
		// count missing peak number 
		cntMissPeakNum();
		// count maximum consecutive peak number 
		cntMaxConsPeakNum();
	}


// map peaks in theo_env to the peaks in sp 
void RealEnv::mapPeakList(std::vector<PeakPtr> &peak_list, EnvelopePtr theo_env, 
                          double tolerance, double min_inte) {
  int peak_num = theo_env->getPeakNum();
  mzs_.resize(peak_num, getNonExistPeakIdx());
  intensities_.resize(peak_num, 0.0);
  peak_idxes_.resize(peak_num, getNonExistPeakIdx());
  for (int i = 0; i < peak_num; i++) {
    //PeakPtr peak_ptr(new Peak(theo_env->getMz(i), 0));
    int idx = RawMsUtil::getNearPeakIdx(peak_list, theo_env->getMz(i), tolerance);
    //LOG_DEBUG("peak list size " << peak_list.size() << " theo mz " << theo_env->getMz(i) << " idx " << idx << " tolerance " << tolerance);
    if (idx >= 0 && peak_list[idx]->getIntensity() >= min_inte) {
      peak_idxes_[i] = idx;
      mzs_[i] = peak_list[idx]->getPosition();
      intensities_[i] = peak_list[idx]->getIntensity();
    }
  }
}

// Remove duplicated matches. If two theoretical peaks are matched to the
// same real peak, only the one with less mz error is kept.
void RealEnv::remvDuplMatch(EnvelopePtr theo_env) {
  for (int i = 0; i < getPeakNum() - 1; i++) {
    if (isExist(i) && peak_idxes_[i] == peak_idxes_[i + 1]) {
      if (std::abs(theo_env->getMz(i) - mzs_[i]) < std::abs(theo_env->getMz(i + 1) - mzs_[i + 1])) {
        peak_idxes_[i + 1] = getNonExistPeakIdx();
        mzs_[i + 1] = getNonExistPeakIdx();
        intensities_[i + 1] = 0;
      } else {
        peak_idxes_[i] = getNonExistPeakIdx();
        mzs_[i] = getNonExistPeakIdx();
        intensities_[i] = 0;
      }
    }
  }
}

// Count missing peak number 
void RealEnv::cntMissPeakNum() {
  miss_peak_num_ = 0;
  for (int i = 0; i < getPeakNum(); i++) {
    if (!isExist(i)) {
      miss_peak_num_++;
    }
  }
}

// Compute maximum number of consecutive matched peaks 
void RealEnv::cntMaxConsPeakNum() {
  max_consecutive_peak_num_ = 0;
  int n = 0;
  for (int i = 0; i < getPeakNum(); i++) {
    if (isExist(i)) {
      n++;
      if (n > max_consecutive_peak_num_) {
        max_consecutive_peak_num_ = n;
      }
    } else {
      n = 0;
    }
  }
}

bool RealEnv::isExist(int i) {
  if (peak_idxes_[i] != getNonExistPeakIdx()) {
    return true;
  } else {
    return false;
  }
}

bool RealEnv::testPeakShare(RealEnvPtr a, RealEnvPtr  b) {
  std::vector<int> peak_A = a->getPeakIdxList();
  std::vector<int> peak_B = b->getPeakIdxList();
  for (size_t i = 0; i < peak_A.size(); i++) {
    for (size_t j = 0; j < peak_B.size(); j++) {
      if (peak_A[i] >= 0 && peak_B[j] == peak_A[i]) {
        return true;
      }
    }
  }
  return false;
}

}
