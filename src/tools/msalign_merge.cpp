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

#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <iostream>

#include <boost/program_options.hpp>
#include <boost/filesystem/operations.hpp>

#include "spec/ms_header.hpp"
#include "spec/deconv_ms.hpp"
#include "spec/msalign_reader.hpp"
#include "feature/msalign_writer.hpp"

int main(int argc, const char *argv[]) {
  try {
    namespace po = boost::program_options;
    po::options_description desc("Options");
    int N = 100000;
    std::string output_filename = "";
    std::vector<std::string> msalign_file_list;
    desc.add_options()
        ("help,h", "Help message")
        ("N", po::value<int>(&N), "N")
        ("msalign,m", po::value<std::vector<std::string> >()->multitoken()->required(), "msalign file list")
        ("output,o", po::value<std::string>(&output_filename)->required(), "output msalign file name");
    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(desc).run(), vm); 
    if (vm.count("help")) {
      std::cout << desc << "\n";
      return EXIT_SUCCESS;
    }

    po::notify(vm);

    if (vm.count("msalign")) {
      msalign_file_list = vm["msalign"].as<std::vector<std::string> >();
      if (msalign_file_list.size() == 1) {
        return EXIT_SUCCESS;
      }
    }

    std::cout << "msalign files to merge:" << std::endl;
    for (size_t i = 0; i < msalign_file_list.size(); i++) {
      if (!boost::filesystem::exists(msalign_file_list[i])) {
        std::cerr << "msalign file " << msalign_file_list[i] << " does not exist!" << std::endl;
        return EXIT_FAILURE;
      }
      std::cout << msalign_file_list[i] << std::endl;
    }

    std::ofstream outfile(output_filename); 
    outfile.precision(16);

    for (int i = 0; i < static_cast<int>(msalign_file_list.size()); i++) {
      prot::MsAlignReader sp_reader(msalign_file_list[i], 1,
                                    nullptr, std::set<std::string>());
      prot::DeconvMsPtr ms_ptr = sp_reader.getNextMs();
      while (ms_ptr != nullptr) {
        prot::MsHeaderPtr header = ms_ptr->getMsHeaderPtr();
        header->setId(header->getId() + i * N);
        if (header->getMsOneId() > 0) {
          header->setMsOneId(header->getMsOneId() + i * N);
        }
        if (header->getFeatureId() > 0) {
          header->setFeatureId(header->getFeatureId() + i * N); 
        }
        ms_ptr->setHeaderPtr(header);
        prot::MsalignWriter::write(outfile, ms_ptr);
        ms_ptr = sp_reader.getNextMs();
      }
      sp_reader.close();
    }

    outfile.close();

  } catch (const std::exception &ex) {
    std::cerr << ex.what() << std::endl;
  }
  return EXIT_SUCCESS;
}
