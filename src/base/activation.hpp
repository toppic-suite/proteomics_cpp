/*
 * Activation.hpp
 *
 *  Created on: Nov 25, 2013
 *      Author: xunlikun
 */

#ifndef PROT_ACTIVATION_HPP_
#define PROT_ACTIVATION_HPP_

#include "xercesc/dom/DOM.hpp"
#include "base/ion_type.hpp"

namespace prot {

class Activation {
public:
	Activation(std::string name,IonTypePtr n_ion_type_ptr,IonTypePtr c_ion_type_ptr);
	Activation(IonTypePtrVec ion_type_list,xercesc::DOMElement * element);
	std::string getName(){return name_;}
	double getNShit(){return n_ion_type_->getShift()-b_ion_shift_;}
	double getCShit(){return c_ion_type_->getShift()-y_ion_shift_;}
	IonTypePtr getNIonType(){return n_ion_type_;}
	IonTypePtr getCIonType(){return c_ion_type_;}
	void appendXml(XmlDOMDocument* xml_doc,xercesc::DOMElement* parent);

private:
	std::string name_;
	double b_ion_shift_ = 0;
	double y_ion_shift_ = 18.0106;
	IonTypePtr n_ion_type_;
	IonTypePtr c_ion_type_;
};

typedef std::shared_ptr<Activation> ActivationPtr;
typedef std::vector<ActivationPtr> ActivationPtrVec;

ActivationPtrVec getActivationPtrVecInstance(IonTypePtrVec ion_type_list, std::string file_name);
ActivationPtr getActivationPtrByName(ActivationPtrVec activation_list,std::string name);

} /* namespace prot */

#endif /* ACTIVATION_HPP_ */
