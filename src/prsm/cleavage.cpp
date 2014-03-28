/*
 * cleavage.cpp
 *
 *  Created on: Feb 24, 2014
 *      Author: xunlikun
 */

#include <prsm/cleavage.hpp>

namespace prot {
Cleavage::Cleavage(int pos){
  pos_=pos;
  exist_n_ion_ = false;
  exist_c_ion_ = false;
}
void Cleavage::appendXml(XmlDOMDocument* xml_doc,xercesc::DOMElement* parent){
  xercesc::DOMElement* element = xml_doc->createElement("character");
      std::string str = "cleavage";
      xml_doc->addElement(element, "type", str.c_str());
      str = convertToString(pos_);
      xml_doc->addElement(element, "position", str.c_str());
      str = convertToString(exist_n_ion_);
      xml_doc->addElement(element, "exist_n_ion", str.c_str());
      str = convertToString(exist_c_ion_);
      xml_doc->addElement(element, "exist_c_ion", str.c_str());

      xercesc::DOMElement* peaks = xml_doc->createElement("matched_peaks");
      for(unsigned int i=0;i<pairs_.size();i++){
        pairs_[i]->appendPeakToXml(xml_doc,peaks);
      }
      element->appendChild(peaks);
      parent->appendChild(element);
}

CleavagePtrVec getProteoCleavage(ProteoformPtr prot,
                                 ExtendMsPtr ms_three,
                                 double min_mass){
  CleavagePtrVec cleavages;
  PeakIonPairPtrVec pairs;
  getPeakIonPairs (prot, ms_three,min_mass, pairs);

  PeakIonPairPtrVec2D peak_list;
  std::vector<bool> n_ion;
  std::vector<bool> c_ion;
//  for(int i=0;i<prot->getResSeqPtr()->getLen()+1;i++){
  for(unsigned int i=0;i<prot->getDbResSeqPtr()->getResidues().size()+1;i++){
    PeakIonPairPtrVec temp;
    peak_list.push_back(temp);
    n_ion.push_back(false);
    c_ion.push_back(false);
  }
//  std::cout<<prot->getStartPos()<<"->"<<prot->getEndPos()<<std::endl;
//  std::cout<<"S"<<peak_list.size()<<std::endl;
  for(unsigned int i=0;i<pairs.size();i++){
    int pos = pairs[i]->getTheoPeakPtr()->getIonPtr()->getPos()+prot->getStartPos();
//    std::cout<<pairs[i]->getTheoPeakPtr()->getIonPtr()->getPos()<<"+"<<prot->getStartPos()<<std::endl;
    peak_list[pos].push_back(pairs[i]);
    if(pairs[i]->getTheoPeakPtr()->getIonPtr()->getIonTypePtr()->isNTerm()){
      n_ion[pos] = true;
    }
    else{
      c_ion[pos] = true;
    }
  }

  for(int i=0;i< (int)prot->getDbResSeqPtr()->getResidues().size()+1;i++){
    CleavagePtr cleavage = CleavagePtr(new Cleavage(i));
    cleavage->setPairs(peak_list[i]);
    cleavage->setExistCIon(c_ion[i]);
    cleavage->setExistNIon(n_ion[i]);
    cleavages.push_back(cleavage);
  }
  return cleavages;
}


} /* namespace prot */
