// Copyright (c) 2014 - 2016, The Trustees of Indiana University.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation and/or
// other materials provided with the distribution.
//
// Indiana University provides no reassurances that the source code provided does
// not infringe the patent or any other intellectual property rights of any other
// entity. Indiana University disclaims any liability to any recipient for claims
// brought by any other entity based on infringement of intellectual property
// rights or otherwise.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
// ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


#include <sstream>
#include <cmath>

#include "base/activation_base.hpp"
#include "base/logger.hpp"
#include "base/string_util.hpp"
#include "base/xml_dom_util.hpp"
#include "spec/peak_util.hpp"
#include "spec/ms_header.hpp"

namespace prot {

MsHeader::MsHeader(xercesc::DOMElement* element){
  file_name_ = XmlDomUtil::getChildValue(element,"file_name",0);
  id_ = XmlDomUtil::getIntChildValue(element,"id",0);
  prec_id_ = XmlDomUtil::getIntChildValue(element,"prec_id",0);
  title_= XmlDomUtil::getChildValue(element,"title",0);
  level_ = XmlDomUtil::getIntChildValue(element,"level",0);

  xercesc::DOMElement* scan_element 
      = XmlDomUtil::getChildElement(element,"scan_list",0);
  int scans = XmlDomUtil::getChildCount(scan_element,"scan");
  for(int i=0;i<scans;i++){
    scans_.push_back(XmlDomUtil::getIntChildValue(scan_element,"scan",i));
  }
  retention_time_ = XmlDomUtil::getDoubleChildValue(element,"retention_time",0);
  prec_sp_mz_ = XmlDomUtil::getDoubleChildValue(element,"prec_sp_mz",0);
  prec_mono_mz_ = XmlDomUtil::getDoubleChildValue(element,"prec_mono_mz",0);
  prec_charge_ = XmlDomUtil::getIntChildValue(element,"prec_charge",0);
  std::string element_name = Activation::getXmlElementName();
  xercesc::DOMElement* ac_element 
      = XmlDomUtil::getChildElement(element,element_name.c_str(),0);
  activation_ptr_= ActivationBase::getActivationPtrFromXml(ac_element);
}

double MsHeader::getPrecMonoMass() {
  if (prec_mono_mz_ < 0 || std::isnan(prec_mono_mz_)) {
    LOG_WARN("monoisotopic mass is not initialized");
    return 0.0;
  } 
  else {
    return PeakUtil::compPeakMass(prec_mono_mz_, prec_charge_);
  }
}

double MsHeader::getPrecSpMass() {
  if (prec_sp_mz_ < 0) {
    LOG_WARN("precursor spectrum mass is not initialized");
    return 0.0;
  } else {
    return PeakUtil::compPeakMass(prec_sp_mz_, prec_charge_);
  }
}

double MsHeader::getPrecMonoMassMinusWater() {
  if (prec_mono_mz_ < 0 || std::isnan(prec_mono_mz_)) {
    LOG_WARN("monoisotopic mass is not initialized");
    return 0.0;
  } else {
    return PeakUtil::compPeakMass(prec_mono_mz_, prec_charge_)
        - MassConstant::getWaterMass();
  }
}


std::pair<int,int> MsHeader::getPrecMonoMassMinusWaterError(double ppo, 
                                                            double scale) {
  int mass = (int) std::round(getPrecMonoMassMinusWater() * scale);
  double error_tolerance = getErrorTolerance(ppo);
  int error = (int) std::ceil(error_tolerance*scale);
  std::pair<int,int> result (mass,error);
  return result;
}


std::string MsHeader::toString() {
  std::stringstream tmp;
  tmp << "MS HEADER\n";
  tmp << "==========\n";
  tmp << "Title = " << title_ << "\n";
  tmp << "Scan Number = " << scans_[0] << "\n";
  tmp << "MS Level = " << level_ << "\n";
  tmp << "Activation type = " << activation_ptr_ << "\n";
  tmp << "Precursor Sp Mz = " << prec_sp_mz_ << "\n";
  tmp << "Precursor Charge = " << prec_charge_ << "\n";
  tmp << "Precursro Mono Mz = " << prec_mono_mz_ << "\n";
  return tmp.str();
}

std::string MsHeader::getScansString() {
  std::stringstream scan_list;
  scan_list << scans_[0];
  for (size_t i = 1; i < scans_.size(); i++) {
    scan_list <<  " " << scans_[i];
  }
  return scan_list.str();
}

void MsHeader::setScans(const std::string &s) {
  if (s == "") {
    scans_.clear();
    scans_.push_back(-1);
    return;
  }
  std::vector<std::string> strs = StringUtil::split(s,' '); 
  for (size_t i = 0; i < strs.size(); i++) {
    scans_.push_back(std::stoi(strs[i]));
  }
}

xercesc::DOMElement* MsHeader::getHeaderXml(XmlDOMDocument* xml_doc) {
  // float number precison
  int precison = 4;
  xercesc::DOMElement* element = xml_doc->createElement("ms_header");
  xml_doc->addElement(element, "file_name", file_name_.c_str());
  std::string str = StringUtil::convertToString(id_);
  xml_doc->addElement(element, "id", str.c_str());
  str = StringUtil::convertToString(prec_id_);
  xml_doc->addElement(element, "prec_id", str.c_str());
  xml_doc->addElement(element, "title", title_.c_str());
  str = StringUtil::convertToString(level_);
  xml_doc->addElement(element, "level", str.c_str());
  str = getScansString();
  xml_doc->addElement(element, "scans", str.c_str());
  xercesc::DOMElement* scans = xml_doc->createElement("scan_list");
  for (size_t i = 0; i < scans_.size(); i++) {
    str = StringUtil::convertToString(scans_[i]);
    xml_doc->addElement(scans, "scan", str.c_str());
  }
  element->appendChild(scans);
  str = StringUtil::convertToString(retention_time_);
  xml_doc->addElement(element, "retention_time", str.c_str());
  str = StringUtil::convertToString(prec_sp_mz_);
  xml_doc->addElement(element, "prec_sp_mz", str.c_str());
  str = StringUtil::convertToString(prec_mono_mz_, precison);
  xml_doc->addElement(element, "prec_mono_mz", str.c_str());
  str = StringUtil::convertToString(prec_charge_);
  xml_doc->addElement(element, "prec_charge", str.c_str());
  activation_ptr_->appendNameToXml(xml_doc, element);
  return element;
}

void MsHeader::appendXml(XmlDOMDocument* xml_doc,xercesc::DOMElement* parent){
  parent->appendChild(getHeaderXml(xml_doc));
}

MsHeaderPtr MsHeader::geneMsHeaderPtr(MsHeaderPtr ori_ptr, double new_prec_mass) {
  MsHeaderPtr new_header_ptr(new MsHeader(*ori_ptr.get()));
  double mono_mz = PeakUtil::compMonoMz(new_prec_mass, ori_ptr->getPrecCharge());
  new_header_ptr->setPrecMonoMz(mono_mz);
  return new_header_ptr;
}

bool MsHeader::cmpPrecInteDec(const MsHeaderPtr &a, const MsHeaderPtr &b) {
  return a->getPrecInte() > b->getPrecInte();
}

}
