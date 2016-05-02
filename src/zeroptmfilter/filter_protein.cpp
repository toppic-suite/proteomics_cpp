#include <algorithm>

#include "base/logger.hpp"
#include "zeroptmfilter/filter_protein.hpp"

namespace prot {

inline bool cmpScore(const std::pair<int, int> &a, const std::pair<int, int> &b) {
    return a.second > b.second;
}

inline void sortResult(std::vector<std::pair<int,int>> &results) {
  std::pair<int,int> tmp;
  for (size_t i = 0; i < results.size(); i++) {
    for (size_t j = i+1; j < results.size(); j++) {
      if (results[i].second < results[j].second) {
        tmp = results[i];
        results[i] = results[j];
        results[j] = tmp;
      }   
    }   
  }
}


FilterProteinPtrVec FilterProtein::geneResults(std::vector<std::pair<int,int>> &single_type_results, 
                                               int threshold, int single_type_num) {

  FilterProteinPtrVec prot_results;
  //std::sort(single_type_results.begin(), single_type_results.end(), cmpScore);
  sortResult(single_type_results);
  int output_num =0;
  for(int i=0;i< single_type_num;i++){
    if (i >= (int)single_type_results.size()) {
      break;
    }
    //LOG_DEBUG("rank " << i  <<  " score " << single_type_results[i].second);
    if(single_type_results[i].second >= threshold){
      output_num++;
    }
    else {
      break;
    }
  }
  for(int i=0;i<output_num;i++){
    int prot_id = single_type_results[i].first;
    int score = single_type_results[i].second;
    FilterProteinPtr prot_ptr = FilterProteinPtr(new FilterProtein(prot_id, score));
    prot_results.push_back(prot_ptr);
  }
  return prot_results;
}

} /* namespace prot */

