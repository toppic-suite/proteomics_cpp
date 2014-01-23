#include "base/logger.hpp"
#include "base/proteoform.hpp"
#include "base/fasta_reader.hpp"
#include "spec/msalign_reader.hpp"
#include "prsm/prsm_writer.hpp"
#include "zeroptmsearch/zero_ptm_fast_match.hpp"
#include "zeroptmsearch/zero_ptm_slow_match.hpp"
#include "zeroptmsearch/zero_ptm_search.hpp"

namespace prot {

void zeroPtmSearch(SpectrumSetPtr spec_set_ptr, int type,
                   ProteoformPtrVec &proteoform_ptr_vec, ZeroPtmMngPtr mng_ptr, 
                   PrSMPtrVec &prsms) {
  ExtendMsPtr ms_three = spec_set_ptr->getSpThree();

  ZpFastMatchPtrVec fast_matches = zeroPtmFastFilter(type, ms_three,
                                                     proteoform_ptr_vec, 
                                                     mng_ptr->zero_ptm_filter_result_num_);

  LOG_DEBUG("fast_match ended size " << fast_matches.size());
  DeconvMsPtr deconv_ms = spec_set_ptr->getDeconvMs();
  ZpSlowMatchPtrVec slow_matches = zeroPtmSlowFilter(type, deconv_ms, fast_matches, mng_ptr ); 

  LOG_DEBUG("slow_match ended size " << slow_matches.size());
  for (unsigned int i = 0; i < slow_matches.size(); i++) {
      prsms.push_back(slow_matches[i]->geneResult());
  }
  LOG_DEBUG("prsm generation ended size " << prsms.size());

  std::sort(prsms.begin(), prsms.end(), prsm_match_fragment_down);
  if (prsms.size() > 0) {
    prsms.erase(prsms.begin() + 1, prsms.end());
  }
}

void zeroPtmSearchProcess(ZeroPtmMngPtr mng_ptr) {
  BaseDataPtr base_data_ptr = mng_ptr->base_data_ptr_;
  
  ProteoformPtrVec raw_forms = readFastaToProteoform(mng_ptr->search_db_file_name_,
                                                     AcidFactory::getAcidPtrVec(),
                                                     base_data_ptr->getFixModResiduePtrVec(),
                                                     base_data_ptr->getDefaultProtModPtr());
  ProteoformPtrVec prot_mod_forms 
      = generateProtModProteoform(raw_forms, 
                                  base_data_ptr->getResiduePtrVec(),
                                  base_data_ptr->getAllowProtModPtrVec());

  int spectra_num = countSpNum (mng_ptr->spectrum_file_name_.c_str(), 
                                base_data_ptr->getActivationPtrVec());
  LOG_DEBUG("spectra_number  " << spectra_num);

  MsAlignReader reader(mng_ptr->spectrum_file_name_.c_str(), 
                       base_data_ptr->getActivationPtrVec());
  std::string output_file_name = basename(mng_ptr->spectrum_file_name_) 
                                          + "." + mng_ptr->output_file_ext_;
  PrSMWriter comp_writer(output_file_name + "_COMPLETE");
  PrSMWriter pref_writer(output_file_name + "_PREFIX");
  PrSMWriter suff_writer(output_file_name + "_SUFFIX");
  PrSMWriter internal_writer(output_file_name + "_INTERNAL");
  PrSMWriter all_writer(output_file_name);

  ProtModPtrVec prot_mod_ptr_list = base_data_ptr->getProtModPtrVec();
  double shift = base_data_ptr->getAcetylationProtModPtr()->getProtShift();
  IonTypePtrVec ion_type_ptr_list = IonTypeFactory::getIonTypePtrVec();
  LOG_DEBUG("start reading");
  int n = 0;
  DeconvMsPtr ms_ptr = reader.getNextMs();
  LOG_DEBUG("init ms_ptr");

  while (ms_ptr.get() != nullptr) {
    n++;
    SpectrumSetPtr spec_set_ptr = getSpectrumSet(ms_ptr, 0, mng_ptr->sp_para_ptr_, 
                                                 shift, ion_type_ptr_list);
    if (spec_set_ptr.get() != nullptr) {
      PrSMPtrVec comp_prsms;
      zeroPtmSearch(spec_set_ptr, SEMI_ALIGN_TYPE_COMPLETE, prot_mod_forms, 
                    mng_ptr, comp_prsms);
      comp_writer.writeVector(comp_prsms);
      all_writer.writeVector(comp_prsms);
      PrSMPtrVec pref_prsms;
      zeroPtmSearch(spec_set_ptr, SEMI_ALIGN_TYPE_PREFIX, prot_mod_forms, 
                    mng_ptr, pref_prsms);
      pref_writer.writeVector(pref_prsms);
      all_writer.writeVector(pref_prsms);
      PrSMPtrVec suff_prsms;
      zeroPtmSearch(spec_set_ptr, SEMI_ALIGN_TYPE_SUFFIX, raw_forms, 
                    mng_ptr, suff_prsms);
      suff_writer.writeVector(suff_prsms);
      all_writer.writeVector(suff_prsms);
      PrSMPtrVec internal_prsms;
      zeroPtmSearch(spec_set_ptr, SEMI_ALIGN_TYPE_SUFFIX, raw_forms, 
                    mng_ptr, internal_prsms);
      internal_writer.writeVector(internal_prsms);
      all_writer.writeVector(internal_prsms);
      LOG_DEBUG("zero ptm search complete " << n);
    }
    ms_ptr = reader.getNextMs();
    LOG_DEBUG("spectrum " << n);
  }

  reader.close();
  std::cout << "Non-ptm search finished." << std::endl;
}

} // end namespace
