#include "feature/charge_cmp.hpp" 

namespace prot {



// get common peaks in two peak lists 
std::vector<int> getCommonPeak(RealEnvPtr env_a, RealEnvPtr env_b) {
  std::vector<int> list_a = env_a->getPeakIdxList();
  std::vector<int> list_b = env_b->getPeakIdxList();
  int len = list_a.size();
  std::vector<int> common_peaks(len, -1);
  for (int i = 0; i < len; i++) {
    for (size_t j = 0; j < list_b.size(); j++) {
      if (env_a->isExist(i) && list_a[i] == list_b[j]) {
        common_peaks[i] = list_a[i];
      }
    }
  }
  return common_peaks;
}

// count the number of common peaks. 
int cntCommonPeak(std::vector<int> &list_a) {
  int cnt = 0;
  for (size_t i = 0; i < list_a.size(); i++) {
    if (list_a[i] >= 0) {
      cnt++;
    }
  }
  return cnt;
}

// check if the second charge state is better 
bool isSecondBetter(PeakPtrVec &peak_list, MatchEnvPtr a,
                    MatchEnvPtr  b, double tolerance) {
  // get common peak 
  std::vector<int> common_peaks = getCommonPeak(a->getRealEnvPtr(), b->getRealEnvPtr());
  int common_num = cntCommonPeak(common_peaks);
  if (common_num <= 6) {
    return false;
  }
  /* get distance list */
  std::vector<double> dist;
  for (size_t i = 0; i < common_peaks.size(); i++) {
    for (size_t j = i + 1; j < common_peaks.size(); j++) {
      if (common_peaks[i] >= 0 && common_peaks[j] >= 0) {
        dist.push_back(
            (peak_list[common_peaks[j]]->getPosition() - peak_list[common_peaks[i]]->getPosition()) / (j - i));
      }
    }
  }
  // check if the distance is near to 1/(chrg_a) or 1/(chrg+b) 
  int cnt_a = 0;
  int cnt_b = 0;
  double center_a = 1 / a->getTheoEnvPtr()->getCharge();
  double center_b = 1 / b->getTheoEnvPtr()->getCharge();
  for (size_t i = 0; i < dist.size(); i++) {
    double d = dist[i];
    if (std::abs(d - center_a) < std::abs(d - center_b)
        && std::abs(d - center_a) < tolerance) {
      cnt_a++;
    }
    if (std::abs(d - center_b) < std::abs(d - center_a)
        && std::abs(d - center_b) < tolerance) {
      cnt_b++;
    }
  }
  if (cnt_b > cnt_a) {
    return true;
  } else {
    return false;
  }
}

int ChargeCmp::comp(PeakPtrVec &peak_list, MatchEnvPtr a, MatchEnvPtr b, double tolerance) {
  if (isSecondBetter(peak_list, a, b, tolerance)) {
    return -1;
  }
  if (isSecondBetter(peak_list, b, a, tolerance)) {
    return 1;
  }
  return 0;
}

}