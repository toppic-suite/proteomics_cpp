#ifndef PROT_LOCAL_ANNO_HPP_
#define PROT_LOCAL_ANNO_HPP_

#include <vector>
#include "base/ptm.hpp"
#include "base/ptm_base.hpp"
#include "base/xml_dom_document.hpp"

namespace prot {

class LocalAnno {
 public:
  LocalAnno(xercesc::DOMElement* element);

  LocalAnno(int left_pos, int right_pos, double conf, 
            std::vector<double> scr_vec, double raw_scr, PtmPtr p):
      left_pos_(left_pos), right_pos_(right_pos), 
      conf_(conf), scr_vec_(scr_vec), raw_scr_(raw_scr), ptm_ptr_(p){}

  int getLeftBpPos() {return left_pos_;}

  int getRightBpPos() {return right_pos_;}
  // get the confidence score
  double getConf() {return conf_;}
  // get the score vector containing score for each site
  std::vector<double> getScrVec() {return scr_vec_;}

  PtmPtr getPtmPtr() {return ptm_ptr_;}

  double getRawScr() {return raw_scr_;};

  void getRawScr(double s) {raw_scr_ = s;};

  double getMassShift() {return mass_shift_;}

  void setMassShift(double m) {mass_shift_ = m;}

  static std::string getXmlElementName() {return "localization_annotation";}

  void appendToXml(XmlDOMDocument* xml_doc,xercesc::DOMElement* parent);

 private:
  // left and right position filtered by thredshould
  int left_pos_, right_pos_;
  // confidence score
  double conf_;
  // score for each possible site
  std::vector<double> scr_vec_;
  // raw score
  double raw_scr_;
  // PTM
  PtmPtr ptm_ptr_;
  // mass shift
  double mass_shift_;
};

typedef std::shared_ptr<LocalAnno> LocalAnnoPtr;
typedef std::vector<LocalAnnoPtr> LocalAnnoPtrVec;

} // namespace prot

#endif