#include <limits>

#include "base/logger.hpp"
#include "prsm/simple_prsm_str.hpp"
#include "prsm/prsm_str.hpp"

namespace prot {

PrsmStr::PrsmStr(const std::vector<std::string> &str_vec) {
  str_vec_ = str_vec;
  std::string line = getXmlLine(str_vec_, "<spectrum_id>");
  spectrum_id_ = std::stoi(getValueStr(line));
  line = getXmlLine(str_vec_, "<db_seq_id>");
  db_seq_id_ = std::stoi(getValueStr(line));
  line = getXmlLine(str_vec_, "<db_seq_name>");
  db_seq_name_ = getValueStr(line);
  line = getXmlLine(str_vec_, "<match_fragment_num>");
  match_frag_num_ = std::stod(getValueStr(line));
  line = getXmlLine(str_vec_, "<e_value>"); 
  if (line == "") { 
    e_value_ = 0.0;
  }
  else {
    e_value_ = std::stod(getValueStr(line));
  }
  line = getXmlLine(str_vec_, "<fdr>"); 
  fdr_ = std::stod(getValueStr(line));
  //LOG_DEBUG("spectrum id " << spectrum_id_ << " match num " << match_frag_num_);
}

int getXmlLineIndex(const std::vector<std::string> &str_vec,
                    const std::string &property) {
  for (size_t i = 0; i < str_vec.size(); i++) {
    size_t found = str_vec[i].find(property);
    if (found != std::string::npos) {
      return i;
    }
  }
  return -1;
}

void PrsmStr::setFdr(double fdr) {
  int i = getXmlLineIndex(str_vec_, "fdr");
  str_vec_[i] = "<fdr>" + std::to_string(fdr) + "</fdr>";
}

void PrsmStr::setId(int id) {
  int i = getXmlLineIndex(str_vec_, "prsm_id");
  str_vec_[i] = "<prsm_id>" + std::to_string(id) + "</prsm_id>";
}

}

