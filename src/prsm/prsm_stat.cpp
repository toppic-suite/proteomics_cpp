#include <iostream>
#include <fstream>
#include <iomanip>

#include <boost/algorithm/string.hpp>

#include "base/file_util.hpp"
#include "spec/peak.hpp"
#include "spec/msalign_reader.hpp"
#include "prsm/prsm_reader.hpp"
#include "prsm/peak_ion_pair.hpp"
#include "prsm/prsm_stat.hpp"

namespace prot {

PrsmStat::PrsmStat(PrsmParaPtr prsm_para_ptr, 
                   const std::string &input_file_ext, 
                   const std::string &output_file_ext) {
  prsm_para_ptr_ = prsm_para_ptr;
  min_mass_ = prsm_para_ptr_->getSpParaPtr()->getMinMass();
  input_file_ext_ = input_file_ext;
  output_file_ext_ = output_file_ext;
  acid_ptr_vec_ = AcidFactory::getBaseAcidPtrVec();
}

int countCoverage(const std::vector<bool> &match_ion_vec, int start, int end) {
  int count = 0;
  for (size_t i = 0; i < match_ion_vec.size(); i++) {
    if ((int)i >= start && (int)i < end && match_ion_vec[i]) {
      count++;
    }
  }
  return count;
}

int countAcid(ResSeqPtr res_seq_ptr, AcidPtr acid_ptr) {
  int count = 0;
  for (int i = 0; i < res_seq_ptr->getLen(); i++) {
    if (res_seq_ptr->getResiduePtr(i)->getAcidPtr() == acid_ptr) {
      count++;
    }
  }
  return count;
}

int countAcidLeftCoverage(ResSeqPtr res_seq_ptr, AcidPtr acid_ptr, 
                          const std::vector<bool> &match_ion_vec) {
  int count = 0;
  for (int i = 0; i < res_seq_ptr->getLen(); i++) {
    if (res_seq_ptr->getResiduePtr(i)->getAcidPtr() == acid_ptr && match_ion_vec[i]) {
      count++;
    }
  }
  return count;
}

int countAcidRightCoverage(ResSeqPtr res_seq_ptr, AcidPtr acid_ptr, 
                          const std::vector<bool> &match_ion_vec) {
  int count = 0;
  for (int i = 0; i < res_seq_ptr->getLen(); i++) {
    if (res_seq_ptr->getResiduePtr(i)->getAcidPtr() == acid_ptr && match_ion_vec[i+1]) {
      count++;
    }
  }
  return count;
}

void PrsmStat::writePrsm(std::ofstream &file, PrsmPtr prsm_ptr) {
  std::string spec_ids;
  std::string spec_activations;
  std::string spec_scans;
  int peak_num = 0;
  DeconvMsPtrVec deconv_ms_ptr_vec = prsm_ptr->getDeconvMsPtrVec();
  for (size_t i = 0; i < deconv_ms_ptr_vec.size(); i++) {
    spec_ids = spec_ids + std::to_string(deconv_ms_ptr_vec[i]->getHeaderPtr()->getId()) + " ";
    spec_activations = spec_activations + deconv_ms_ptr_vec[i]->getHeaderPtr()->getActivationPtr()->getName() + " ";
    spec_scans = spec_scans + deconv_ms_ptr_vec[i]->getHeaderPtr()->getScansString() + " ";
    peak_num += deconv_ms_ptr_vec[i]->size();
  }
  boost::algorithm::trim(spec_ids);
  boost::algorithm::trim(spec_activations);
  boost::algorithm::trim(spec_scans);
  file << prsm_para_ptr_->getSpectrumFileName() << "\t"
      << prsm_ptr->getId() << "\t"
      << spec_ids << "\t"
      << spec_activations<< "\t"
      << spec_scans << "\t"
      << peak_num << "\t"
      << deconv_ms_ptr_vec[0]->getHeaderPtr()->getPrecCharge() << "\t"
      << prsm_ptr->getOriPrecMass()<< "\t"//"Precursor_mass"
      << prsm_ptr->getAdjustedPrecMass() << "\t"
      << prsm_ptr->getProteoformPtr()->getDbResSeqPtr()->getId() << "\t"
      << prsm_ptr->getProteoformPtr()->getSpeciesId() << "\t"
      << prsm_ptr->getProteoformPtr()->getDbResSeqPtr()->getName() << " "
      << prsm_ptr->getProteoformPtr()->getDbResSeqPtr()->getDesc() << "\t"
      << prsm_ptr->getProteoformPtr()->getDbResSeqPtr()->getSeqMass() << "\t"
      << prsm_ptr->getProteoformPtr()->getStartPos() << "\t"
      << prsm_ptr->getProteoformPtr()->getEndPos() << "\t"
      << prsm_ptr->getProteoformPtr()->getProteinMatchSeq() << "\t"
      << prsm_ptr->getProteoformPtr()->getUnexpectedChangeNum() << "\t"
      << prsm_ptr->getMatchPeakNum() << "\t"
      << prsm_ptr->getMatchFragNum() << "\t"
      << prsm_ptr->getPValue() << "\t"
      << prsm_ptr->getEValue() << "\t"
      << prsm_ptr->getOneProtProb()<< "\t"
      << prsm_ptr->getFdr() << "\t";

  ProteoformPtr proteo_ptr = prsm_ptr->getProteoformPtr();
  ExtendMsPtrVec refine_ms_ptr_vec = prsm_ptr->getRefineMsPtrVec();
  int proteo_len = proteo_ptr->getEndPos() - proteo_ptr->getStartPos() + 1;
  int third_start = 1;
  int third_first = proteo_len /3 + 1;
  int third_second = proteo_len * 2 / 3 + 1;
  int third_end = proteo_len;

  file << (proteo_len - 1) << "\t";
  file << (third_first - third_start) << "\t";
  file << (third_second - third_first) << "\t";
  file << (third_end - third_second) << "\t";

  std::vector<bool> comb_n_ion (proteo_len + 1, false);
  std::vector<bool> comb_c_ion (proteo_len + 1, false);
  std::vector<bool> comb_both_ion (proteo_len + 1, false);

  std::vector<std::vector<bool>> n_ion_2d;
  std::vector<std::vector<bool>> c_ion_2d;
  std::vector<std::vector<bool>> both_ion_2d;
  for (size_t s = 0; s < deconv_ms_ptr_vec.size(); s++) {
    //get ion_pair
    PeakIonPairPtrVec pair_ptrs =  getPeakIonPairs (prsm_ptr->getProteoformPtr(), 
                                                    refine_ms_ptr_vec[s],
                                                    min_mass_);
    std::vector<bool> n_ion (proteo_len + 1, false);
    std::vector<bool> c_ion (proteo_len + 1, false);
    std::vector<bool> both_ion (proteo_len + 1, false);
    for(size_t p = 0; p<pair_ptrs.size(); p++){
      int pos = pair_ptrs[p]->getTheoPeakPtr()->getIonPtr()->getPos();
      //LOG_DEBUG("start pos " << prot_ptr->getStartPos() << " pos " << pos);
      if(pair_ptrs[p]->getTheoPeakPtr()->getIonPtr()->getIonTypePtr()->isNTerm()){
        n_ion[pos] = true;
        both_ion[pos] = true;
        comb_n_ion[pos] = true;
        comb_both_ion[pos] = true;
      }
      else{
        c_ion[pos] = true;
        both_ion[pos] = true;
        comb_c_ion[pos] = true;
        comb_both_ion[pos] = true;
      }
    }
    n_ion_2d.push_back(n_ion);
    c_ion_2d.push_back(c_ion);
    both_ion_2d.push_back(both_ion);
  }

  for (size_t s = 0; s < deconv_ms_ptr_vec.size(); s++) {
    file << countCoverage(n_ion_2d[s], third_start, third_end) << "\t";
    file << countCoverage(n_ion_2d[s], third_start, third_first) << "\t";
    file << countCoverage(n_ion_2d[s], third_first, third_second) << "\t";
    file << countCoverage(n_ion_2d[s], third_second, third_end) << "\t";

    file << countCoverage(c_ion_2d[s], third_start, third_end) << "\t";
    file << countCoverage(c_ion_2d[s], third_start, third_first) << "\t";
    file << countCoverage(c_ion_2d[s], third_first, third_second) << "\t";
    file << countCoverage(c_ion_2d[s], third_second, third_end) << "\t";

    file << countCoverage(both_ion_2d[s], third_start, third_end) << "\t";
    file << countCoverage(both_ion_2d[s], third_start, third_first) << "\t";
    file << countCoverage(both_ion_2d[s], third_first, third_second) << "\t";
    file << countCoverage(both_ion_2d[s], third_second, third_end) << "\t";
  }

  file << countCoverage(comb_n_ion, third_start, third_end) << "\t";
  file << countCoverage(comb_n_ion, third_start, third_first) << "\t";
  file << countCoverage(comb_n_ion, third_first, third_second) << "\t";
  file << countCoverage(comb_n_ion, third_second, third_end) << "\t";

  file << countCoverage(comb_c_ion, third_start, third_end) << "\t";
  file << countCoverage(comb_c_ion, third_start, third_first) << "\t";
  file << countCoverage(comb_c_ion, third_first, third_second) << "\t";
  file << countCoverage(comb_c_ion, third_second, third_end) << "\t";

  file << countCoverage(comb_both_ion, third_start, third_end) << "\t";
  file << countCoverage(comb_both_ion, third_start, third_first) << "\t";
  file << countCoverage(comb_both_ion, third_first, third_second) << "\t";
  file << countCoverage(comb_both_ion, third_second, third_end) << "\t";

  ResSeqPtr res_seq_ptr = proteo_ptr->getResSeqPtr();

  for (size_t i = 0; i < acid_ptr_vec_.size(); i++) {
    AcidPtr acid = acid_ptr_vec_[i];
    file << countAcid(res_seq_ptr, acid) << "\t";
    for (size_t j = 0; j < deconv_ms_ptr_vec.size(); j++) {
      file << countAcidLeftCoverage(res_seq_ptr, acid, n_ion_2d[j]) << "\t";
      file << countAcidLeftCoverage(res_seq_ptr, acid, c_ion_2d[j]) << "\t";
      file << countAcidLeftCoverage(res_seq_ptr, acid, both_ion_2d[j]) << "\t";

      file << countAcidRightCoverage(res_seq_ptr, acid, n_ion_2d[j]) << "\t";
      file << countAcidRightCoverage(res_seq_ptr, acid, c_ion_2d[j]) << "\t";
      file << countAcidRightCoverage(res_seq_ptr, acid, both_ion_2d[j]) << "\t";
    }

    file << countAcidLeftCoverage(res_seq_ptr, acid, comb_n_ion) << "\t";
    file << countAcidLeftCoverage(res_seq_ptr, acid, comb_c_ion) << "\t";
    file << countAcidLeftCoverage(res_seq_ptr, acid, comb_both_ion) << "\t";

    file << countAcidRightCoverage(res_seq_ptr, acid, comb_n_ion) << "\t";
    file << countAcidRightCoverage(res_seq_ptr, acid, comb_c_ion) << "\t";
    file << countAcidRightCoverage(res_seq_ptr, acid, comb_both_ion) << "\t";
  }

  file << std::endl;
}

void PrsmStat::process() {
  std::string spectrum_file_name  = prsm_para_ptr_->getSpectrumFileName(); 
  std::string base_name = basename(spectrum_file_name);
  std::string output_file_name = base_name + "." + output_file_ext_;
  std::ofstream file; 
  file.open(output_file_name.c_str());
  //write title
  file << "Data_file_name" << "\t"
      << "Prsm_ID" << "\t"
      << "Spectrum_ID"<< "\t"
      << "Activation_type" << "\t"
      << "Scan(s)" << "\t"
      << "#peaks"<< "\t"
      << "Charge" << "\t"
      << "Precursor_mass" << "\t"
      << "Adjusted_precursor_mass" << "\t"
      << "Protein_ID" << "\t"
      << "Species_ID" << "\t"
      << "Protein_name" << "\t"
      << "Protein_mass" << "\t"
      << "First_residue" << "\t"
      << "Last_residue" << "\t"
      << "Peptide" << "\t"
      << "#unexpected_modifications" << "\t"
      << "#matched_peaks" << "\t"
      << "#matched_fragment_ions" << "\t"
      << "P-Value" << "\t"
      << "E-Value" << "\t"
      << "One_Protein_probabilty"<< "\t"
      << "FDR" << "\t";
  int group_spec_num = prsm_para_ptr_->getGroupSpecNum();

  file << "Total break point number " << "\t"
      << "Left break point number " <<  "\t"
      << "Middle break point number " <<  "\t"
      << "Right break point number " <<  "\t";
  for (int i = 1; i <= group_spec_num; i++) {
    file << "Spectrum " << i << " N_term_ion_coverage" << "\t"
        << "Spectrum " << i << " left_N_term_ion_coverage" << "\t"
        << "Spectrum " << i << " middle_N_term_ion_coverage" << "\t"
        << "Spectrum " << i << " right_N_term_ion_coverage" << "\t"

        << "Spectrum " << i << " C_term_ion_coverage" << "\t"
        << "Spectrum " << i << " left_C_term_ion_coverage" << "\t"
        << "Spectrum " << i << " middle_C_term_ion_coverage" << "\t"
        << "Spectrum " << i << " right_C_term_ion_coverage" << "\t"

        << "Spectrum " << i << " Both_term_ion_coverage" << "\t"
        << "Spectrum " << i << " left_Both_term_ion_coverage" << "\t"
        << "Spectrum " << i << " middle_both_term_ion_coverage" << "\t"
        << "Spectrum " << i << " right_Both_term_ion_coverage" << "\t";
  }

  file << "Combined spectra N_term_ion_coverage" << "\t"
      << "Combined spectra left_N_term_ion_coverage" << "\t"
      << "Combined spectra middle_N_term_ion_coverage" << "\t"
      << "Combined spectra right_N_term_ion_coverage" << "\t"

      << "Combined spectra C_term_ion_coverage" << "\t"
      << "Combined spectra left_C_term_ion_coverage" << "\t"
      << "Combined spectra middle_C_term_ion_coverage" << "\t"
      << "Combined spectra right_C_term_ion_coverage" << "\t"

      << "Combined spectra both_term_ion_coverage" << "\t"
      << "Combined spectra left_both_term_ion_coverage" << "\t"
      << "Combined spectra middle_both_term_ion_coverage" << "\t"
      << "Combined spectra right_both_term_ion_coverage" << "\t";

  for (size_t i = 0; i < acid_ptr_vec_.size(); i++) {
    std::string acid = acid_ptr_vec_[i]->getOneLetter();
    file << acid << " number" << "\t";
    for (int j = 0; j < group_spec_num; j++) {
      file << "Spectrum " << j << " " << acid << " left N term" << "\t";
      file << "Spectrum " << j << " " << acid << " left C term" << "\t";
      file << "Spectrum " << j << " " << acid << " left both term" << "\t";
      file << "Spectrum " << j << " " << acid << " right N term" << "\t";
      file << "Spectrum " << j << " " << acid << " right C term" << "\t";
      file << "Spectrum " << j << " " << acid << " right both term" << "\t";
    }
    file << "Combined spectra " << acid << " left N term" << "\t";
    file << "Combined spectra " << acid << " left C term" << "\t";
    file << "Combined spectra " << acid << " left both term" << "\t";
    file << "Combined spectra " << acid << " right N term" << "\t";
    file << "Combined spectra " << acid << " right C term" << "\t";
    file << "Combined spectra " << acid << " right both term" << "\t";
  }

  file << std::endl;

  std::string input_file_name = basename(spectrum_file_name) + "." + input_file_ext_;
  std::string db_file_name = prsm_para_ptr_->getSearchDbFileName();
  ResiduePtrVec residue_ptr_vec = prsm_para_ptr_->getFixModResiduePtrVec();

  PrsmPtrVec prsm_ptrs = readAllPrsms(input_file_name, db_file_name,
                                      residue_ptr_vec);
  LOG_DEBUG("prsm loaded");

  addSpectrumPtrsToPrsms(prsm_ptrs, prsm_para_ptr_);
  LOG_DEBUG("spectrum added");

  std::sort(prsm_ptrs.begin(), prsm_ptrs.end(), prsmSpectrumIdUpPrecursorIdUp); 

  for (size_t i = 0; i < prsm_ptrs.size(); i++) {
    writePrsm(file, prsm_ptrs[i]);
  }
  //write end;
  file.close();
}

} /* namespace prot */