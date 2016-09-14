// Copyright (c) 2014 - 2016, The Trustees of Indiana University.
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


#include <limits>
#include <cstddef>
#include <algorithm>

#include "base/logger.hpp"
#include "feature/vertex_base.hpp"
#include "feature/dp_b.hpp"

namespace prot {

DpB::DpB(DeconvDataPtr data_ptr, MatchEnvPtr2D &win_envs_, FeatureMngPtr mng_ptr):
    Dp(data_ptr, win_envs_, mng_ptr) {
      initGraph();
      dp();
      backtrace();
    }

void DpB::initGraph() {
  // use win_num__ + 2 columns 
  vertices_.resize(win_num_ + 2);
  MatchEnvPtrVec envs;
  vertices_[0] = VertexBase::getVertexBList(data_ptr_, -1, envs, envs, mng_ptr_);
  vertices_[1] = VertexBase::getVertexBList(data_ptr_, 0, envs, win_envs_[0], mng_ptr_);
  for (int i = 1; i < win_num_; i++) {
    vertices_[i + 1] = VertexBase::getVertexBList(data_ptr_, i, win_envs_[i - 1], 
                                                 win_envs_[i], mng_ptr_);
  }
  vertices_[win_num_ + 1] = VertexBase::getVertexBList(data_ptr_, win_num_, 
                                                     win_envs_[win_num_ - 1], envs, mng_ptr_);
}

void DpB::dp() {
  for (int i = 1; i < win_num_ + 2; i++) {
    for (size_t j = 0; j < vertices_[i].size(); j++) {
      VertexBPtr cur_ver = vertices_[i][j];
      for (size_t k = 0; k < vertices_[i - 1].size(); k++) {
        VertexBPtr prev_ver = vertices_[i - 1][k];
        if (Vertex::checkConsist(prev_ver, cur_ver, mng_ptr_->max_env_num_per_peak_)) {
          double new_score = Vertex::getShareScr(prev_ver, cur_ver, mng_ptr_->score_error_tolerance_);
          for (int cur_num = 0; cur_num <= mng_ptr_->dp_env_num_; cur_num++) {
            int prev_num = cur_num - cur_ver->getPreMatchEnvSize();
            if (prev_num >= 0) {
              double cur_score = prev_ver->getScoreB(prev_num) + new_score;
              if (cur_score > cur_ver->getScoreB(cur_num)) {
                cur_ver->setScoreB(cur_num, cur_score);
                cur_ver->setPrevB(cur_num, k);
              }
            }
          }
        }
      }
    }
  }
}

// backtracking 
void DpB::backtrace() {
  int best_ver = -1;
  double best_score = - std::numeric_limits<double>::max();
  int num = mng_ptr_->dp_env_num_;
  for (size_t i = 0; i < vertices_[win_num_ + 1].size(); i++) {
    double cur_score = vertices_[win_num_ + 1][i]->getScoreB(num);
    if (cur_score > best_score) {
      best_ver = i;
      best_score = cur_score;
    }
  }
  for (int i = win_num_ + 1; i >= 1; i--) {
    if (vertices_[i][best_ver]->getPrevB(num) >= 0) {
      MatchEnvPtrVec prev_envs = vertices_[i][best_ver]->getPreMatchEnvs();
      addEnv(results_, prev_envs);
    }
    best_ver = vertices_[i][best_ver]->getPrevB(num);
    if (best_ver < 0) {
      break;
    }
  }
  std::sort(results_.begin(), results_.end(), MatchEnv::cmpScoreDec);
}

}
