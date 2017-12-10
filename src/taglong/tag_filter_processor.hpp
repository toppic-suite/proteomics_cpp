// Copyright (c) 2014 - 2017, The Trustees of Indiana University.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


#ifndef PROT_TAG_LONG_FILTER_PROCESSOR_HPP
#define PROT_TAG_LONG_FILTER_PROCESSOR_HPP


#include <utility>
#include <string>
#include <vector>

#include "taglong/tag_filter_mng.hpp"
#include "peak_node.hpp"

namespace prot {

class TagFilterProcessor {
 public:
  explicit TagFilterProcessor(TagFilterMngPtr mng_ptr): mng_ptr_(mng_ptr) {}

  void process();

 private:
  TagFilterMngPtr mng_ptr_;

  void processDB();

  std::vector<std::pair<double, std::string> > mass_list_;

  std::vector<double> getEdgeLimits(PeakNodePtr peak, PeakNodePtr next);

  void generateGapEdges(std::vector<PeakNodePtr> & peaks);

  std::vector<std::string> getTags(std::vector<std::vector<PeakNodePtr> > componentsFromGraph);

  int compTagScore(const std::string & seq, const std::vector<std::string> & tags);
};

typedef std::shared_ptr<TagFilterProcessor> TagFilterProcessorPtr;

}  // namespace prot

#endif
