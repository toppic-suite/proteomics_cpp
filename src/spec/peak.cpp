#include "spec/peak.hpp"

namespace prot {

void Peak::appendXml(XmlDOMDocument* xml_doc,xercesc::DOMElement* parent){
  xercesc::DOMElement* element = xml_doc->createElement("peak");
  std::string str = convertToString(getPosition());
  xml_doc->addElement(element, "position", str.c_str());
  str = convertToString(getIntensity());
  xml_doc->addElement(element, "intensity", str.c_str());
  parent->appendChild(element);
}

}
