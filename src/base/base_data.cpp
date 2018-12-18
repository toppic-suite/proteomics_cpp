//Copyright (c) 2014 - 2018, The Trustees of Indiana University.
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
#include <map>

#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem.hpp>
#undef BOOST_NO_CXX11_SCOPED_ENUMS

#include "base/base_data.hpp"
#include "util/logger.hpp"
#include "base/xml_dom_document.hpp"
#include "base/string_util.hpp"
#include "base/xml_dom_util.hpp"
#include "util/file_util.hpp"

#include "base/amino_acid_base.hpp"
#include "base/ptm_base.hpp"
#include "base/residue_base.hpp"
#include "base/trunc_base.hpp"
#include "base/mod_base.hpp"
#include "base/prot_mod_base.hpp"
#include "base/ion_type_base.hpp"
#include "base/neutral_loss_base.hpp"
#include "base/activation_base.hpp"
#include "base/support_peak_type_base.hpp"

namespace toppic {

namespace base_data {

bool base_data_init_ = false;

// The shared resource directory is provided in the cmake configuration file
std::string shared_resource_dir = "/usr/share/toppic";

std::string getBaseDir(const std::string &resource_dir, const std::string &separator) {
  std::string base_data_dir = resource_dir + separator + base_data::getBaseDataDirName();
  if (boost::filesystem::exists(base_data_dir)) {
    return base_data_dir;
  }
// TOPPIC_LINUX is defined in the cmake configuration file
#if defined (TOPPIC_LINUX) 
  std::string base_data_dir_2 = shared_resource_dir + separator + base_data::getBaseDataDirName();
  if (boost::filesystem::exists(base_data_dir_2)) {
    return base_data_dir_2;
  }
  LOG_ERROR("The directories " << base_data_dir << " and " << base_data_dir_2 << " do not exist!");
#else
  LOG_ERROR("The directory " << base_data_dir << " does not exist!");
#endif
  exit (1);
}

void init(const std::string & resource_dir) {
  // base data only need to be init once
  if (base_data_init_) { return; }

  std::string separator = file_util::getFileSeparator();
  std::string base_data_dir = getBaseDir(resource_dir, separator);

  XmlDOMParser* parser = XmlDOMParserFactory::getXmlDOMParserInstance();
  if (parser) {
    std::string config_file_name
        = base_data_dir + separator + base_data::getBaseDataConfigFileName();
    LOG_DEBUG("config_file_name: " << config_file_name);
    XmlDOMDocument doc(parser, config_file_name.c_str());
    xercesc::DOMElement* root = doc.getDocumentElement();
    LOG_DEBUG("root " << root);
    std::string acid_file_name = xml_dom_util::getChildValue(root, "acid_list_file_name", 0);
    acid_file_name = base_data_dir + separator + acid_file_name;
    LOG_DEBUG("acid file name: " << acid_file_name);
    AminoAcidBase::initBase(acid_file_name);
    LOG_DEBUG("acid initialized ");

    std::string ptm_file_name = xml_dom_util::getChildValue(root, "ptm_list_file_name", 0);
    ptm_file_name = base_data_dir + separator + ptm_file_name;
    LOG_DEBUG("ptm file name: " << ptm_file_name);
    PtmBase::initBase(ptm_file_name);

    std::string residue_file_name = xml_dom_util::getChildValue(root, "residue_list_file_name", 0);
    residue_file_name = base_data_dir + separator + residue_file_name;
    LOG_DEBUG("residue file name: " << residue_file_name);
    ResidueBase::initBase(residue_file_name);
    LOG_DEBUG("residue initialized");

    std::string trunc_file_name
        = xml_dom_util::getChildValue(root, "trunc_list_file_name", 0);
    trunc_file_name = base_data_dir + separator + trunc_file_name;
    LOG_DEBUG("trunc file name: " << trunc_file_name);
    TruncBase::initBase(trunc_file_name);
    LOG_DEBUG("trunc initialized ");

    std::string mod_file_name
        = xml_dom_util::getChildValue(root, "mod_list_file_name", 0);
    mod_file_name = base_data_dir + separator + mod_file_name;
    LOG_DEBUG("mod file name: " << mod_file_name);
    ModBase::initBase(mod_file_name);
    LOG_DEBUG("mod initialized ");

    std::string prot_mod_file_name
        = xml_dom_util::getChildValue(root, "prot_mod_list_file_name", 0);
    prot_mod_file_name = base_data_dir + separator + prot_mod_file_name;
    LOG_DEBUG("prot mod file name: " << prot_mod_file_name);
    ProtModBase::initBase(prot_mod_file_name);
    LOG_DEBUG("prot mod initialized ");

    std::string ion_type_file_name
        = xml_dom_util::getChildValue(root, "ion_type_list_file_name", 0);
    ion_type_file_name = base_data_dir + separator + ion_type_file_name;
    LOG_DEBUG("ion type file name: " << ion_type_file_name);
    IonTypeBase::initBase(ion_type_file_name);
    LOG_DEBUG("ion type initialized ");

    std::string neutral_loss_file_name
        = xml_dom_util::getChildValue(root, "neutral_loss_list_file_name", 0);
    neutral_loss_file_name = base_data_dir + separator + neutral_loss_file_name;
    LOG_DEBUG("neutral loss file name: " << neutral_loss_file_name);
    NeutralLossBase::initBase(neutral_loss_file_name);
    LOG_DEBUG("neutral loss initialized ");

    std::string activation_file_name
        = xml_dom_util::getChildValue(root, "activation_list_file_name", 0);
    activation_file_name = base_data_dir + separator + activation_file_name;
    LOG_DEBUG("activation file name: " << activation_file_name);
    ActivationBase::initBase(activation_file_name);
    LOG_DEBUG("activation initialized ");

    std::string sp_type_file_name
        = xml_dom_util::getChildValue(root, "support_peak_type_file_name", 0);
    sp_type_file_name = base_data_dir + separator + sp_type_file_name;
    LOG_DEBUG("support_peak_type_file_name: " << sp_type_file_name);
    SPTypeBase::initBase(sp_type_file_name);
    LOG_DEBUG("support peak type initialized ");
  }
  base_data_init_ = true;
}

} // namespace base_data

} // namespace toppic

