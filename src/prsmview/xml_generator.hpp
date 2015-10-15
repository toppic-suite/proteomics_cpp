#ifndef PROT_XML_GENERATOR_HPP_
#define PROT_XML_GENERATOR_HPP_

#include <map>
#include <xercesc/util/PlatformUtils.hpp>

#include "base/proteoform.hpp"
#include "base/proteoform_reader.hpp"
#include "base/xml_writer.hpp"
#include "spec/msalign_reader.hpp"
#include "prsm/prsm.hpp"
#include "ptmsearch/ptm_mng.hpp"
#include "prsmview/prsm_view_mng.hpp"
#include "prsmview/anno_view.hpp"

namespace prot {

class XmlGenerator {
 public:
  XmlGenerator(PrsmParaPtr prsm_para_ptr, const std::string &exec_dir, 
               const std::string &input_file_name);
  void process();
  void outputPrsms(const PrsmPtrVec &prsm_ptrs);
  void outputAllPrsms(const PrsmPtrVec &prsm_ptrs);
  void outputProteoforms(const PrsmPtrVec &prsm_ptrs);
  void outputProteins(const PrsmPtrVec &prsm_ptrs);
  void outputAllProteins(const PrsmPtrVec &prsm_ptrs);
  void outputFileList();

 private:
  std::string input_file_ext_;
  PrsmViewMngPtr mng_ptr_;
  AnnoViewPtr anno_view_ptr_;
};

typedef std::shared_ptr<XmlGenerator> XmlGeneratorPtr;

} /* namespace prot */

#endif /* PROT_XML_GENERATOR_HPP_ */
