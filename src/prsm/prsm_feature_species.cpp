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

#include <string>
#include <algorithm>
#include <vector>

#include <boost/algorithm/string.hpp>

#include "base/file_util.hpp"
#include "base/proteoform_util.hpp"
#include "prsm/prsm_reader.hpp"
#include "prsm/prsm_feature_species.hpp"
#include "prsm/prsm_util.hpp"
#include "spec/msalign_reader.hpp"

namespace prot {

void PrsmFeatureSpecies::setProtId(PrsmStrPtrVec& prsm_ptrs) {
  std::vector<PrsmStrPtrVec> proteins;
  std::vector<std::string> protein_names;
  for (size_t i = 0; i < prsm_ptrs.size(); i++) {
    std::string name = prsm_ptrs[i]->getSeqName();
    bool is_found = false;
    for (size_t j = 0; j < protein_names.size(); j++) {
      if (protein_names[j] == name) {
        proteins[j].push_back(prsm_ptrs[i]);
        is_found = true;
        break;
      }
    }
    if (!is_found) {
      PrsmStrPtrVec new_protein;
      new_protein.push_back(prsm_ptrs[i]);
      proteins.push_back(new_protein);
      protein_names.push_back(name);
    }
  }

  for (size_t i = 0; i < proteins.size(); i++) {
    for (size_t j = 0; j < proteins[i].size(); j++) {
      proteins[i][j]->setProtId(i);
    }
  }
}

void PrsmFeatureSpecies::setSpeciesId(PrsmStrPtrVec& prsm_ptrs) {
  std::vector<PrsmStrPtrVec> species;
  for (size_t i = 0; i < prsm_ptrs.size(); i++) {
    bool is_found = false;
    PrsmStrPtr cur_ptr = prsm_ptrs[i];
    for (size_t j = 0; j < species.size(); j++) {
      PrsmStrPtr ref_ptr = species[j][0];
      if (cur_ptr->getProtId() == ref_ptr->getProtId()) {
        if (cur_ptr->getPrecFeatureId() == ref_ptr->getPrecFeatureId()) {
          species[j].push_back(cur_ptr);
          is_found = true;
          break;
        }
        if (std::abs(cur_ptr->getAdjustedPrecMass() - ref_ptr->getAdjustedPrecMass()) <= prec_error_tole_) {
          species[j].push_back(cur_ptr);
          is_found = true;
          break;
        }
      } else if (cur_ptr->getProteinMatchSeq() == ref_ptr->getProteinMatchSeq()) {
        species[j].push_back(cur_ptr);
        is_found = true;
        break;
      }
    }
    if (!is_found) {
      PrsmStrPtrVec new_species;
      new_species.push_back(prsm_ptrs[i]);
      species.push_back(new_species);
    }
  }
  for (size_t i = 0; i < species.size(); i++) {
    for (size_t j = 0; j < species[i].size(); j++) {
      species[i][j]->setSpeciesId(i);
    }
  }
}

void PrsmFeatureSpecies::process() {
  std::string base_name = file_util::basename(spec_file_name_);
  std::string input_file_name = base_name + "." + input_file_ext_;
  FastaIndexReaderPtr seq_reader = std::make_shared<FastaIndexReader>(db_file_name_);
  PrsmStrPtrVec prsm_ptrs = PrsmReader::readAllPrsmStrsMatchSeq(input_file_name, seq_reader,
                                                                fix_mod_ptr_vec_);

  prsm_util::addFeatureIDToPrsms(prsm_ptrs, feature_file_name_);

  std::sort(prsm_ptrs.begin(), prsm_ptrs.end(), PrsmStr::cmpEValueInc);
  setProtId(prsm_ptrs);
  setSpeciesId(prsm_ptrs);
  std::sort(prsm_ptrs.begin(), prsm_ptrs.end(), PrsmStr::cmpSpectrumIdIncPrecursorIdInc);
  // output
  std::string output_file_name = base_name + "." + output_file_ext_;
  PrsmXmlWriter writer(output_file_name);
  writer.writeVector(prsm_ptrs);
  writer.close();
}

}  // namespace prot
