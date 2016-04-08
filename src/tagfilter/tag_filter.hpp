
#ifndef PROT_TAG_FILTER
#define PROT_TAG_FILTER

#include "base/proteoform.hpp"
#include "base/residue_util.hpp"
#include "spec/prm_ms.hpp"
#include "prsm/simple_prsm.hpp"
#include "tag_filter_mng.hpp"
#include "tag.hpp"

namespace prot {

class TagFilter {
 public:
  TagFilter(const ProteoformPtrVec &proteo_ptrs,
            TagFilterMngPtr mng_ptr);

  std::vector<std::string> getBestMatch(const PrmMsPtrVec &ms_ptr_vec);

 private:
  TagFilterMngPtr mng_ptr_;
  ProteoformPtrVec proteo_ptrs_;
  //std::map<std::string, seq_tag> seq_tag_map_;
  std::vector<std::pair<std::string, double>> residue_mass_list_;
  std::vector<Tag> geneSpecTag(const PrmPeakPtrVec & prm_peaks); 

  void geneSeqTag(ProteoformPtr proteoform);
};

typedef std::shared_ptr<TagFilter> TagFilterPtr;
} /* namespace prot */

#endif
