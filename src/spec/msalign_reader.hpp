#ifndef PROT_MSALIGN_READER_HPP_
#define PROT_MSALIGN_READER_HPP_

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <memory>

#include "spec/deconv_ms.hpp"

namespace prot {

class MsAlignReader {
 public:
  MsAlignReader(std::string file_name);

  std::vector<std::string> readOneSpectrum();

  void readNext();

  DeconvMsPtr getNextMs();

  std::vector<std::string> getSpectrumStr() {
    return spectrum_str_;
  }

  void close();

 private:
  std::string file_name_;
  std::ifstream input_;
  std::vector<std::string> spectrum_str_;
  int current_ = 0;
  DeconvMsPtr deconv_ms_ptr_ = DeconvMsPtr(nullptr);
};

int countSpNum(const char *spectrum_file);

}
#endif
