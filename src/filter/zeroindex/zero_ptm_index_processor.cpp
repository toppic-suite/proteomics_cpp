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

#include "common/util/file_util.hpp"
#include "common/thread/simple_thread_pool.hpp"

#include "seq/proteoform.hpp"
#include "seq/proteoform_factory.hpp"

#include "ms/spec/msalign_reader.hpp"
#include "ms/spec/msalign_util.hpp"
#include "ms/spec/spectrum_set.hpp"

#include "prsm/simple_prsm_xml_writer_set.hpp"
#include "prsm/simple_prsm_str_merge.hpp"

#include "filter/zeroindex/zero_ptm_index_processor.hpp"

#include "filter/zeroindex/zero_ptm_index_file.hpp"


#include "console/topindex_file_name.hpp"

namespace toppic{

inline void createIndexFiles(const ProteoformPtrVec & raw_forms,
                          int block_idx, ZeroPtmFilterMngPtr mng_ptr, int block_num, int *current_num) {  
                        
    std::string block_str = str_util::toString(block_idx);
    PrsmParaPtr prsm_para_ptr = mng_ptr->prsm_para_ptr_;
    
    //index file names = fixed mod + n terminal + activation + error tolerance + decoy + block number
    TopIndexFileNamePtr file_name_ptr = std::make_shared<TopIndexFileName>();
    std::string parameters = file_name_ptr->geneFileName(prsm_para_ptr);

    std::vector<std::string> file_vec;

    for (int i = 0; i < file_name_ptr->zero_ptm_file_vec_.size(); i++){
      file_vec.push_back(file_name_ptr->zero_ptm_file_vec_[i] + parameters + block_str);
    }

    ZeroPtmIndexFilePtr index_file_ptr = std::make_shared<ZeroPtmIndexFile>(raw_forms, mng_ptr, file_vec);

    mng_ptr->mutex_.lock();

    std::cout << "Non PTM index files - processing " << *current_num << " of " << block_num << " files." << std::endl;
    *current_num = *current_num + 1;

    mng_ptr->mutex_.unlock();
  
}

std::function<void()> geneIndexTask(int block_idx, 
                               ZeroPtmFilterMngPtr mng_ptr, int block_num, int *current_num) {
  return[block_idx, mng_ptr, block_num, current_num] () {
    PrsmParaPtr prsm_para_ptr = mng_ptr->prsm_para_ptr_;
    std::string db_block_file_name = prsm_para_ptr->getSearchDbFileName()
        + "_" + str_util::toString(block_idx);
    ProteoformPtrVec raw_forms
        = proteoform_factory::readFastaToProteoformPtrVec(db_block_file_name,
                                                          prsm_para_ptr->getFixModPtrVec());

    createIndexFiles(raw_forms, block_idx, mng_ptr, block_num, current_num);
  };
}
void ZeroPtmIndexProcessor::process(){
  //for generating index files
  PrsmParaPtr prsm_para_ptr = mng_ptr_->prsm_para_ptr_;
  std::string db_file_name = prsm_para_ptr->getSearchDbFileName();
  DbBlockPtrVec db_block_ptr_vec = DbBlock::readDbBlockIndex(db_file_name);

  // n_spec_block = spec_num * block_num

  SimpleThreadPoolPtr pool_ptr = std::make_shared<SimpleThreadPool>(mng_ptr_->thread_num_);
  
  int block_num = db_block_ptr_vec.size();
  int current_num = 1; //show how many files have been processed. n in the message "n of 5 files processed"..

  //logger::setLogLevel(2);
  std::cout << "Generating Non PTM index files --- started" << std::endl;
  for (int i = 0; i < block_num; i++) {
    while (pool_ptr->getQueueSize() >= mng_ptr_->thread_num_ * 2) {
      boost::this_thread::sleep(boost::posix_time::milliseconds(100));
    }
    pool_ptr->Enqueue(geneIndexTask(db_block_ptr_vec[i]->getBlockIdx(), mng_ptr_, block_num, &current_num));
  }
  pool_ptr->ShutDown();
  std::cout << "Generating Non PTM index files --- finished" << std::endl;
  //std::cout << std::endl;
}

}