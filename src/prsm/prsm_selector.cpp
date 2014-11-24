#include "base/file_util.hpp"
#include "prsm/prsm_reader.hpp"
#include "prsm/prsm_selector.hpp"

namespace prot {

PrsmSelector::PrsmSelector(const std::string &db_file_name,
                           const std::string &spec_file_name,
                           const std::string &in_file_ext,
                           const std::string &out_file_ext, int n_top){
  spec_file_name_ = spec_file_name;
  db_file_name_ = db_file_name;
  input_file_ext_ = in_file_ext;
  output_file_ext_ = out_file_ext;
  n_top_ = n_top;
}

bool containsSameDbSeq(const PrsmStrPtrVec prsm_ptrs, PrsmStrPtr target_prsm_ptr) {
  for(size_t i=0; i< prsm_ptrs.size();i++){
    if(prsm_ptrs[i]->getDbId() == target_prsm_ptr->getDbId()){
      return true;
    }
  }
  return false;
}

PrsmStrPtrVec getTopPrsms(PrsmStrPtrVec &prsm_str_ptrs, int n_top){
  std::sort(prsm_str_ptrs.begin(),prsm_str_ptrs.end(),prsmStrEValueUp);
  int size = prsm_str_ptrs.size();
  int max = size > n_top? n_top:size;
  PrsmStrPtrVec result_ptrs;
  for(int i=0;i<max;i++){
    if(!containsSameDbSeq(result_ptrs, prsm_str_ptrs[i])){
      result_ptrs.push_back(prsm_str_ptrs[i]);
    }
  }
  return result_ptrs;
}

/*
PrsmPtrVec PrsmSelector::getTopPrsms(PrsmPtrVec &prsm_ptrs, int n_top){
  std::sort(prsm_ptrs.begin(),prsm_ptrs.end(),prsmEValueUp);
  int size = prsm_ptrs.size();
  int max = size > n_top? n_top:size;
  PrsmPtrVec result_ptrs;
  for(int i=0;i<max;i++){
    if(!containsSameDbSeq(result_ptrs, prsm_ptrs[i])){
      result_ptrs.push_back(prsm_ptrs[i]);
    }
  }
  return result_ptrs;
}
*/

/*
void PrsmSelector::process(){
  std::string base_name = basename(spec_file_name_);
  std::string input_file_name = base_name+"."+input_file_ext_;
  ProteoformPtrVec proteo_ptrs 
      = readFastaToProteoform(db_file_name_, ResidueFactory::getBaseResiduePtrVec());
  PrsmPtrVec prsm_ptrs = readPrsm(input_file_name,proteo_ptrs);
  std::string output_file_name = base_name+"."+output_file_ext_;
  int max_id = prsm_ptrs[prsm_ptrs.size()-1]->getSpectrumId();
  PrsmWriter writer(output_file_name);
  for(int i=0; i<= max_id;i++){
    PrsmPtrVec selected_prsm_ptrs;
    for(size_t j=0;j<prsm_ptrs.size();j++){
      if(prsm_ptrs[j]->getSpectrumId()==i){
        selected_prsm_ptrs.push_back(prsm_ptrs[j]);
      }
    }
    PrsmPtrVec result_ptrs = getTopPrsms(selected_prsm_ptrs, n_top_);
    writer.writeVector(result_ptrs);
  }
  //because the prsm_writer ~PrsmWriter changed and the fileclosing is an independant function
  writer.close();
}
*/

void PrsmSelector::process(){
  std::string base_name = basename(spec_file_name_);
  std::string input_file_name = base_name+"."+input_file_ext_;
  PrsmReader reader(input_file_name);
  PrsmStrPtr prsm_str_ptr = reader.readOnePrsmStr();

  PrsmWriter writer(base_name +"."+output_file_ext_);
  
  int spec_id = 0;
  while (prsm_str_ptr != nullptr) {
    PrsmStrPtrVec cur_str_ptrs;
    while (prsm_str_ptr != nullptr && prsm_str_ptr->getSpectrumId() == spec_id) {
      cur_str_ptrs.push_back(prsm_str_ptr);
      prsm_str_ptr = reader.readOnePrsmStr();
    }
    PrsmStrPtrVec result_ptrs = getTopPrsms(cur_str_ptrs, n_top_);
    for (size_t i = 0; i < result_ptrs.size(); i++) {
      writer.write(result_ptrs[i]);
    }

    spec_id++;
  }
  
  reader.close();
  writer.close();
}

} /* namespace prot */
