//Copyright (c) 2014 - 2018, The Trustees of Indiana University.
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


#ifndef TOPPIC_DECONV_FEATURE_FRAC_MS2_FEATURE_HPP_
#define TOPPIC_DECONV_FEATURE_FRAC_MS2_FEATURE_HPP_

#include <memory>
#include <vector>

#include "spec/ms_header.hpp"
#include "deconv/feature/frac_feature.hpp"

namespace toppic {

class FracMs2Feature;
typedef std::shared_ptr<FracMs2Feature> FracMs2FeaturePtr;
typedef std::vector<FracMs2FeaturePtr> FracMs2FeaturePtrVec;

class FracMs2Feature {
 public:
  FracMs2Feature(int id, int frac_id, 
                 const std::string &file_name,
                 std::string &scans,
                 int ms_one_id, int ms_one_scan, 
                 double prec_mass, double prec_inte,
                 int frac_feature_id, double frac_feature_inte,
                 int sample_feature_id, double sample_feature_inte);

  FracMs2Feature(std::string line);

  FracMs2Feature(MsHeaderPtr header, FracFeaturePtr feature);

  int getId() {return id_;}

  int getFracId() {return frac_id_;}

  std::string getFileName() {return file_name_;}
  
  std::string getScans() {return scans_;}

  int getMsOneId() {return ms_one_id_;}

  int getMsOneScan() {return ms_one_scan_;}

  double getPrecMass() {return prec_mass_;}

  double getPrecInte() {return prec_inte_;}

  int getFracFeatureId() {return frac_feature_id_;}

  double getFracFeatureInte() {return frac_feature_inte_;}

  int getSampleFeatureId() {return sample_feature_id_;}

  double getSampleFeatureInte() {return sample_feature_inte_;}

  void setSampleFeatureId(int id) {sample_feature_id_ = id;}

  void setSampleFeatureInte(double inte) {sample_feature_inte_ = inte;}

 protected:
  int id_;
  int frac_id_;
  std::string file_name_;
  std::string scans_;
  int ms_one_id_;
  int ms_one_scan_;
  double prec_mass_;
  double prec_inte_;
  int frac_feature_id_;
  double frac_feature_inte_;
  int sample_feature_id_;
  double sample_feature_inte_;
};

}
#endif
