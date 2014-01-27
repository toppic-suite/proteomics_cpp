#include <iostream>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/framework/XMLFormatter.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationRegistry.hpp>
#include <xercesc/dom/DOMLSSerializer.hpp>
#include <xercesc/dom/DOMLSOutput.hpp>

#include "base/xml_dom.hpp"
#include "base/xml_dom_err_handler.hpp"
 
namespace prot {

XmlDOMParser* XmlDOMParserFactory::dom_parser_ = nullptr;

/* XmlDOMParser */
XmlDOMParser::XmlDOMParser() : parser_(nullptr), err_handler_(nullptr) {
  xercesc::XMLPlatformUtils::Initialize();
  parser_ = new xercesc::XercesDOMParser();
  err_handler_ = (xercesc::ErrorHandler*) new XmlDOMErrorHandler();
  parser_->setErrorHandler(err_handler_);
}

XmlDOMParser::~XmlDOMParser() {
  if (parser_ != nullptr) {
    delete parser_;
    xercesc::XMLPlatformUtils::Terminate();
  }
}

xercesc::DOMDocument* XmlDOMParser::parse(std::string xml_file) {
  parser_->parse(xml_file.c_str());
  return parser_->adoptDocument();
}

/* XmlDOMImplenmation */
XmlDOMImpl* XmlDOMImplFactory::dom_impl_ = nullptr;

XmlDOMImpl::XmlDOMImpl() {
  impl_ = xercesc::DOMImplementationRegistry::getDOMImplementation(X("Core"));
}

XmlDOMImpl::~XmlDOMImpl() {
  if (impl_ != nullptr) {
    delete impl_;
  }
}

xercesc::DOMDocument* XmlDOMImpl::createDoc(std::string root) {
  xercesc::DOMDocument* doc = impl_->createDocument(0, X(root.c_str()), 0);
  return doc;
}

xercesc::DOMLSSerializer* XmlDOMImpl::createSerializer() {
  xercesc::DOMLSSerializer* writer = impl_->createLSSerializer();
  writer->getDomConfig()->setParameter(
      xercesc::XMLUni::fgDOMWRTFormatPrettyPrint, true);
  writer->getDomConfig()->setParameter(
      xercesc::XMLUni::fgDOMWRTDiscardDefaultContent, true);
  writer->setNewLine(X("\n"));
  return writer;
}

}
