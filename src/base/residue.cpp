#include <base/logger.hpp>

#include "base/residue.hpp"
#include "base/xml_dom.hpp"
#include "base/xml_dom_document.hpp"

namespace prot {

Residue::Residue(AcidPtr acid_ptr, PtmPtr ptm_ptr) {
  acid_ptr_ = acid_ptr;
  ptm_ptr_ = ptm_ptr;
  mass_ = acid_ptr->getMonoMass() + ptm_ptr->getMonoMass();
}

Residue::Residue(AcidPtrVec acid_list, PtmPtrVec ptm_list,
          std::string acid_name, std::string ptm_abbr_name) {
  acid_ptr_ = getAcidPtrByName(acid_list, acid_name);
  ptm_ptr_ = getPtmPtrByAbbrName(ptm_list, ptm_abbr_name);
  mass_ = acid_ptr_->getMonoMass() + ptm_ptr_->getMonoMass();
}

std::string Residue::toString(std::string delim_bgn, std::string delim_end) {
  if (ptm_ptr_->isEmpty()) {
    return acid_ptr_->getOneLetter();
  } else {
    return acid_ptr_->getOneLetter() + delim_bgn + ptm_ptr_->getAbbrName()
        + delim_end;
  }
}

void Residue::appendXml(XmlDOMDocument* xml_doc,xercesc::DOMElement* parent){
	xercesc::DOMElement* element = xml_doc->createElement("residue");
	std::string str = convertToString(mass_);
	xml_doc->addElement(element, "mass", str.c_str());
	acid_ptr_->appendxml(xml_doc,element);
	ptm_ptr_->appendxml(xml_doc,element);
	parent->appendChild(element);
}

ResiduePtr getResiduePtrByAcid(ResiduePtrVec &residue_ptrs,
                                  AcidPtr acid_ptr) {
  for (unsigned int i = 0; i < residue_ptrs.size(); i++) {
    if (residue_ptrs[i]->getAcidPtr().get() == acid_ptr.get()) {
      return residue_ptrs[i];
    }
  }
  return ResiduePtr(nullptr);
}

ResiduePtr getResiduePtrByAcidPtm(ResiduePtrVec &residue_list,
                                  AcidPtr acid_ptr, PtmPtr ptm_ptr) {
  for (unsigned int i = 0; i < residue_list.size(); i++) {
    if (residue_list[i]->isSame(acid_ptr, ptm_ptr)) {
      return residue_list[i];
    }
  }
  return ResiduePtr(nullptr);
}

ResiduePtrVec getResiduePtrVecInstance(AcidPtrVec &acid_list, 
                                       PtmPtrVec &ptm_list,
                                       std::string file_name) {
  ResiduePtrVec residue_list;
  XmlDOMParser* parser = XmlDOMParserFactory::getXmlDOMParserInstance();
  if (parser) {
    XmlDOMDocument doc(parser, file_name.c_str());
    xercesc::DOMElement* parent = doc.getDocumentElement();
    int residue_num = getChildCount(parent, "residue");
    LOG_DEBUG( "residue num " << residue_num);
    for (int i = 0; i < residue_num; i++) {
      xercesc::DOMElement* element = getChildElement(parent, "residue", i);
      std::string acid_name = getChildValue(element, "acid", 0);
      std::string ptm_abbr_name = getChildValue(element, "ptm", 0);
      LOG_DEBUG( "acid vec " << acid_list.size() << " ptm vec " 
                << ptm_list.size() << " acid " << acid_name << " ptm " << ptm_abbr_name);
      residue_list.push_back(ResiduePtr(
              new Residue(acid_list, ptm_list, acid_name, ptm_abbr_name)));
    }
  }
  return residue_list;
}

ResiduePtrVec getResiduePtrVecInstance(AcidPtrVec &acid_list, 
                                       PtmPtrVec &ptm_list,
                                       ResiduePtrVec &residue_list,
                                       std::string file_name) {
  ResiduePtrVec new_list;
  XmlDOMParser* parser = XmlDOMParserFactory::getXmlDOMParserInstance();
  if (parser) {
    XmlDOMDocument doc(parser, file_name.c_str());
    xercesc::DOMElement* parent = doc.getDocumentElement();
    int residue_num = getChildCount(parent, "residue");
    LOG_DEBUG( "residue num " << residue_num);
    for (int i = 0; i < residue_num; i++) {
      xercesc::DOMElement* element = getChildElement(parent, "residue", i);
      std::string acid_name = getChildValue(element, "acid", 0);
      std::string ptm_abbr_name = getChildValue(element, "ptm", 0);
      LOG_DEBUG( "acid vec " << acid_list.size() << " ptm vec " << ptm_list.size() 
                << " acid " << acid_name << " ptm " << ptm_abbr_name);
      AcidPtr acid_ptr = getAcidPtrByName(acid_list, acid_name);
      if (acid_ptr.get() == nullptr) {
        LOG_ERROR( "acid " << acid_name  << " not found ");
        throw("acid not found");
      }
      PtmPtr ptm_ptr = getPtmPtrByAbbrName(ptm_list, ptm_abbr_name);
      if (ptm_ptr.get() == nullptr) {
        LOG_ERROR( "ptm " << ptm_abbr_name  << " not found ");
        throw("ptm not found");
      }
      ResiduePtr residue_ptr = getResiduePtrByAcidPtm(residue_list, acid_ptr, ptm_ptr);
      if (residue_ptr.get() == nullptr) {
        residue_ptr = addResidue(residue_list, acid_ptr, ptm_ptr);
      }
      new_list.push_back(residue_ptr);
    }
  }
  return new_list;
}


ResiduePtr addResidue(ResiduePtrVec &residue_list, AcidPtr acid_ptr,
                      PtmPtr ptm_ptr) {
  ResiduePtr residue_ptr = getResiduePtrByAcidPtm(residue_list, acid_ptr, ptm_ptr);
  if (residue_ptr.get() == nullptr) {
    ResiduePtr new_ptr(new Residue(acid_ptr, ptm_ptr));
    residue_list.push_back(new_ptr);
    return new_ptr;
  }
  else {
    return residue_ptr;
  }
}

ResiduePtrVec convertAcidToResidueSeq(ResiduePtrVec residue_list,
                                      AcidPtrVec acid_ptrs) {
  ResiduePtrVec result_seq;
  for (unsigned int i = 0; i < acid_ptrs.size(); i++) {
    ResiduePtr residue_ptr = getResiduePtrByAcid(residue_list, acid_ptrs[i]);
    result_seq.push_back(residue_ptr);
  }
  return result_seq;
}

}
