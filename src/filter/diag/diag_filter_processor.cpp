//Copyright (c) 2014 - 2020, The Trustees of Indiana University.
//
//Licensed under the Apache License, Version 2.0 (the "License");
//you may not use this file except in compliance with the License.
//You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
//Unless required by applicable law or agreed to in writing, software
//distributed under the License is distributed on an "AS IS" BASIS,
//WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//See the License for the specific language governing permissions and
//limitations under the License.

#include <iostream>
#include <chrono>

#include "common/util/file_util.hpp"
#include "common/base/mod_util.hpp"
#include "common/base/base_data.hpp"
#include "common/thread/simple_thread_pool.hpp"
#include "seq/proteoform.hpp"
#include "seq/db_block.hpp"
#include "seq/proteoform_factory.hpp"
#include "ms/spec/msalign_util.hpp"
#include "prsm/simple_prsm_xml_writer.hpp"
#include "prsm/simple_prsm_xml_writer_util.hpp"
#include "prsm/simple_prsm_str_merge.hpp"
#include "filter/diag/mass_diag_filter.hpp"
#include "filter/diag/diag_filter_processor.hpp"
#include "filter/diag/mass_diag_index_file.hpp"

namespace toppic {

//typedef std::shared_ptr<ThreadPool<SimplePrsmXmlWriter>> SimplePrsmThreadPoolPtr;

std::function<void()> geneTask(MassDiagFilterPtr filter_ptr,
                               const PrmMsPtrVec & ms_ptr_vec,
                               SimpleThreadPoolPtr  pool_ptr, 
                               SimplePrsmXmlWriterPtrVec &writer_ptr_vec) {
  return [filter_ptr, ms_ptr_vec, pool_ptr, writer_ptr_vec]() {
    SimplePrsmPtrVec match_ptrs = filter_ptr->getBestMatch(ms_ptr_vec);
    boost::thread::id thread_id = boost::this_thread::get_id();
    int writer_id = pool_ptr->getId(thread_id);
    writer_ptr_vec[writer_id]->write(match_ptrs);
  };
}

void DiagFilterProcessor::process() {
  std::string db_file_name = mng_ptr_->prsm_para_ptr_->getSearchDbFileName();
  DbBlockPtrVec db_block_ptr_vec = DbBlock::readDbBlockIndex(db_file_name);

  std::vector<double> mod_mass_list;
  if (mng_ptr_->residueModFileName_ != "") {
    mod_mass_list = mod_util::getModMassVec(mod_util::readModTxt(mng_ptr_->residueModFileName_)[2]);
  }

  for (size_t i = 0; i < db_block_ptr_vec.size(); i++) {
    std::cout << "Multiple PTM filtering - block " << (i + 1) << " out of "
        << db_block_ptr_vec.size() << " started." << std::endl;
    processBlock(db_block_ptr_vec[i], db_block_ptr_vec.size(), mod_mass_list, i);
    std::cout << "Multiple PTM filtering - block " << (i + 1) << " finished. " << std::endl;
  }

  std::cout << "Multiple PTM filtering - combining blocks started." << std::endl;

  std::string sp_file_name = mng_ptr_->prsm_para_ptr_->getSpectrumFileName();
  int block_num = db_block_ptr_vec.size();
  SimplePrsmStrMergePtr merge_ptr
      = std::make_shared<SimplePrsmStrMerge>(sp_file_name, mng_ptr_->output_file_ext_,
                                             block_num, mng_ptr_->output_file_ext_,
                                             mng_ptr_->filter_result_num_);
  merge_ptr->process();
  merge_ptr = nullptr;
  //Remove temporary files
  file_util::cleanTempFiles(sp_file_name, mng_ptr_->output_file_ext_ + "_");

  std::cout << "Multiple PTM filtering - combining blocks finished." << std::endl;
}

void DiagFilterProcessor::processBlock(DbBlockPtr block_ptr, int total_block_num,
                                       const std::vector<double> & mod_mass_list, int block_idx) {
  std::string block_number = str_util::toString(block_idx);
  PrsmParaPtr prsm_para_ptr = mng_ptr_->prsm_para_ptr_;
  std::string db_block_file_name = prsm_para_ptr->getSearchDbFileName()
      + "_" + str_util::toString(block_ptr->getBlockIdx());
  ProteoformPtrVec raw_forms
      = proteoform_factory::readFastaToProteoformPtrVec(db_block_file_name,
                                                        prsm_para_ptr->getFixModPtrVec());
  //timer
  auto start = std::chrono::steady_clock::now();

  MassDiagFilterPtr filter_ptr = std::make_shared<MassDiagFilter>(raw_forms, mng_ptr_, block_number);
  auto end = std::chrono::steady_clock::now();
 // std::cout << "multi_ptm process time: " << std::chrono::duration_cast<std::chrono::seconds>(end-start).count() << " sec" << std::endl;

  int group_spec_num = mng_ptr_->prsm_para_ptr_->getGroupSpecNum();
  SpParaPtr sp_para_ptr =  mng_ptr_->prsm_para_ptr_->getSpParaPtr();
  sp_para_ptr->prec_error_ = 0;
  std::string sp_file_name = prsm_para_ptr->getSpectrumFileName();
  MsAlignReader reader(sp_file_name,
                       group_spec_num,
                       sp_para_ptr->getActivationPtr(),
                       sp_para_ptr->getSkipList());

  // init writer 
  std::string output_file_name = file_util::basename(prsm_para_ptr->getSpectrumFileName())
      + "." + mng_ptr_->output_file_ext_+"_"+ str_util::toString(block_ptr->getBlockIdx());
  SimplePrsmXmlWriterPtrVec writer_ptr_vec 
      = simple_prsm_xml_writer_util::geneWriterPtrVec(output_file_name, mng_ptr_->thread_num_);

  SimpleThreadPoolPtr pool_ptr = std::make_shared<SimpleThreadPool>(mng_ptr_->thread_num_);

  SpectrumSetPtr spec_set_ptr = reader.getNextSpectrumSet(sp_para_ptr)[0];

  int spectrum_num = msalign_util::getSpNum(prsm_para_ptr->getSpectrumFileName());

  int cnt = 0;

  while (spec_set_ptr != nullptr) {
    cnt += group_spec_num;
    if (spec_set_ptr->isValid()) {
      if (mng_ptr_->var_num_ == 0) {
        PrmMsPtrVec ms_ptr_vec = spec_set_ptr->getMsTwoPtrVec();
        while (pool_ptr->getQueueSize() >= mng_ptr_->thread_num_ * 2) {
          boost::this_thread::sleep(boost::posix_time::milliseconds(100));
        }
        pool_ptr->Enqueue(geneTask(filter_ptr, ms_ptr_vec, pool_ptr, writer_ptr_vec));
      } else {
        std::vector<double> mod_mass(3);
        for (size_t i = 0; i < mod_mass_list.size(); i++) {
          for (size_t k1 = 0; k1 < mod_mass.size(); k1++) {
            std::fill(mod_mass.begin(), mod_mass.end(), 0.0);
            mod_mass[k1] += mod_mass_list[i];
            PrmMsPtrVec ms_ptr_vec = spec_set_ptr->getMsTwoPtrVec(sp_para_ptr, mod_mass);
            while (pool_ptr->getQueueSize() >= mng_ptr_->thread_num_ * 2) {
              boost::this_thread::sleep(boost::posix_time::milliseconds(100));
            }
            pool_ptr->Enqueue(geneTask(filter_ptr, ms_ptr_vec, pool_ptr, writer_ptr_vec));
          }
        }
      }
    }
    std::stringstream msg;
    msg << std::flush << "Multiple PTM filtering - processing " << cnt
        << " of " << spectrum_num << " spectra.\r";
    std::cout << msg.str();
    spec_set_ptr = reader.getNextSpectrumSet(sp_para_ptr)[0];
  }
  pool_ptr->ShutDown();
  std::cout << std::endl;
  reader.close();
  simple_prsm_xml_writer_util::closeWriterPtrVec(writer_ptr_vec);

  //combine files generated by multiple threads
  std::string block_str = str_util::toString(block_ptr->getBlockIdx());
  std::vector<std::string> input_exts;
  std::string cur_output_ext = mng_ptr_->output_file_ext_ + "_" + block_str;
  for (int i = 0; i < mng_ptr_->thread_num_; i++) {
    std::string fname = cur_output_ext + "_" + str_util::toString(i);
    input_exts.push_back(fname);
  }

  SimplePrsmStrMergePtr merge_ptr
      = std::make_shared<SimplePrsmStrMerge>(mng_ptr_->prsm_para_ptr_->getSpectrumFileName(),
                                             input_exts, cur_output_ext, INT_MAX);
  merge_ptr->process();
  merge_ptr = nullptr;
  
  //Remove temporary files
  file_util::cleanTempFiles(sp_file_name, cur_output_ext + "_");
}
//below are used for index file generation
/*
std::function<void()> geneIndexTask(MassDiagFilterPtr filter_ptr,
                               const PrmMsPtrVec & ms_ptr_vec,
                               SimpleThreadPoolPtr  pool_ptr, 
                               SimplePrsmXmlWriterPtrVec &writer_ptr_vec) {
  return [filter_ptr, ms_ptr_vec, pool_ptr, writer_ptr_vec]() {
    SimplePrsmPtrVec match_ptrs = filter_ptr->getBestMatch(ms_ptr_vec);
    boost::thread::id thread_id = boost::this_thread::get_id();
    int writer_id = pool_ptr->getId(thread_id);
    writer_ptr_vec[writer_id]->write(match_ptrs);
  };
}*/

void DiagFilterProcessor::indexProcess() {
  std::string db_file_name = mng_ptr_->prsm_para_ptr_->getSearchDbFileName();
  DbBlockPtrVec db_block_ptr_vec = DbBlock::readDbBlockIndex(db_file_name);

  std::cout << "Generating Multi PTM index files --- started" << std::endl;

  std::vector<double> mod_mass_list;
  if (mng_ptr_->residueModFileName_ != "") {
    mod_mass_list = mod_util::getModMassVec(mod_util::readModTxt(mng_ptr_->residueModFileName_)[2]);
  }

  SimpleThreadPoolPtr pool_ptr = std::make_shared<SimpleThreadPool>(mng_ptr_->thread_num_);
  int block_num = db_block_ptr_vec.size();
  int current_num = 1; //show how many files have been processed. n in the message "n of 5 files processed"..

  for (size_t i = 0; i < db_block_ptr_vec.size(); i++) {
    
    createIndexFiles(db_block_ptr_vec[i], db_block_ptr_vec.size(), mod_mass_list, block_num, &current_num);
  }
  pool_ptr->ShutDown();
  std::cout << "Generating Multi PTM index files --- finished" << std::endl;
}

void DiagFilterProcessor::createIndexFiles(DbBlockPtr block_ptr, int total_block_num,
                                       const std::vector<double> & mod_mass_list, int block_num, int *current_num) {
  PrsmParaPtr prsm_para_ptr = mng_ptr_->prsm_para_ptr_;

  std::string db_block_file_name = prsm_para_ptr->getSearchDbFileName()
      + "_" + str_util::toString(block_ptr->getBlockIdx());

  ProteoformPtrVec raw_forms
      = proteoform_factory::readFastaToProteoformPtrVec(db_block_file_name,
                                                        prsm_para_ptr->getFixModPtrVec());

  std::string block_str = str_util::toString(block_ptr->getBlockIdx());

  MassDiagIndexPtr filter_ptr = std::make_shared<MassDiagIndex>(raw_forms, mng_ptr_, block_str);
  
  mng_ptr_->mutex_.lock();

    std::cout << "Multi PTM index files - processing " << *current_num << " of " << block_num << " files." << std::endl;
    *current_num = *current_num + 1;

    mng_ptr_->mutex_.unlock();

}

}  // namespace toppic
