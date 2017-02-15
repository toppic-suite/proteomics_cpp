// Copyright (c) 2014 - 2017, The Trustees of Indiana University.
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


#ifndef PROT_SPEC_MS_HEADER_HPP_
#define PROT_SPEC_MS_HEADER_HPP_

#include <cmath>

#include "base/activation.hpp"

namespace prot {

class MsHeader;
typedef std::shared_ptr<MsHeader> MsHeaderPtr;

class MsHeader {
 public:
  MsHeader() {};

  MsHeader(xercesc::DOMElement* element);

  double getPrecMonoMass();

  double getPrecSpMass();

  double getPrecMonoMassMinusWater();

  std::pair<int,int> getPrecMonoMassMinusWaterError(double ppo, double scale);

  std::string getScansString();

  int getFirstScanNum() {return scans_[0];}

  void setScans(const std::string &s);

  std::string toString();

  // get functions 
  ActivationPtr getActivationPtr() {return activation_ptr_;}

  double getFeatureInte() {return feature_inte_;}

  std::string getFileName() {return file_name_;}

  int getId() {return id_;}

  int getMsLevel() {return level_;}

  std::string getTitle() {return title_;}

  int getMsOneId() {return ms_one_id_;}

  int getMsOneScan() {return ms_one_scan_;}

  double getPrecSpMz() {return prec_sp_mz_;}

  int getPrecCharge() {return prec_charge_;}

  double getPrecMonoMz() {
    if (std::isnan(prec_mono_mz_)) {
      return 0.0; 
    } else {
      return prec_mono_mz_;
    }
  }

  double getRetentionTime() {return retention_time_;}

  int getPrecId() {return prec_id_;}

  double getErrorTolerance(double ppo) {return getPrecMonoMass() * ppo;}

  double getPrecInte() {return prec_inte_;}

  int getFeatureId() {return feature_id_;}

  // set function 
  void setActivationPtr(ActivationPtr acti_ptr) {activation_ptr_ = acti_ptr;}

  void setFileName(const std::string &file_name) {file_name_ = file_name;}

  void setId(int id) {id_ = id;}

  void setTitle(const std::string &title) {title_ = title;}

  void setMsOneId(int ms_one_id) {ms_one_id_ = ms_one_id;}

  void setMsOneScan(int ms_one_scan) {ms_one_scan_ = ms_one_scan;}

  void setPrecSpMz(double prec_sp_mz) {prec_sp_mz_ = prec_sp_mz;}

  void setPrecCharge(int prec_charge) {prec_charge_ = prec_charge;}

  void setPrecMonoMz(double prec_mono_mz) {prec_mono_mz_ = prec_mono_mz;}

  void setRetentionTime(double retention_time) {retention_time_ = retention_time;}

  void setScan(int scan_num) {scans_.push_back(scan_num);}

  void setScans(const std::vector<int> &scans) {scans_ = scans;}

  void setMsLevel(int level) {level_ = level;}

  void setPrecId(int prec_id) {prec_id_ = prec_id;}

  void setPrecInte(double inte) {prec_inte_ = inte;}

  void setFeatureId(int feature_id) {feature_id_ = feature_id;}

  void setFeatureInte(double feature_inte) {feature_inte_ = feature_inte;}

  xercesc::DOMElement* getHeaderXml(XmlDOMDocument* xml_doc);

  void appendXml(XmlDOMDocument* xml_doc,xercesc::DOMElement* parent);

  static std::string getXmlElementName() {return "ms_header";}

  static MsHeaderPtr geneMsHeaderPtr(MsHeaderPtr ori_ptr, double new_prec_mass);

  static bool cmpPrecInteDec(const MsHeaderPtr &a, const MsHeaderPtr &b);

 private:
  // data set name 
  std::string file_name_;
  // A data set may contain several spectra with the same id, but different
  // precursor id 
  int id_ = -1;
  // one spectrum may have several possible precursor mass */
  int prec_id_ = -1;
  
  std::string title_;
  // a list of scans for merged spectra 
  std::vector<int> scans_;
  // ms level 
  int level_ = 0;
  // activation type 
  ActivationPtr activation_ptr_;
  // ms1 id 
  int ms_one_id_ = -1;
  // ms1 scan number
  int ms_one_scan_ = -1;
  // retention time 
  double retention_time_ = 0.0;
  // precursor m/z value in the MS1 spectrum 
  double prec_sp_mz_ = -1;
  // computed monoisotopic precursor m/z value 
  double prec_mono_mz_ = -1;
  // precursor charge state  
  int prec_charge_ = -1;
  // precursor intensity 
  double prec_inte_ = 0;
  // feature id
  int feature_id_ = -1;
  // feature inte
  double feature_inte_ = -1;
};

typedef std::vector<MsHeaderPtr> MsHeaderPtrVec;
typedef std::vector<MsHeaderPtrVec> MsHeaderPtr2D;

}

#endif
