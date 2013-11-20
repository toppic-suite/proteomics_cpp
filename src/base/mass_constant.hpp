#ifndef PROTOMICS_MASS_CONSTANT_H_
#define PROTOMICS_MASS_CONSTANT_H_

namespace proteomics {

class MassConstant {
 public:
	/**
	 * Returns the mass of an ammonia (NH3).
	 */
	static double getAmmoniaMass() {return 17.0265; }
	
	/**
	 * Returns the mass difference between two isotopic peaks.
	 */
  static double getIsotopeMass() { /* from Thrash paper */ return 1.00235; }

	/**
	 * Returns the mass of an oxygen molecular.
	 */
	static double getOxygenMass() { return 15.9949; }

	/**
	 * Returns the mass of a proton. 
	 */
	static double getProtonMass() { return 1.007276; }

	/**
	 * Returns the mass of a water molecule (H2O).
	 */
	static double getWaterMass() { return 18.010565; }
};

}
#endif
