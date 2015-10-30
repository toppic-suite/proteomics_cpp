/*
 * author  Xiaowen Liu
 * date    2013-11-01
 */

#ifndef PROT_ACID_BASE_HPP_
#define PROT_ACID_BASE_HPP_

#include "base/acid.hpp"

namespace prot {

/* acid base */
class AcidBase {
 public:
  static void initBase(const std::string &file_name);

  static const AcidPtrVec& getBaseAcidPtrVec() {return acid_ptr_vec_;}

  static AcidPtr getEmptyAcidPtr() {return empty_acid_ptr_;}

  /**
   * Returns an amino acid based on the the name. Returns null if the amino
   * acid name does not exist.
   */
  static AcidPtr getBaseAcidPtrByName(const std::string &name);

  /**
   * Returns an amino acid based on the one letter representation. Returns
   * null if the one letter representation does not exist.
   */
  static AcidPtr getBaseAcidPtrByOneLetter(const std::string &one_letter);

  /**
   * Returns an amino acid based on the three letter representation. Returns
   * null if the three letter representation does not exist.
   */
  static AcidPtr getBaseAcidPtrByThreeLetter(const std::string &three_letter);

  /**
   * Checks if the list contains an amino acid with the specific name.
   */
  static bool baseContainsName(const std::string &name);

  /**
   * Checks if the list contains an amino acid with the specific one letter
   * representation.
   */
  static bool baseContainsOneLetter(const std::string &one_letter);

  /**
   * Checks if the list contains an amino acid with the specific three letter
   * representation.
   */
  static bool baseContainsThreeLetter(const std::string &three_letter);

  /**
   * Converts a protein sequence (with one letter representation of amino
   * acids) to an amino acid array.
   */
  static AcidPtrVec convertSeqToAcidSeq(const std::string &seq);



 private:
  static AcidPtrVec acid_ptr_vec_;
  static AcidPtr empty_acid_ptr_;
};

}
#endif
