
#include <algorithm>
#include "prsm/peak_ion_pair_factory.hpp"
#include "base/acid_base.hpp"
#include "base/mod_util.hpp"
#include "base/residue_util.hpp"
#include "spec/theo_peak_factory.hpp"
#include "local_util.hpp"

namespace prot {

LocalMngPtr LocalUtil::mng_ptr_;
PtmPtrVec LocalUtil::var_ptm_list_;
PtmPairVec LocalUtil::ptm_pair_vec_;
ModPtrVec LocalUtil::mod_list_N_;
ModPtrVec LocalUtil::mod_list_C_;
ModPtrVec LocalUtil::mod_list_any_;
double LocalUtil::ppm_;
double LocalUtil::p1_, LocalUtil::p2_;

void LocalUtil::init(LocalMngPtr mng_ptr) {
  std::vector<ModPtrVec> mod_ptr_vec2d = ModUtil::readModTxt(mng_ptr->residueModFileName_);
  mod_list_N_ = mod_ptr_vec2d[0];
  mod_list_C_ = mod_ptr_vec2d[1];
  mod_list_any_ = mod_ptr_vec2d[2];
  readPtmTxt(mng_ptr->residueModFileName_);
  mng_ptr_ = mng_ptr;
  p1_ = mng_ptr->p1_;
  p2_ = mng_ptr->p2_;
  ppm_ = mng_ptr->prsm_para_ptr_->getSpParaPtr()->getPeakTolerancePtr()->getPpo();
}

std::vector<double> LocalUtil::normalize(const std::vector<double> & scr) {
  std::vector<double> res(scr.size());
  // to avoid overflow if the scores are too large
  double max = *std::max_element(scr.begin(), scr.end());
  for (size_t i = 0; i < scr.size(); i++) {
    res[i] = scr[i] / max;
  }
  double sum = std::accumulate(res.begin(), res.end(), 0.0);
  for (size_t i = 0; i < res.size(); i++) {
    res[i] /= sum;
  }
  return res;
}

void LocalUtil::scr_filter(std::vector<double> & scr, int & bgn, int & end,
                           double & conf, double thread) {
  bgn = std::distance(scr.begin(), std::max_element(scr.begin(), scr.end()));
  end = scr.size() - 1;
  for (; end >= 0; end--) {
    if (scr[bgn] == scr[end]) {
      break;
    }
  }
  conf = 0;
  for (int i = bgn; i <= end; i++) { conf += scr[i];}
  while (conf <= thread) {
    if (bgn == 0) {
      end++; conf += scr[end];
    } else if (end == (int) scr.size() - 1) {
      bgn--; conf += scr[bgn];
    } else {
      if (scr[bgn] == scr[end]) {
        end++; conf += scr[end];
        bgn--; conf += scr[bgn];
      } else if (scr[bgn] > scr[end]) {
        bgn--; conf += scr[bgn];
      } else {
        end++; conf += scr[end];
      }
    }
  }
  std::vector<double> scr2;
  scr2.insert(scr2.end(), scr.begin() + bgn, scr.begin() + end + 1);
  scr = scr2;
}

void LocalUtil::compSupPeakNum(ProteoformPtr proteoform, const ExtendMsPtrVec & extend_ms_ptr_vec, 
                               ChangePtr change, double min_mass, int & left, int & right) {
  left = right = 0;
  PeakIonPairPtrVec pair_ptrs = 
      PeakIonPairFactory::genePeakIonPairs(proteoform, extend_ms_ptr_vec, mng_ptr_->min_mass_);

  for (size_t i = 0; i < pair_ptrs.size(); i++) {
    std::string ion_name = 
        pair_ptrs[i]->getTheoPeakPtr()->getIonPtr()->getIonTypePtr()->getName();
    // A, B, C are for N-terminal ions
    if (ion_name == "A" || ion_name == "B" || ion_name == "C") {
      if (pair_ptrs[i]->getTheoPeakPtr()->getIonPtr()->getDisplayPos()
          <= change->getLeftBpPos()) {
        left++;
      } else if (pair_ptrs[i]->getTheoPeakPtr()->getIonPtr()->getDisplayPos()
                 >= change->getRightBpPos()) {
        right++;
      }
    } else {
      if (pair_ptrs[i]->getTheoPeakPtr()->getIonPtr()->getDisplayPos()
          >= proteoform->getLen() - change->getLeftBpPos()) {
        left++;
      } else if (pair_ptrs[i]->getTheoPeakPtr()->getIonPtr()->getDisplayPos()
                 <= proteoform->getLen() - change->getRightBpPos()) {
        right++;
      }
    }
  }
}

PtmPtrVec LocalUtil::getPtmPtrVecByMass(double mass, double err) {
  PtmPtrVec res;
  for (size_t i = 0; i < var_ptm_list_.size(); i++) {
    if (std::abs(var_ptm_list_[i]->getMonoMass() - mass) < err
        || std::abs(std::abs(var_ptm_list_[i]->getMonoMass() - mass) - 1) < err) {
      res.push_back(var_ptm_list_[i]);
    }
  }
  return res;
}

PtmPairVec LocalUtil::getPtmPairVecByMass(double mass1, double mass2, double err) {
  PtmPairVec res;
  double mass = mass1 + mass2;
  for (size_t i = 0; i < ptm_pair_vec_.size(); i++) {
    double pair_mass = ptm_pair_vec_[i].first->getMonoMass() 
        + ptm_pair_vec_[i].second->getMonoMass();

    if (std::abs(pair_mass - mass) < err
        || std::abs(std::abs(pair_mass - mass) - MassConstant::getIsotopeMass() ) < err)
      res.push_back(ptm_pair_vec_[i]);
  }

  return res;
}

bool LocalUtil::modifiable(ProteoformPtr proteoform_ptr, int i, PtmPtr ptm_ptr) {
  ChangePtrVec fixed_change = proteoform_ptr->getChangePtrVec(ChangeType::FIXED);

  for (size_t j = 0; j < fixed_change.size(); j++) {
    if (i < fixed_change[j]->getRightBpPos() && i >= fixed_change[j]->getLeftBpPos()) {
      return false;
    }
  }

  if (ptm_ptr == nullptr) return true;

  int start = proteoform_ptr->getStartPos();
  int end = proteoform_ptr->getEndPos();

  ResiduePtr residue_ptr = proteoform_ptr->getResSeqPtr()->getResiduePtr(i);

  ModPtrVec mod_list;

  if (i == 0) {
    mod_list = mod_list_N_;
  } else if (i + start == end) {
    mod_list = mod_list_C_;
  } else {
    mod_list = mod_list_any_;
  }

  for (size_t j = 0; j < mod_list.size(); j++) {
    if (mod_list[j]->getOriResiduePtr()->isSame(residue_ptr) &&
        mod_list[j]->getModResiduePtr()->getPtmPtr()->isSame(ptm_ptr))
      return true; 
  }

  return false;
}

void LocalUtil::getNtermTruncRange(ProteoformPtr proteoform, const ExtendMsPtrVec & extend_ms_ptr_vec,
                                   int & min, int & max) {
  int left_sup, tmp;
  ChangePtrVec change_vec = proteoform->getChangePtrVec(ChangeType::UNEXPECTED);
  ChangePtr change_ptr = change_vec[0];
  compSupPeakNum(proteoform, extend_ms_ptr_vec, change_ptr, mng_ptr_->min_mass_, left_sup, tmp);

  if (left_sup > LEFT_SUP_LIMIT) {
    min = max = 0;
    return;
  }

  double ori_mass = proteoform->getChangePtrVec(ChangeType::UNEXPECTED)[0]->getMassShift();
  double mass = ori_mass;
  int ori_start = proteoform->getStartPos();
  max = min = 0;
  while (ori_start + min > 0) {
    min--;
    std::string t_seq = proteoform->getFastaSeqPtr()->getSeq().substr(ori_start + min, -min);
    mass = ori_mass - getPeptideMass(t_seq);
    if (std::abs(mass) >= mng_ptr_->max_ptm_mass_ || ori_start + min <= 0) {
      min++;
      break;
    }
  }
  mass = ori_mass;
  while (std::abs(ori_mass) < mng_ptr_->max_ptm_mass_) {
    max++;
    std::string t_seq = proteoform->getFastaSeqPtr()->getSeq().substr(ori_start, max);
    mass = ori_mass + getPeptideMass(t_seq);
    if (std::abs(mass) >= mng_ptr_->max_ptm_mass_) {
      max--;
      break;
    }
  }

  ChangePtrVec fix_change_vec = proteoform->getChangePtrVec(ChangeType::FIXED);

  for (size_t i = 0; i < fix_change_vec.size(); i++) {
    if (fix_change_vec[i]->getModPtr()->getModResiduePtr()->getPtmPtr()->getAbbrName() == "Acetyl") {
      change_vec[0]->setMassShift(ori_mass + fix_change_vec[i]->getModPtr()->getShift());
      fix_change_vec.erase(fix_change_vec.begin() + i);
      change_vec.insert(change_vec.end(), fix_change_vec.begin(), fix_change_vec.end());
      std::sort(change_vec.begin(), change_vec.end(), Change::cmpPosInc);
      break;
    }
  }

  proteoform = std::make_shared<Proteoform>(proteoform->getFastaSeqPtr(), proteoform->getProtModPtr(),
                                            proteoform->getStartPos(), proteoform->getEndPos(), 
                                            proteoform->getResSeqPtr(), change_vec);
}

void LocalUtil::getCtermTruncRange(ProteoformPtr proteoform, const ExtendMsPtrVec & extend_ms_ptr_vec,          
                                   int & min, int & max) {
  ChangePtrVec change_vec = proteoform->getChangePtrVec(ChangeType::UNEXPECTED);
  ChangePtr change_ptr = change_vec[change_vec.size() - 1];
  int right_sup, tmp;
  compSupPeakNum(proteoform, extend_ms_ptr_vec, change_ptr, mng_ptr_->min_mass_, tmp, right_sup);

  if (right_sup > RIGHT_SUP_LIMIT) {
    min = max = 0;
    return;
  }

  double ori_mass = change_vec[change_vec.size() - 1]->getMassShift();
  double mass = ori_mass;
  int ori_end = proteoform->getEndPos();
  max = min = 0;
  while (ori_end + std::abs(max) < proteoform->getFastaSeqPtr()->getLen() - 1) {
    max++;
    std::string t_seq = proteoform->getFastaSeqPtr()->getSeq().substr(ori_end + 1, max);
    mass = ori_mass - getPeptideMass(t_seq);
    if (std::abs(mass) >= mng_ptr_->max_ptm_mass_ || ori_end + std::abs(max) >= proteoform->getFastaSeqPtr()->getLen() - 1) {
      max--;
      break;
    }
  }

  mass = ori_mass;
  while (std::abs(ori_mass) < mng_ptr_->max_ptm_mass_) {
    min--;
    std::string t_seq = proteoform->getFastaSeqPtr()->getSeq().substr(ori_end + 1 + min, -min);
    mass = ori_mass + getPeptideMass(t_seq);
    if (std::abs(mass) >= mng_ptr_->max_ptm_mass_) {
      min++;
      break;
    }
  }
}

ProteoformPtr geneProteoform(FastaSeqPtr fasta_seq_ptr, ProtModPtr prot_mod_ptr, 
                             int start_pos, int end_pos,
                             const ChangePtrVec &change_ptr_vec, const ModPtrVec & mod_ptr_vec) {
  ResSeqPtr new_res_seq = std::make_shared<ResidueSeq>(
      ResidueUtil::convertStrToResiduePtrVec(
          fasta_seq_ptr->getSeq().substr(start_pos, end_pos - start_pos + 1), 
          mod_ptr_vec));
  return std::make_shared<Proteoform>(fasta_seq_ptr,
                                      prot_mod_ptr, start_pos, end_pos, 
                                      new_res_seq, change_ptr_vec);
}

void LocalUtil::onePtmTermAdjust(ProteoformPtr & proteoform, const ExtendMsPtrVec & extend_ms_ptr_vec,
                                 double & mass, double err) {

  int n_trunc_min, n_trunc_max, c_trunc_min, c_trunc_max;
  getNtermTruncRange(proteoform, extend_ms_ptr_vec, n_trunc_min, n_trunc_max);
  getCtermTruncRange(proteoform, extend_ms_ptr_vec, c_trunc_min, c_trunc_max);

  ChangePtr change_ptr = proteoform->getChangePtrVec(ChangeType::UNEXPECTED)[0];
  ChangePtrVec fix_change_vec = proteoform->getChangePtrVec(ChangeType::FIXED);
  double ori_mass = change_ptr->getMassShift();
  mass = ori_mass;
  int ori_start = proteoform->getStartPos(), ori_end = proteoform->getEndPos();

  std::string n_seq, c_seq;
  std::vector<bool> ptm_known_vec;
  std::vector<int> c_vec, n_vec;
  std::vector<double> raw_scr_vec;

  for (int i = n_trunc_min; i <= n_trunc_max; i++) {
    for (int j = c_trunc_min; j <= c_trunc_max; j++) {
      if (i < 0) {
        n_seq = proteoform->getFastaSeqPtr()->getSeq().substr(ori_start + i, -i);
        mass = ori_mass - getPeptideMass(n_seq);
      } else {
        n_seq = proteoform->getFastaSeqPtr()->getSeq().substr(ori_start, i);
        mass = ori_mass + getPeptideMass(n_seq);
      }
      if (j >= 0) {
        c_seq = proteoform->getFastaSeqPtr()->getSeq().substr(ori_end + 1, j);
        mass = mass - getPeptideMass(c_seq);
      } else {
        c_seq = proteoform->getFastaSeqPtr()->getSeq().substr(ori_end + 1 + j, -j);
        mass = mass + getPeptideMass(c_seq);
      }

      if (std::abs(mass) > mng_ptr_->max_ptm_mass_ && (i != 0 || j != 0))
        continue;

      if (std::abs(mass) < MassConstant::getIsotopeMass() + err) {
        change_ptr->setMassShift(mass);
        fix_change_vec.push_back(change_ptr);
        proteoform = geneProteoform(proteoform->getFastaSeqPtr(),
                                    proteoform->getProtModPtr(), ori_start + i, ori_end + j, 
                                    fix_change_vec, 
                                    mng_ptr_->prsm_para_ptr_->getFixModPtrVec());
        return;
      }

      n_vec.push_back(i);
      c_vec.push_back(j);
      PtmPtrVec ptm_vec_tmp = getPtmPtrVecByMass(mass, err);
      ptm_known_vec.push_back(ptm_vec_tmp.size() > 0);
      change_ptr->setMassShift(mass);
      ChangePtrVec new_change_vec;
      new_change_vec.insert(new_change_vec.end(), fix_change_vec.begin(), fix_change_vec.end());
      new_change_vec.push_back(change_ptr);
      std::sort(new_change_vec.begin(), new_change_vec.end(), Change::cmpPosInc);
      proteoform = geneProteoform(proteoform->getFastaSeqPtr(),
                                  proteoform->getProtModPtr(), ori_start + i, ori_end + j, 
                                  new_change_vec, mng_ptr_->prsm_para_ptr_->getFixModPtrVec());
      double raw_scr;
      std::vector<double> scr_vec;
      compOnePtmScr(proteoform, extend_ms_ptr_vec, scr_vec, raw_scr, ptm_vec_tmp);
      raw_scr_vec.push_back(raw_scr);
    }
  }

  for (size_t i = 0; i < raw_scr_vec.size(); i++) {
    if (ptm_known_vec[i])
      raw_scr_vec[i] = raw_scr_vec[i] * mng_ptr_->theta_;
    else
      raw_scr_vec[i] = raw_scr_vec[i] * (1 - mng_ptr_->theta_);
  }
  int idx = std::distance(raw_scr_vec.begin(), std::max_element(raw_scr_vec.begin(), raw_scr_vec.end()));
  if (n_vec[idx] < 0) {
    n_seq = proteoform->getFastaSeqPtr()->getSeq().substr(ori_start + n_vec[idx], -n_vec[idx]);
    mass = ori_mass - getPeptideMass(n_seq);
  } else {
    n_seq = proteoform->getFastaSeqPtr()->getSeq().substr(ori_start, n_vec[idx]);
    mass = ori_mass + getPeptideMass(n_seq);
  }
  if (c_vec[idx] >= 0) {
    c_seq = proteoform->getFastaSeqPtr()->getSeq().substr(ori_end + 1, c_vec[idx]);
    mass = mass - getPeptideMass(c_seq);
  } else {
    c_seq = proteoform->getFastaSeqPtr()->getSeq().substr(ori_end + 1 + c_vec[idx], -c_vec[idx]);
    mass = mass + getPeptideMass(c_seq);
  }
  change_ptr->setMassShift(ori_mass);
  ChangePtrVec new_change_vec;
  new_change_vec.insert(new_change_vec.end(), fix_change_vec.begin(), fix_change_vec.end());
  new_change_vec.push_back(geneUnexpectedChange(change_ptr, mass));
  std::sort(new_change_vec.begin(), new_change_vec.end(), Change::cmpPosInc);
  proteoform = geneProteoform(proteoform->getFastaSeqPtr(),
                              proteoform->getProtModPtr(), ori_start + n_vec[idx], ori_end + c_vec[idx], 
                              new_change_vec, mng_ptr_->prsm_para_ptr_->getFixModPtrVec());
}

void LocalUtil::twoPtmTermAdjust(ProteoformPtr & proteoform, int num_match, 
                                 const ExtendMsPtrVec & extend_ms_ptr_vec, double prec_mass,
                                 double & mass1, double & mass2) {

  int n_trunc_min, n_trunc_max, c_trunc_min, c_trunc_max;
  getNtermTruncRange(proteoform, extend_ms_ptr_vec, n_trunc_min, n_trunc_max);
  getCtermTruncRange(proteoform, extend_ms_ptr_vec, c_trunc_min, c_trunc_max);

  ChangePtr change_ptr1 = proteoform->getChangePtrVec(ChangeType::UNEXPECTED)[0];
  ChangePtr change_ptr2 = proteoform->getChangePtrVec(ChangeType::UNEXPECTED)[1];
  ChangePtrVec fix_change_vec = proteoform->getChangePtrVec(ChangeType::FIXED);
  int ori_start = proteoform->getStartPos(), ori_end = proteoform->getEndPos();
  double ori_mass1 = change_ptr1->getMassShift(), ori_mass2 = change_ptr2->getMassShift();
  mass1 = ori_mass1, mass2 = ori_mass2;

  std::vector<bool> ptm1_known_vec, ptm2_known_vec;
  std::vector<int> c_vec, n_vec;
  std::string n_seq, c_seq;
  std::vector<double> raw_scr_vec;

  for (int i = n_trunc_min; i <= n_trunc_max; i++) {
    for (int j = c_trunc_min; j <= c_trunc_max; j++) {
      if (i < 0) {
        n_seq = proteoform->getFastaSeqPtr()->getSeq().substr(ori_start + i, -i);
        mass1 = ori_mass1 - getPeptideMass(n_seq);
      } else {
        n_seq = proteoform->getFastaSeqPtr()->getSeq().substr(ori_start, i);
        mass1 = ori_mass1 + getPeptideMass(n_seq);
      }
      if (j >= 0) {
        c_seq = proteoform->getFastaSeqPtr()->getSeq().substr(ori_end + 1, j);
        mass2 = ori_mass2 - getPeptideMass(c_seq);
      } else {
        c_seq = proteoform->getFastaSeqPtr()->getSeq().substr(ori_end + 1 + j, -j);
        mass2 = ori_mass2 + getPeptideMass(c_seq);
      }

      if (std::abs(mass1) > mng_ptr_->max_ptm_mass_ 
          && std::abs(mass2) > mng_ptr_->max_ptm_mass_
          && (i != 0 || j != 0))
        continue;

      n_vec.push_back(i);
      c_vec.push_back(j);

      PtmPairVec ptm_pair_vec = getPtmPairVecByMass(mass1, mass2, prec_mass * ppm_);
      if (ptm_pair_vec.size() == 0) {
        ptm_pair_vec.push_back(std::make_pair(nullptr, nullptr));
      }
      ptm1_known_vec.push_back(getPtmPtrVecByMass(mass1, prec_mass * ppm_).size() > 0);
      ptm2_known_vec.push_back(getPtmPtrVecByMass(mass2, prec_mass * ppm_).size() > 0);
      change_ptr1->setMassShift(mass1);
      change_ptr2->setMassShift(mass2);
      ChangePtrVec new_change_vec;
      new_change_vec.insert(new_change_vec.end(), fix_change_vec.begin(), fix_change_vec.end());
      new_change_vec.push_back(change_ptr1); new_change_vec.push_back(change_ptr2);
      std::sort(new_change_vec.begin(), new_change_vec.end(), Change::cmpPosInc);

      proteoform = geneProteoform(proteoform->getFastaSeqPtr(),
                                  proteoform->getProtModPtr(), ori_start + i, ori_end + j, 
                                  new_change_vec, mng_ptr_->prsm_para_ptr_->getFixModPtrVec());
      double raw_scr;
      compTwoPtmScr(proteoform, num_match, extend_ms_ptr_vec, prec_mass, raw_scr, ptm_pair_vec);
      raw_scr_vec.push_back(raw_scr);
    }
  }

  for (size_t i = 0; i < raw_scr_vec.size(); i++) {
    if (ptm1_known_vec[i] && ptm2_known_vec[i])
      raw_scr_vec[i] = raw_scr_vec[i] * mng_ptr_->theta_ * mng_ptr_->theta_;
    else if (ptm1_known_vec[i] || ptm2_known_vec[i])
      raw_scr_vec[i] = raw_scr_vec[i] * mng_ptr_->theta_ * (1 - mng_ptr_->theta_);
    else
      raw_scr_vec[i] = raw_scr_vec[i] * (1 - mng_ptr_->theta_) * (1 - mng_ptr_->theta_);
  }
  int idx = std::distance(raw_scr_vec.begin(), std::max_element(raw_scr_vec.begin(), raw_scr_vec.end()));
  if (n_vec[idx] < 0) {
    n_seq = proteoform->getFastaSeqPtr()->getSeq().substr(ori_start + n_vec[idx], -n_vec[idx]);
    mass1 = ori_mass1 - getPeptideMass(n_seq);
  } else {
    n_seq = proteoform->getFastaSeqPtr()->getSeq().substr(ori_start, n_vec[idx]);
    mass1 = ori_mass1 + getPeptideMass(n_seq);
  }

  if (c_vec[idx] >= 0) {
    c_seq = proteoform->getFastaSeqPtr()->getSeq().substr(ori_end + 1, c_vec[idx]);
    mass2 = ori_mass2 - getPeptideMass(c_seq);
  } else {
    c_seq = proteoform->getFastaSeqPtr()->getSeq().substr(ori_end + 1 + c_vec[idx], -c_vec[idx]);
    mass2 = ori_mass2 + getPeptideMass(c_seq);
  }
  change_ptr1->setMassShift(ori_mass1);
  change_ptr2->setMassShift(ori_mass2);
  ChangePtrVec new_change_vec;
  new_change_vec.insert(new_change_vec.end(), fix_change_vec.begin(), fix_change_vec.end());
  new_change_vec.push_back(geneUnexpectedChange(change_ptr1, mass1));
  new_change_vec.push_back(geneUnexpectedChange(change_ptr2, mass2));
  std::sort(new_change_vec.begin(), new_change_vec.end(), Change::cmpPosInc);
  proteoform = geneProteoform(proteoform->getFastaSeqPtr(),
                              proteoform->getProtModPtr(), ori_start + n_vec[idx], ori_end + c_vec[idx], 
                              new_change_vec, mng_ptr_->prsm_para_ptr_->getFixModPtrVec());
}

void LocalUtil::readPtmTxt(const std::string &file_name) {
  std::ifstream infile(file_name.c_str());
  std::string line;
  while(std::getline(infile, line)) {
    if (line == "" || line[0] == '#') continue;

    line = StringUtil::rmComment(line);
    std::vector<std::string> l = StringUtil::split(line, ',');
    PtmPtr p = PtmBase::getPtmPtrByAbbrName(l[0]);
    var_ptm_list_.push_back(p);
  }

  std::sort(var_ptm_list_.begin(), var_ptm_list_.end(), Ptm::cmpMassInc);
  auto end = std::unique(var_ptm_list_.begin(), var_ptm_list_.end());
  var_ptm_list_.erase(end, var_ptm_list_.end());

  for (size_t i = 0; i < var_ptm_list_.size(); i++) {
    for (size_t j = 0; j < var_ptm_list_.size(); j++) {
      ptm_pair_vec_.push_back(std::make_pair(var_ptm_list_[i], var_ptm_list_[j]));
    }
  }
}

void LocalUtil::compOnePtmScr(ProteoformPtr proteoform, const ExtendMsPtrVec & extend_ms_ptr_vec, 
                              std::vector<double> &scr_vec, double & raw_scr, PtmPtrVec & ptm_vec) {
  raw_scr = 0.0;
  ChangePtr change_ptr = proteoform->getChangePtrVec(ChangeType::UNEXPECTED)[0];

  if (ptm_vec.size() == 0) ptm_vec.push_back(nullptr);

  std::vector<double> temp;
  std::vector<std::vector<double>> scr_vec2d;
  int n = proteoform->getLen();
  int count = 0;

  for (size_t i = 0; i < ptm_vec.size(); i++) {
    scr_vec.clear();
    for (int j = 0; j < n; j++) {
      if (LocalUtil::modifiable(proteoform, j, ptm_vec[i])) {
        count++;
        change_ptr->setLeftBpPos(j);
        change_ptr->setRightBpPos(j + 1);
        int match = compNumPeakIonPairs(proteoform, extend_ms_ptr_vec);
        scr_vec.push_back(std::pow(p1_, n - match) * std::pow(p2_, match));
      } else {
        scr_vec.push_back(0.0);
      }
    }
    scr_vec2d.push_back(scr_vec);
    temp.push_back(std::accumulate(scr_vec.begin(), scr_vec.end(), 0.0));
  }

  scr_vec.clear();
  int idx = std::distance(temp.begin(), std::max_element(temp.begin(), temp.end()));

  if (temp[idx] == 0) return;

  raw_scr = std::accumulate(scr_vec2d[idx].begin(), scr_vec2d[idx].end(), 0.0) / count;
  scr_vec = LocalUtil::normalize(scr_vec2d[idx]);
  PtmPtr p = ptm_vec[idx];
  ptm_vec.clear();
  ptm_vec.push_back(p);
}

void two_ptm_mass_adjust(double & mass1, double & mass2, PtmPtr p1, PtmPtr p2) {
  if (p1 == nullptr || p2 == nullptr) return; 
  double err = mass1 + mass2 - p1->getMonoMass() - p2->getMonoMass();
  if (std::abs(err) < MassConstant::getIsotopeMass()) {
    mass1 = p1->getMonoMass() + err / 2;
    mass2 = p2->getMonoMass() + err / 2;
  } else if (err > MassConstant::getIsotopeMass()) {
    err = err - MassConstant::getIsotopeMass();
    mass1 = p1->getMonoMass() + MassConstant::getIsotopeMass() + err / 2;
    mass2 = p2->getMonoMass() + err / 2; 
  } else {
    err = err + MassConstant::getIsotopeMass();
    mass1 = p1->getMonoMass() - MassConstant::getIsotopeMass() + err / 2;
    mass2 = p2->getMonoMass() + err / 2;
  }
}

void LocalUtil::compTwoPtmScr(ProteoformPtr proteoform, int num_match, 
                              const ExtendMsPtrVec & extend_ms_ptr_vec, double prec_mass,
                              double & raw_scr, PtmPairVec & ptm_pair_vec) {
  double mass1 = proteoform->getChangePtrVec(ChangeType::UNEXPECTED)[0]->getMassShift(); 
  double mass2 = proteoform->getChangePtrVec(ChangeType::UNEXPECTED)[1]->getMassShift();
  std::vector<double> scr_vec;
  ProteoformPtr no_unexpected =
      geneProteoform(proteoform->getFastaSeqPtr(),
                     proteoform->getProtModPtr(), proteoform->getStartPos(),
                     proteoform->getEndPos(),
                     getExpectedChangeVec(proteoform), mng_ptr_->prsm_para_ptr_->getFixModPtrVec());

  for (size_t i = 0; i < ptm_pair_vec.size(); i++) {
    two_ptm_mass_adjust(mass1, mass2, ptm_pair_vec[i].first, ptm_pair_vec[i].second);
    scr_vec.push_back(dpTwoPtmScr(no_unexpected, num_match, extend_ms_ptr_vec, prec_mass,
                                  mass1, mass2, ptm_pair_vec[i].first, ptm_pair_vec[i].second));
  }

  int idx = std::distance(scr_vec.begin(), std::max_element(scr_vec.begin(), scr_vec.end()));
  raw_scr = scr_vec[idx];
  PtmPtr p1 = ptm_pair_vec[idx].first;
  PtmPtr p2 = ptm_pair_vec[idx].second;
  ptm_pair_vec.clear();
  ptm_pair_vec.push_back(std::make_pair(p1, p2));
}

std::vector<std::pair<double, double>> getSpecPeak(const ExtendMsPtrVec & extend_ms_ptr_vec) {
  std::vector<std::pair<double, double>> spec_peak;
  for (size_t i = 0; i < extend_ms_ptr_vec.size(); i++) {
    for (size_t j = 0; j < extend_ms_ptr_vec[i]->getPeakPtrVec().size(); j++) {
      spec_peak.push_back(std::make_pair(extend_ms_ptr_vec[i]->getPeakPtr(j)->getMonoMass(),
                                         extend_ms_ptr_vec[i]->getPeakPtr(j)->getOrigTolerance()));
    }
  }
  return spec_peak;
}

void compNumMatch(const std::vector<double> & b, std::vector<int> & s, 
                  std::vector<std::pair<double, double>> spec_peak, double prec_mass) {

  size_t i = 0, j = 0;
  while (i < b.size() && j < spec_peak.size()) {
    if (std::abs(spec_peak[j].first - b[i]) <= spec_peak[j].second) {
      s[i]++; i++; j++;
    } else if (b[i] > spec_peak[j].first) {
      j++;
    } else {
      i++;
    }
  }
  i = b.size() - 1, j = 0;
  while ((int)i >= 0 && j < spec_peak.size()) {
    if (std::abs(spec_peak[j].first - prec_mass + b[i]) <= spec_peak[j].second) {
      s[i]++; i--; j++;
    } else if (prec_mass - b[i] > spec_peak[j].first) {
      j++;
    } else {
      i--;
    }
  } 
}

void fillTableB(std::vector<std::vector<double>> & b_table, const std::string & seq,
                double mass1, double mass2, ChangePtrVec change_vec) {
  for (size_t i = 1; i < b_table.size(); i++) {
    b_table[i].resize(b_table[0].size());
    std::fill(b_table[i].begin(), b_table[i].end(), 0);
  }

  for (size_t i = 0; i < b_table[0].size(); i++) {
    b_table[1][i] = b_table[0][i] + mass1;
    b_table[2][i] = b_table[1][i] + mass2;
  }
}

std::vector<double> geneNTheoMass(ProteoformPtr proteoform, const ExtendMsPtrVec & extend_ms_ptr_vec,
                                  double min_mass){
  TheoPeakPtrVec theo_peaks;
  for (size_t i = 0; i < extend_ms_ptr_vec.size(); i++) {
    TheoPeakPtrVec theo_tmp = 
        TheoPeakFactory::geneProteoformTheoPeak(proteoform, 
                                                extend_ms_ptr_vec[i]->getMsHeaderPtr()->getActivationPtr(),
                                                min_mass);
    theo_peaks.insert(theo_peaks.end(), theo_tmp.begin(), theo_tmp.end());
  }

  std::vector<double> res;
  for (size_t i = 0; i < theo_peaks.size(); i++) {
    if (theo_peaks[i]->getIonPtr()->getIonTypePtr()->isNTerm()) {
      res.push_back(theo_peaks[i]->getModMass());
    }
  }
  return res;
} 

void fillTableS(std::vector<std::vector<double>> & b_table, std::vector<std::vector<int>> & s_table,
                std::vector<std::pair<double, double>> spec_peak, double prec_mass) {

  for (size_t i = 0; i < s_table.size(); i++) {
    s_table[i].resize(b_table[i].size());
    std::fill(s_table[i].begin(), s_table[i].end(), 0);
  }

  for (int i = 0; i < 3; i++) {
    compNumMatch(b_table[i], s_table[i], spec_peak, prec_mass);
  }
}

double LocalUtil::dpTwoPtmScr(ProteoformPtr proteoform, int h, const ExtendMsPtrVec & extend_ms_ptr_vec, 
                              double prec_mass, double mass1, double mass2, PtmPtr ptm1, PtmPtr ptm2) {

  std::vector<std::pair<double, double>> spec_peak = getSpecPeak(extend_ms_ptr_vec);

  int g = proteoform->getLen();

  std::string seq = proteoform->getFastaSeqPtr()->getSeq().substr(proteoform->getStartPos(), g);

  std::vector<std::vector<double>> b_table(3);
  b_table[0] = geneNTheoMass(proteoform, extend_ms_ptr_vec,mng_ptr_->min_mass_);
  std::sort(b_table[0].begin(), b_table[0].end());
  fillTableB(b_table, seq, mass1, mass2, getExpectedChangeVec(proteoform));

  std::vector<std::vector<int>> s_table(3);
  fillTableS(b_table, s_table, spec_peak, prec_mass);

  // fill D(f,g,h)
  int d_table[3][g + 1][h + 1];
  memset(d_table, 0, sizeof(int) * 3 * (g + 1) * (h + 1));
  d_table[0][0][0] = 1;

  for (int i = 1; i <= g; i++) {
    for (int j = 0; j <= h; j++) {
      if (j >= s_table[0][i - 1])
        d_table[0][i][j] = d_table[0][i - 1][j - s_table[0][i - 1]];
      else
        d_table[0][i][j] = 0;
    }
  }

  for (int i = 1; i <= g; i++) {
    for (int j = 0; j <= h; j++) {
      if (modifiable(proteoform, i - 1, ptm1) && j >= s_table[1][i - 1])
        d_table[1][i][j] = d_table[0][i - 1][j - s_table[1][i - 1]] + d_table[1][i - 1][j - s_table[1][i - 1]];
      else if (j >= s_table[1][i - 1])
        d_table[1][i][j] = d_table[1][i - 1][j - s_table[1][i - 1]];
      else
        d_table[1][i][j] = 0;
    }
  }

  for (int i = 1; i <= g; i++) {
    for (int j = 0; j <= h; j++) {
      if (modifiable(proteoform, i - 1, ptm2) && j >= s_table[2][i - 1])
        d_table[2][i][j] = d_table[1][i - 1][j - s_table[2][i - 1]] + d_table[2][i - 1][j - s_table[2][i - 1]];
      else if (j >= s_table[2][i - 1])
        d_table[2][i][j] = d_table[2][i - 1][j - s_table[2][i - 1]];
      else
        d_table[2][i][j] = 0;
    }
  }

  double scr = 0.0;
  int count = 0;
  for (int i = 0; i <= h; i++) {
    count += d_table[2][g][i];
    scr += d_table[2][g][i] * std::pow(p1_, g - i) * std::pow(p2_, i);
  }
  return scr / count;
}

void LocalUtil::compSplitPoint(ProteoformPtr & proteoform, int h, const ExtendMsPtrVec & extend_ms_ptr_vec,
                               double prec_mass) {
  ChangePtr change_ptr1 = proteoform->getChangePtrVec(ChangeType::UNEXPECTED)[0];
  ChangePtr change_ptr2 = proteoform->getChangePtrVec(ChangeType::UNEXPECTED)[1];
  double mass1 = change_ptr1->getMassShift(), mass2 = change_ptr2->getMassShift();
  PtmPtr ptm1 = change_ptr1->getLocalAnno()->getPtmPtr();
  PtmPtr ptm2 = change_ptr2->getLocalAnno()->getPtmPtr();
  two_ptm_mass_adjust(mass1, mass2, ptm1, ptm2);
  change_ptr1->setMassShift(mass1);
  change_ptr2->setMassShift(mass2);

  std::vector<std::pair<double, double>> spec_peak = getSpecPeak(extend_ms_ptr_vec);

  int g = proteoform->getLen();

  std::string seq = proteoform->getFastaSeqPtr()->getSeq();

  ProteoformPtr no_unexpected =
      geneProteoform(proteoform->getFastaSeqPtr(),
                     proteoform->getProtModPtr(), proteoform->getStartPos(),
                     proteoform->getEndPos(),
                     getExpectedChangeVec(proteoform), mng_ptr_->prsm_para_ptr_->getFixModPtrVec());

  std::vector<std::vector<double>> b_table(3);
  b_table[0] = geneNTheoMass(no_unexpected, extend_ms_ptr_vec,mng_ptr_->min_mass_);
  std::sort(b_table[0].begin(), b_table[0].end());
  fillTableB(b_table, seq, mass1, mass2, getExpectedChangeVec(proteoform));

  std::vector<std::vector<int>> s_table(3);
  fillTableS(b_table, s_table, spec_peak, prec_mass);

  int d_table[3][g + 1][h + 1];

  std::vector<double> split_scr_vec;
  for (int k = 1; k < g; k++) {
    memset(d_table, 0, sizeof(int) * 3 * (g + 1) * (h + 1));
    d_table[0][0][0] = 1;

    for (int i = 1; i <= g; i++) {
      for (int j = 0; j <= h; j++) {
        if (j >= s_table[0][i - 1])
          d_table[0][i][j] = d_table[0][i - 1][j - s_table[0][i - 1]];
        else
          d_table[0][i][j] = 0;
      }
    }

    for (int i = 1; i <= g; i++) {
      for (int j = 0; j <= h; j++) {
        if (modifiable(proteoform, i - 1, ptm1) && j >= s_table[1][i - 1] && i <= k)
          d_table[1][i][j] = d_table[0][i - 1][j - s_table[1][i - 1]] + d_table[1][i - 1][j - s_table[1][i - 1]];
        else if (j >= s_table[1][i - 1])
          d_table[1][i][j] = d_table[1][i - 1][j - s_table[1][i - 1]];
        else
          d_table[1][i][j] = 0;
      }
    }

    for (int i = 1; i <= g; i++) {
      for (int j = 0; j <= h; j++) {
        if (modifiable(proteoform, i - 1, ptm2) && j >= s_table[2][i - 1] && i > k)
          d_table[2][i][j] = d_table[1][i - 1][j - s_table[2][i - 1]] + d_table[2][i - 1][j - s_table[2][i - 1]];
        else if (j >= s_table[2][i - 1])
          d_table[2][i][j] = d_table[2][i - 1][j - s_table[2][i - 1]];
        else
          d_table[2][i][j] = 0;
      }
    }

    double scr = 0.0;
    for (int i = 0; i <= h; i++) {
      scr += d_table[2][g][i] * std::pow(p1_, i) * std::pow(p2_, i);
    }
    split_scr_vec.push_back(scr);
  }

  int split_point = std::distance(split_scr_vec.begin(), std::max_element(split_scr_vec.begin(), split_scr_vec.end()));

  double split_max = *std::max_element(split_scr_vec.begin(), split_scr_vec.end());

  double split_scr = 0.0;

  int split_end = split_scr_vec.size();

  for (;split_end > 0; split_end--) {
    if (split_scr_vec[split_end] == split_max)
      break;
  }

  for (int i = split_point; i <= split_end; i++) {
    split_scr += split_scr_vec[i];
  }

  split_point = (split_point + split_end) / 2;

  split_scr = split_scr / std::accumulate(split_scr_vec.begin(), split_scr_vec.end(), 0.0);

  if (split_scr <= mng_ptr_->threshold_) {
    proteoform = nullptr;
    return;
  }

  std::vector<double> ptm_scr;

  for (int i = 0; i <= split_point; i++) {
    change_ptr1->setLeftBpPos(i);
    change_ptr1->setRightBpPos(i + 1);
    if (modifiable(proteoform, i, ptm1)) {
      int match = compNumPeakIonPairs(proteoform, extend_ms_ptr_vec );
      ptm_scr.push_back(std::pow(p1_, split_point - match) * std::pow(p2_, match));
    } else {
      ptm_scr.push_back(0.0);
    }
  }

  ptm_scr = normalize(ptm_scr);
  int bgn, end;
  double conf;
  std::transform(ptm_scr.begin(), ptm_scr.end(), ptm_scr.begin(),
                 std::bind1st(std::multiplies<double>(), split_scr));
  scr_filter(ptm_scr, bgn, end, conf, mng_ptr_->threshold_);
  LocalAnnoPtr anno1 = std::make_shared<LocalAnno>(bgn, end, conf, ptm_scr, 0, ptm1);
  change_ptr1->setLocalAnno(anno1);
  ptm_scr.clear();
  int len = proteoform->getLen();
  for (int i = split_point + 1; i < len; i++) {
    change_ptr2->setLeftBpPos(i);
    change_ptr2->setRightBpPos(i + 1);
    if (modifiable(proteoform, i, ptm2)) {
      int match = compNumPeakIonPairs(proteoform, extend_ms_ptr_vec );
      ptm_scr.push_back(std::pow(p1_, len - match) * std::pow(p2_, match));
    } else {
      ptm_scr.push_back(0.0);
    }
  }
  ptm_scr = normalize(ptm_scr);
  std::transform(ptm_scr.begin(), ptm_scr.end(), ptm_scr.begin(),
                 std::bind1st(std::multiplies<double>(), split_scr));
  scr_filter(ptm_scr, bgn, end, conf, mng_ptr_->threshold_);
  LocalAnnoPtr anno2 = std::make_shared<LocalAnno>(split_point + bgn, split_point + end, conf, ptm_scr, 0, ptm1);
  change_ptr2->setLocalAnno(anno2);
}

} // namespace prot
