/*
 * author  Xiaowen Liu
 * date    2013-11-1
 */
#include <base/logger.hpp>

#include "base/base_data.hpp"
#include "base/xml_dom.hpp"
#include "base/xml_dom_document.hpp"

namespace prot {


BaseData::BaseData  (std::string  config_file_name) {

  XmlDOMParser* parser = getXmlDOMInstance();
  if (parser) {
    LOG_DEBUG( "config_file_name: " << config_file_name);
    XmlDOMDocument* doc = new XmlDOMDocument(parser, config_file_name.c_str());
    LOG_DEBUG( "doc " << doc);
    if (doc) {
      xercesc::DOMElement* root = doc->getDocumentElement();
      LOG_DEBUG( "root " << root);
      std::string acid_file_name = getChildValue(root, "acid_list_file_name", 0);
      LOG_DEBUG( "acid file name: " << acid_file_name);
      acid_list_ = getAcidPtrVecInstance(acid_file_name.c_str());
      LOG_DEBUG( "acid initialized ");

      std::string ptm_file_name = getChildValue(root, "ptm_list_file_name", 0);
      LOG_DEBUG( "ptm file name: " << ptm_file_name);
      ptm_list_ = getPtmPtrVecInstance(ptm_file_name.c_str());
      LOG_DEBUG( "ptm initialized");

      std::string residue_file_name = getChildValue(root, "residue_list_file_name", 0);
      LOG_DEBUG( "residue file name: " << residue_file_name);
      residue_list_ = getResiduePtrVecInstance(acid_list_, ptm_list_,
                                               residue_file_name);
      LOG_DEBUG( "residue initialized");

      std::string trunc_file_name = getChildValue(root, "trunc_list_file_name", 0);
      LOG_DEBUG( "trunc file name: " << trunc_file_name);
      trunc_list_ = getTruncPtrVecInstance(acid_list_, trunc_file_name.c_str());
      LOG_DEBUG( "trunc initialized ");

      std::string prot_mod_file_name = getChildValue(root, "prot_mod_list_file_name", 0);
      LOG_DEBUG( "prot mod file name: " << prot_mod_file_name);
      prot_mod_list_ = getProtModPtrVecInstance(acid_list_, ptm_list_, trunc_list_, prot_mod_file_name.c_str());
      LOG_DEBUG( "prot mod initialized ");

      std::string ion_type_file_name = getChildValue(root, "ion_type_list_file_name", 0);
      LOG_DEBUG( "ion type file name: " << ion_type_file_name);
      ion_type_list_ = getIonTypePtrVecInstance(ion_type_file_name.c_str());
      LOG_DEBUG( "ion type initialized ");

      std::string neutral_loss_file_name = getChildValue(root, "neutral_loss_list_file_name", 0);
      LOG_DEBUG( "neutral loss file name: " << neutral_loss_file_name);
      neutral_loss_list_ = getNeutralLossPtrVecInstance(neutral_loss_file_name.c_str());
      LOG_DEBUG( "neutral loss initialized ");

      std::string activation_file_name = getChildValue(root, "activation_list_file_name", 0);
      LOG_DEBUG( "activation file name: " << activation_file_name);
      activation_list_ = getActivationPtrVecInstance(ion_type_list_, activation_file_name.c_str());
      LOG_DEBUG( "activation initialized ");

      std::string fix_mod_residue_file_name = getChildValue(root, "fix_mod_residue_file_name", 0);
      LOG_DEBUG( "fix mod residue file name: " << fix_mod_residue_file_name);
      fix_mod_residue_list_ = getResiduePtrVecInstance(acid_list_, ptm_list_,
                                                       residue_list_, fix_mod_residue_file_name);
      LOG_DEBUG( "fix mod residue initialized ");

      LOG_DEBUG( "allow prot mods initialization ");
      xercesc::DOMElement* parent = getChildElement(root, "allow_prot_mod_list", 0);
      int prot_mod_num = getChildCount(parent, "prot_mod");
      for (int i = 0; i < prot_mod_num; i++) {
        std::string mod_name = getChildValue(parent, "prot_mod", i);
        ProtModPtr ptr = getProtModPtrByName(prot_mod_list_, mod_name);
        allow_prot_mod_list_.push_back(ptr);
      }
      LOG_DEBUG( "allow prot mods initialized ");

      std::string activation_type = getChildValue(root, "activation_type", 0);
      LOG_DEBUG( "acitivation type: " << activation_type);
      activation_ptr_ = getActivationPtrByName(activation_list_, activation_type);
    }
    delete doc;
  }
  // deleting parser is not necessary
}

}

