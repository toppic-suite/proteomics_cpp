#include "base/logger.hpp"
#include "base/fasta_reader.hpp"

namespace prot {

FastaReader::FastaReader(const std::string &file_name) {
  input_.open(file_name.c_str(), std::ios::in);
  if (!input_.is_open()) {
    LOG_ERROR( "fasta file  " << file_name << " does not exist.");
    throw "fasta file does not exist.";
  }
  std::getline(input_, ori_name_);

  while(ori_name_.length() <= 1){
	  std::getline(input_, ori_name_);
  }  

}

FastaSeqPtr FastaReader::getNextSeq() {
  if (!input_.is_open()) {
    return FastaSeqPtr(nullptr);
  }

  /* get the letters of sequence */
  std::string ori_seq;
  ori_name_ = StringUtil::trim(ori_name_);
  std::string prot_name = ori_name_.substr(1, ori_name_.size() - 1);
  std::string line;
  while (std::getline(input_, line)) {
    if (line.length() >= 1 && line.substr(0, 1) == ">") {
      ori_name_ = StringUtil::trim(line);
      return FastaSeqPtr(new FastaSeq(prot_name, ori_seq));
    }
    line = StringUtil::trim(line);
    ori_seq = ori_seq + line;
    if (ori_seq.size() >= 1000000) {
      LOG_ERROR("Protein sequences are too long! Incorrect fasta file!");
      throw("fasta file error");
    }
  }
  input_.close();
  return FastaSeqPtr(new FastaSeq(prot_name, ori_seq));
}

}
