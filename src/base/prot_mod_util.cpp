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


#include "base/logger.hpp"
#include "base/ptm_base.hpp"
#include "base/mod_base.hpp"
#include "base/prot_mod_base.hpp"
#include "base/prot_mod_util.hpp"
#include "base/trunc_util.hpp"
#include "base/xml_dom_util.hpp"

namespace prot {

bool ProtModUtil::allowMod(ProtModPtr prot_mod_ptr, const ResiduePtrVec &residues){
  if (prot_mod_ptr == ProtModBase::getProtModPtr_NONE()) {
    return true;
  }
  else if (prot_mod_ptr == ProtModBase::getProtModPtr_M_ACETYLATION()) {
    int mod_pos = prot_mod_ptr->getModPos();
    if (mod_pos >= (int)residues.size()) { 
      //LOG_DEBUG("pos false");
      return false;
    }
    ModPtr mod_ptr = prot_mod_ptr->getModPtr();
    if (residues[mod_pos] != mod_ptr->getOriResiduePtr()) {
      //LOG_DEBUG("mod false");
      return false;
    }
    return true;
  }
  else {
    // check trunc
    if (!TruncUtil::isValidTrunc(prot_mod_ptr->getTruncPtr(), residues)) {
      return false;
    }
    ModPtr mod_ptr = prot_mod_ptr->getModPtr();
    if (mod_ptr != ModBase::getNoneModPtr()) {
      // if NME_acetylation
      int mod_pos = prot_mod_ptr->getModPos();
      if (mod_pos >= (int)residues.size()) { 
        //LOG_DEBUG("pos false");
        return false;
      }
      if (residues[mod_pos] != mod_ptr->getOriResiduePtr()) {
        //LOG_DEBUG("mod false");
        return false;
      }
    }
    return true;
  }
}

ProtModPtrVec ProtModUtil::readProtMod(const std::string &file_name) {
  XmlDOMParser* parser = XmlDOMParserFactory::getXmlDOMParserInstance();
  ProtModPtrVec mod_ptr_vec;
  if (parser) {
    XmlDOMDocument doc(parser, file_name.c_str());
    xercesc::DOMElement* parent = doc.getDocumentElement();
    std::string element_name = ProtMod::getXmlElementName();
    int mod_num = XmlDomUtil::getChildCount(parent, element_name.c_str());
    LOG_DEBUG("mod num " << mod_num);
    for (int i = 0; i < mod_num; i++) {
      xercesc::DOMElement* element 
          = XmlDomUtil::getChildElement(parent, element_name.c_str(), i);
      ProtModPtr ptr = ProtModBase::getProtModPtrFromXml(element);
      mod_ptr_vec.push_back(ptr);
    }
  }
  return mod_ptr_vec;
}

ProtModPtr ProtModUtil::findNME_Acetylation(const ProtModPtrVec &prot_mod_ptrs, 
                                            const ResiduePtrVec &residues) {
  for (size_t i = 0; i < prot_mod_ptrs.size(); i++) {
    PtmPtr ptm_ptr = prot_mod_ptrs[i]->getModPtr()->getModResiduePtr()->getPtmPtr();
    //LOG_DEBUG("ptm ptr " << ptm_ptr->getAbbrName() << 
    //          " equal " << (ptm_ptr == PtmBase::getPtmPtr_Acetylation()));
    if (ptm_ptr == PtmBase::getPtmPtr_Acetylation() &&
        allowMod(prot_mod_ptrs[i], residues)) {
      return prot_mod_ptrs[i];
    }
  }
  return nullptr;
}

}
