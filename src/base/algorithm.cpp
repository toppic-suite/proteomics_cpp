#include <cmath>
#include <limits>

#include "base/algorithm.hpp"
#include "base/logger.hpp"

namespace prot {

/* if we need to increase i, return true, otherwise, return false. */
bool increaseIJ(unsigned int i, unsigned int j, double deviation, 
                double tolerance, const std::vector<double> &ms_masses, 
                const std::vector<double> &theo_masses) {
  /*
   * we assume that each real peak is matched to at most one theoretical
   * peak, so we do not check i and j+1
   */
  if (deviation <= 0) {
    return true;
  }
  /* severl real peak can be matched to the same theoretical peak */
  if (i >= ms_masses[ms_masses.size() - 1] ) {
    return false;
  }

  double next_pos = ms_masses[i+1];
  bool j_is_closer
      = std::abs(next_pos - theo_masses[j]) < std::abs(next_pos - theo_masses[j+1]);
  if (std::abs(next_pos - theo_masses[j]) <= tolerance  
      && (j == theo_masses.size() - 1 || j_is_closer)) { 
    return true;
  } else {
    return false;
  }
}

/* compute deviation for each peak */
std::vector<double> compMsMassPpos(const std::vector<double> &ms_masses, 
                                   const std::vector<double> &theo_masses, 
                                   double ppo) {
  // extendMsThree do not have 0 and precursor mass 
  std::vector<double> min_distances;
  for (unsigned i = 0; i < ms_masses.size(); i++) {
    min_distances.push_back(std::numeric_limits<double>::infinity());
  }
  unsigned int i = 0;
  unsigned int j = 0;
  while (i < ms_masses.size() && j < theo_masses.size()) {
    double d = ms_masses[i] - theo_masses[j];
    if (std::abs(d) <= std::abs(min_distances[i])) {
      min_distances[i] = d;
    }
    double tolerance = ms_masses[i] * ppo;
    if (increaseIJ(i, j, d,  tolerance, ms_masses, theo_masses)) {
      i++;
    } else {
      j++;
    }
  }
  // change distance to ppo
  std::vector<double> result_ppos;
  for (i = 0; i < ms_masses.size(); i++) {
    if (ms_masses[i] > 0) {
      result_ppos.push_back (min_distances[i]/ ms_masses[i]); 
    }
    else {
      result_ppos.push_back(std::numeric_limits<double>::infinity());
    }
  }
  return result_ppos;
}

std::vector<double> compTheoMassPpos(const std::vector<double> &ms_masses, 
                                     const std::vector<double> &theo_masses,
                                     double ppo) {

  std::vector<double> min_distances;
  for (unsigned p = 0; p < theo_masses.size(); p++) {
    min_distances.push_back(std::numeric_limits<double>::infinity());
  }
  /* extendMsThree do not have 0 and precursor mass */
  unsigned int i = 0;
  unsigned int j = 0;
  while (i < ms_masses.size() && j < theo_masses.size()) {
    double d = ms_masses[i] - theo_masses[j];
    if (std::abs(d) <= std::abs(min_distances[j])) {
      min_distances[j] = d;
    }
    double tolerance = ms_masses[i] * ppo;
    if (increaseIJ(i, j, d, tolerance, ms_masses, theo_masses)) {
      i++;
    } else {
      j++;
    }
  }
  // change distance to ppo
  std::vector<double> result_ppos;
  for (i = 0; i < theo_masses.size(); i++) {
    if (theo_masses[i] > 0) {
      result_ppos.push_back (min_distances[i]/ theo_masses[i]); 
    }
    else {
      result_ppos.push_back(std::numeric_limits<double>::infinity());
    }
  }
  return result_ppos;
}

/* compute the number of matched theoretical masses (fragment ions) */
double compNumMatchedTheoMasses (const std::vector<double> &ms_masses, 
                                 const std::vector<double> &theo_masses, 
                                 double ppo) {
  std::vector<double> theo_mass_ppos 
      = compTheoMassPpos(ms_masses, theo_masses, ppo);
  double score = 0;
  for (unsigned i = 0; i < theo_mass_ppos.size(); i++) {
    if (std::abs(theo_mass_ppos[i]) <= ppo) {
      score += 1.0;
    }
  }
  return score;
}

/* compute the position of the last residue of a proteoform based 
 * on its n term shift */
int getFirstResPos(double n_term_shift, const std::vector<double> &prm_masses){
  double trunc_mass = - n_term_shift;
  int best_pos = -1;
  double best_shift = std::numeric_limits<double>::infinity();
  for(unsigned int i = 0; i < prm_masses.size();i++){
    if(std::abs(prm_masses[i] - trunc_mass) < best_shift){
      best_pos = i;
      best_shift = std::abs(prm_masses[i] - trunc_mass);
    }
  }
  return best_pos;
}

/* compute the position of the last residue of a proteoform based on 
 * its c term shift */
int getLastResPos(double c_term_shift, const std::vector<double> &prm_masses){
  double trunc_mass = -c_term_shift;
  int best_pos = -1;
  double best_shift = std::numeric_limits<double>::infinity();
  double residue_mass_sum = prm_masses[prm_masses.size()-1];
  for(unsigned int i=0;i<prm_masses.size();i++){
    if (std::abs(residue_mass_sum-prm_masses[i]-trunc_mass)<best_shift){
      best_pos=i;
      best_shift = std::abs(residue_mass_sum-prm_masses[i]-trunc_mass);
    }
  }
  if(best_pos < 0){
    LOG_ERROR("get last residue position error! ");
    throw "get last residue position error!";
  }
  return best_pos - 1;
}

}
