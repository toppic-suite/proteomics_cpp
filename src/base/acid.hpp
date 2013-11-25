/*
 * author  Xiaowen Liu
 * date    2013-11-01
 */

#ifndef PROT_ACID_H_
#define PROT_ACID_H_

#include <memory>
#include <string>
#include <vector>

#include <xercesc/dom/DOM.hpp>

namespace prot {


class Acid {
 public:
  Acid (const std::string &name, const std::string &one_letter, 
        const std::string &three_letter, const std::string &composition, 
        double mono_mass, double avg_mass); 

  /* Get amino acid composition. */
  std::string getAcidComposition() {return composition_;}

  /* Get average mass. */
  double getAvgMass() {return avg_mass_;}

  /* Get  monoisotopic mass. */
  double getMonoMass() {return mono_mass_;}

  /* Get amino acid name. */
  std::string getName() {return name_;}

  /* Get amino acid one letter representation. */
  std::string getOneLetter() {return one_letter_;}

  /* Get amino acid three letter representation. */
  std::string getThreeLetter() {return three_letter_;}

 private:
  /* Name of amino acid */
  std::string name_;
  /* One letter representation */
  std::string one_letter_;
  /* Three letter representation */
  std::string three_letter_;
  /* amino acid chemical composition */
  std::string composition_;
  /* residue monoisotopic mass */
  double mono_mass_;
  /* residue average mass */
  double avg_mass_;
};

typedef std::shared_ptr<Acid> AcidPtr;
typedef std::vector<AcidPtr> AcidPtrVec;

AcidPtrVec getAcidListInstance(const char* file_name);

/**
 * Returns an amino acid based on the the name. Returns null if the amino
 * acid name does not exist.
 */
AcidPtr getAcidPtrByName(AcidPtrVec &acid_list, 
                         const std::string &name);
/**
 * Returns an amino acid based on the one letter representation. Returns
 * null if the one letter representation does not exist.
 */
AcidPtr getAcidPtrByOneLetter(AcidPtrVec &acid_list, 
                              const std::string &one_letter);
/**
 * Returns an amino acid based on the three letter representation. Returns
 * null if the three letter representation does not exist.
 */
AcidPtr getAcidPtrByThreeLetter(AcidPtrVec &acid_list, 
                                const std::string &three_letter);

/**
 * Checks if the list contains an amino acid with the specific name.
 */
bool containsName(AcidPtrVec &acid_list, const std::string &name);

/**
 * Checks if the list contains an amino acid with the specific one letter
 * representation.
 */
bool containsOneLetter(AcidPtrVec &acid_list, const std::string &one_letter);

/**
 * Checks if the list contains an amino acid with the specific three letter
 * representation.
 */
bool containsThreeLetter(AcidPtrVec &acid_list, const std::string &three_letter);

/**
 * Converts a protein sequence (with one letter representation of amino
 * acids) to an amino acid array.
 */
AcidPtrVec convertSeqToAcidSeq(AcidPtrVec &acid_list, 
                               const std::string &seq);
}
#endif

