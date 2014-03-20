/*
 * output_selector.cpp
 *
 *  Created on: Feb 19, 2014
 *      Author: xunlikun
 */

#include <prsm/output_selector.hpp>

namespace prot {
OutputSelector::OutputSelector(std::string db_file,
                 std::string spec_file,
                 std::string input_file,
                 std::string output_file,
                 std::string cutoff_type,
                 double evalue_thresh,
                 double fdr_thresh,
                 double ppo){
  db_file_= db_file;
  spec_file_ = spec_file;
  input_file_=input_file;
  output_file_ = output_file;
  cutoff_type_ = cutoff_type;
  evalue_thresh_ = evalue_thresh;
  fdr_thresh_= fdr_thresh;
  ppo_ = ppo;
}

OutputSelector::OutputSelector(std::map<std::string,std::string> arguments,
                 std::string input_file,
                 std::string output_file
                 ){
  db_file_= arguments["databaseFileName"];
  spec_file_ = arguments["spectrumFileName"];
  input_file_=input_file;
  output_file_ = output_file;
  cutoff_type_ = arguments["cutoffType"];
  evalue_thresh_ = atof(arguments["cutoff"].c_str());
  fdr_thresh_= atof(arguments["cutoff"].c_str());
  ppo_ = atoi(arguments["errorTolerance"].c_str())*0.000001;
}

void OutputSelector::process(){
  std::string base_name = basename(spec_file_);
  std::string input_file_name = base_name + "." + input_file_;
  ProteoformPtrVec proteoforms = readFastaToProteoform(db_file_,ResidueFactory::getBaseResiduePtrVec());
  PrSMPtrVec prsms = readPrsm(input_file_name,proteoforms);
  // it's no need to process prsm

  //select
  sort(prsms.begin(),prsms.end(),prsm_spectrum);
  bool evalue_cutoff = (cutoff_type_.compare("EVALUE")==0);

  PrSMPtrVec selected_prsm ;
  ProteoformPtrVec selected_protein;
  int id =0;
  for(unsigned int i=0;i<prsms.size();i++){
    if(evalue_cutoff && prsms[i]->getEValue() <= evalue_thresh_){
      prsms[i]->setId(id);
      selected_prsm.push_back(prsms[i]);
      selected_protein.push_back(prsms[i]->getProteoformPtr());
      id++;
    }
    else if(!evalue_cutoff && prsms[i]->getFdr() <= fdr_thresh_){
      prsms[i]->setId(id);
      selected_prsm.push_back(prsms[i]);
      selected_protein.push_back(prsms[i]->getProteoformPtr());
      id++;
    }
  }

  //species id

  //output
  std::string output_file_name = base_name +"."+output_file_;
  PrSMWriter writer(output_file_name);
  writer.writeVector(selected_prsm);

  //because the prsm_writer ~PrSMWriter changed and the fileclosing is an independant function
  writer.close();
}
} /* namespace prot */
