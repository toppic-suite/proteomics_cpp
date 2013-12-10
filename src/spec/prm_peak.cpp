/*
 * prm_peak.cpp
 *
 *  Created on: Dec 4, 2013
 *      Author: xunlikun
 */

#include <algorithm>
#include "spec/prm_peak.hpp"
#include "spec/spec_data.hpp"
#include "base/base_data.hpp"

namespace prot {
PrmPeak::PrmPeak(DeconvPeakPtr base_peak,PrmPeakTypePtr base_type,double mono_mass,double score):Peak(mono_mass,base_peak->getIntensity()){
	base_peak_=base_peak;
	base_type_=base_type;
	mono_mass_=mono_mass;
	score_=score;
	strict_tolerance_=0;
	n_strict_c_relax_tolerance_=0;
	n_relax_c_strict_tolerance_=0;
}

void PrmPeak::addNghbEdge(DeconvPeakPtr peak,double offset,SupportPeakTypePtr peak_type,double score){
	score_ +=score;
	neighbor_list_.push_back(SupportPeakPtr(new SupportPeak(peak,offset,score,peak_type)));
}

int PrmPeak::getBreakType(SupportPeakTypePtrVec support_peak_type_list){
	int break_type = 0;
	for(unsigned int i=0;i<neighbor_list_.size();i++){
		if(neighbor_list_[i]->getPeakType() == prot::getSupportPeakTypePtrByName(support_peak_type_list,"N_TERM")){
			if(break_type == 0){
				break_type = 1;
			}
			else if(break_type == 2){
				break_type = 3;
			}
		}
		else{
			if(break_type == 0){
				break_type = 2;
			}
			else if(break_type == 1){
				break_type = 3;
			}
		}
	}
	return break_type;
}

void addTwoMasses(PrmPeakPtrVec list,DeconvPeakPtr peak,double prec_mono_mass,ActivationPtr active_type,PrmPeakTypePtrVec prm_types){
	double orig_mass = peak->getMonoMass()-active_type->getNIonType()->getShift();
	PrmPeakPtr new_peak = PrmPeakPtr(new PrmPeak(peak,prot::getPrmPeakTypePtrByName(prm_types,"ORIGINAL"),orig_mass,1));
	list.push_back(new_peak);
	double reverse_mass = prec_mono_mass - (peak->getMonoMass()-active_type->getCIonType()->getShift());
	list.push_back(PrmPeakPtr(
			new PrmPeak(peak,prot::getPrmPeakTypePtrByName(prm_types,"REVERSED"),reverse_mass,1)
	));
}

void addSixMasses(PrmPeakPtrVec list,DeconvPeakPtr peak,double prec_mono_mass,ActivationPtr active_type,PrmPeakTypePtrVec prm_types,std::vector<double> offsets){
	for(unsigned int i = 0;i<offsets.size();i++){
		double mass = peak->getMonoMass()-active_type->getNIonType()->getShift()+offsets[i];
		list.push_back(PrmPeakPtr(
				new PrmPeak(peak,prot::getPrmPeakTypePtrByName(prm_types,"ORIGINAL"),mass,1)
		));
	}
	for(unsigned int i = 0;i<offsets.size();i++){
		double mass = peak->getMonoMass()-active_type->getCIonType()->getShift()+offsets[i];
		list.push_back(PrmPeakPtr(
				new PrmPeak(peak,prot::getPrmPeakTypePtrByName(prm_types,"REVERSED"),mass,1)
		));
	}
}


PrmPeakMS getMsTwo(DeconvMsPtr deconv_ms,double delta,SpParaPtr sp_para){
	//todo:xunlikun@config file name
	SpecDataPtr spec_data = SpecDataPtr(new SpecData("config file name"));
	BaseDataPtr base_data = BaseDataPtr(new BaseData("config file name"));

	MsHeaderPtr header = prot::getDeltaHeaderPtr(deconv_ms,delta);

	//getSpTwoPrmPeak
	double prec_mono_mass = header->getPrecMonoMass();
	ActivationPtr active_type = header->getActivationPtr();
	PrmPeakPtrVec list;
	for(unsigned int i = 0;i<deconv_ms->size();i++){
		addTwoMasses(list,deconv_ms->getPeakPtr(i),prec_mono_mass,active_type,spec_data->getPrmPeakTypePtrVec());
	}
	//filterPrmPeak
	PrmPeakPtrVec list_filtered;
	for(unsigned int i =0;i<list.size();i++){
		if(list[i]->getPosition() >= sp_para->getMinMass() &&
				list[i]->getPosition()  <= prec_mono_mass - sp_para->getMinMass()){
			list_filtered.push_back(list[i]);
		}
	}
	//end filterPrmPeak
	DeconvPeakPtr zero_peak = DeconvPeakPtr(new DeconvPeak(-1,0,0,0));
	list_filtered.push_back(PrmPeakPtr(new PrmPeak(zero_peak,prot::getPrmPeakTypePtrByName(spec_data->getPrmPeakTypePtrVec(),"ORIGINAL"),0,1)));
	DeconvPeakPtr prec_peak = DeconvPeakPtr(new DeconvPeak(-1,prec_mono_mass-prot::getIonTypePtrByName(base_data->getIonTypePtrVec(),"PREC")->getShift(),0,0));
	list_filtered.push_back(PrmPeakPtr(new PrmPeak(prec_peak,prot::getPrmPeakTypePtrByName(spec_data->getPrmPeakTypePtrVec(),"ORIGINAL"),
			prec_mono_mass-prot::getIonTypePtrByName(base_data->getIonTypePtrVec(),"PREC")->getShift(),1)));
	std::sort(list_filtered.begin(),list_filtered.end(),prmpeak_up);
	//end getSpPrmPeak


	//setTolerance
	list_filtered[0]->setStrictTolerance(sp_para->getPeakTolerance()->compStrictErrorTole(0));
	list_filtered[0]->setNStrictCRelacTolerance(sp_para->getPeakTolerance()->compStrictErrorTole(0));
	list_filtered[0]->setNRelaxCStrictTolerance(sp_para->getPeakTolerance()->compStrictErrorTole(prec_mono_mass));
	list_filtered[list_filtered.size()-1]->setStrictTolerance(sp_para->getPeakTolerance()->compStrictErrorTole(prec_mono_mass));
	list_filtered[list_filtered.size()-1]->setNStrictCRelacTolerance(sp_para->getPeakTolerance()->compStrictErrorTole(prec_mono_mass));
	list_filtered[list_filtered.size()-1]->setNRelaxCStrictTolerance(sp_para->getPeakTolerance()->compStrictErrorTole(0));

	for(unsigned int i=1;i< list_filtered.size()-1;i++){
		list_filtered[i]->setStrictTolerance(sp_para->getPeakTolerance()->compStrictErrorTole(list_filtered[i]->getBasePeak()->getMonoMass()));
		if(list_filtered[i]->getBaseType()==prot::getPrmPeakTypePtrByName(spec_data->getPrmPeakTypePtrVec(),"ORIGINAL")){
			list_filtered[i]->setNStrictCRelacTolerance(sp_para->getPeakTolerance()->compStrictErrorTole(list_filtered[i]->getBasePeak()->getMonoMass()));
			list_filtered[i]->setNRelaxCStrictTolerance(sp_para->getPeakTolerance()->compRelaxErrorTole(list_filtered[i]->getBasePeak()->getMonoMass(),prec_mono_mass));
		}
		else{
			//if the peak is M-m ,use base peak and parent mass to compute the error tolerance
			list_filtered[i]->setNRelaxCStrictTolerance(sp_para->getPeakTolerance()->compStrictErrorTole(list_filtered[i]->getBasePeak()->getMonoMass()));
			list_filtered[i]->setNStrictCRelacTolerance(sp_para->getPeakTolerance()->compRelaxErrorTole(list_filtered[i]->getBasePeak()->getMonoMass(),prec_mono_mass));
		}
	}
	//end settolerance

	return PrmPeakMS(new Ms<PrmPeakPtr>(header,list_filtered)) ;
}
PrmPeakMS getSpSix(DeconvMsPtr deconv_ms,double delta,SpParaPtr sp_para){

	//todo:xunlikun@config file name
	SpecDataPtr spec_data = SpecDataPtr(new SpecData("config file name"));
	BaseDataPtr base_data = BaseDataPtr(new BaseData("config file name"));

	MsHeaderPtr header = prot::getDeltaHeaderPtr(deconv_ms,delta);

	//getSpSixPrmPeak
	double prec_mono_mass = header->getPrecMonoMass();
	ActivationPtr active_type = header->getActivationPtr();
	PrmPeakPtrVec list;
	for(unsigned int i=0;i< deconv_ms->size();i++){
		if(deconv_ms->getPeakPtr(i)->getMonoMass() <=  sp_para->getExtendSpPara()->extend_min_mass_){
			addTwoMasses(list,deconv_ms->getPeakPtr(i),prec_mono_mass,active_type,spec_data->getPrmPeakTypePtrVec());
		}
		else{
			addSixMasses(list,deconv_ms->getPeakPtr(i),prec_mono_mass,active_type,spec_data->getPrmPeakTypePtrVec(),sp_para->getExtendSpPara()->ext_offsets_);
		}
	}

	//filterPrmPeak
	PrmPeakPtrVec list_filtered;
	for(unsigned int i =0;i<list.size();i++){
		if(list[i]->getPosition() >= sp_para->getMinMass() &&
				list[i]->getPosition()  <= prec_mono_mass - sp_para->getMinMass()){
			list_filtered.push_back(list[i]);
		}
	}
	//end filterPrmPeak
	DeconvPeakPtr zero_peak = DeconvPeakPtr(new DeconvPeak(-1,0,0,0));
	list_filtered.push_back(PrmPeakPtr(new PrmPeak(zero_peak,prot::getPrmPeakTypePtrByName(spec_data->getPrmPeakTypePtrVec(),"ORIGINAL"),0,1)));
	DeconvPeakPtr prec_peak = DeconvPeakPtr(new DeconvPeak(-1,prec_mono_mass-prot::getIonTypePtrByName(base_data->getIonTypePtrVec(),"PREC")->getShift(),0,0));
	list_filtered.push_back(PrmPeakPtr(new PrmPeak(prec_peak,prot::getPrmPeakTypePtrByName(spec_data->getPrmPeakTypePtrVec(),"ORIGINAL"),
			prec_mono_mass-prot::getIonTypePtrByName(base_data->getIonTypePtrVec(),"PREC")->getShift(),1)));
	std::sort(list_filtered.begin(),list_filtered.end(),prmpeak_up);

	//setTolerance
	list_filtered[0]->setStrictTolerance(sp_para->getPeakTolerance()->compStrictErrorTole(0));
	list_filtered[0]->setNStrictCRelacTolerance(sp_para->getPeakTolerance()->compStrictErrorTole(0));
	list_filtered[0]->setNRelaxCStrictTolerance(sp_para->getPeakTolerance()->compStrictErrorTole(prec_mono_mass));
	list_filtered[list_filtered.size()-1]->setStrictTolerance(sp_para->getPeakTolerance()->compStrictErrorTole(prec_mono_mass));
	list_filtered[list_filtered.size()-1]->setNStrictCRelacTolerance(sp_para->getPeakTolerance()->compStrictErrorTole(prec_mono_mass));
	list_filtered[list_filtered.size()-1]->setNRelaxCStrictTolerance(sp_para->getPeakTolerance()->compStrictErrorTole(0));
	for(unsigned int i=1;i< list_filtered.size()-1;i++){
		list_filtered[i]->setStrictTolerance(sp_para->getPeakTolerance()->compStrictErrorTole(list_filtered[i]->getBasePeak()->getMonoMass()));
		if(list_filtered[i]->getBaseType()==prot::getPrmPeakTypePtrByName(spec_data->getPrmPeakTypePtrVec(),"ORIGINAL")){
			list_filtered[i]->setNStrictCRelacTolerance(sp_para->getPeakTolerance()->compStrictErrorTole(list_filtered[i]->getBasePeak()->getMonoMass()));
			list_filtered[i]->setNRelaxCStrictTolerance(sp_para->getPeakTolerance()->compRelaxErrorTole(list_filtered[i]->getBasePeak()->getMonoMass(),prec_mono_mass));
		}
		else{
			//if the peak is M-m ,use base peak and parent mass to compute the error tolerance
			list_filtered[i]->setNRelaxCStrictTolerance(sp_para->getPeakTolerance()->compStrictErrorTole(list_filtered[i]->getBasePeak()->getMonoMass()));
			list_filtered[i]->setNStrictCRelacTolerance(sp_para->getPeakTolerance()->compRelaxErrorTole(list_filtered[i]->getBasePeak()->getMonoMass(),prec_mono_mass));
		}
	}
	//end settolerance

	return PrmPeakMS(new Ms<PrmPeakPtr>(header,list_filtered)) ;
}

PrmPeakMS getShiftSpSix(DeconvMsPtr deconv_ms,double delta,double shift,SpParaPtr sp_para){
	PrmPeakMS ms = getSpSix(deconv_ms,delta,sp_para);
	double mono_mz = (ms->getHeaderPtr()->getPrecMonoMass()+shift)/ms->getHeaderPtr()->getPrecCharge();
	ms->getHeaderPtr()->setPrecMonoMz(mono_mz);
	PrmPeakPtrVec prm_peaks ;
	for(unsigned int i=0;i< ms->size();i++){
		double pos= ms->getPeakPtr(i)->getPosition()+shift;
		if(pos>0){
			 ms->getPeakPtr(i)->setPosition(pos);
			 prm_peaks.push_back(ms->getPeakPtr(i));
		}
	}
	return PrmPeakMS(new Ms<PrmPeakPtr>(ms->getHeaderPtr(),prm_peaks));
}

std::vector<std::vector<int>> getIntMassErrorList(PrmPeakMS ms,double scale,bool n_strict,bool c_strict){
	std::vector<int> masses;
	std::vector<int> errors;
	int last_mass = -1;
	int last_error = 0;
	for(int i=0;i<ms->size();i++){
		int m = (int)std::round(ms->getPeakPtr(i)->getPosition()*scale);
		int e = 0;
		if(n_strict && c_strict){
			e = (int) std::ceil(ms->getPeakPtr(i)->getStrictTolerance()*scale);
		}
		else if(n_strict && !c_strict){
			e = (int) std::ceil(ms->getPeakPtr(i)->getNStrictCRelacTolerance()*scale);
		}
		else if(!n_strict && c_strict){
			e = (int) std::ceil(ms->getPeakPtr(i)->getNRelaxCStrictTolerance()*scale);
		}
		if(m != last_mass){
			masses.push_back(m);
			errors.push_back(e);
			last_mass = m;
			last_error = e;
		}
		else if(e>last_error){
			errors.pop_back();
			errors.push_back(e);
			last_error = e;
		}
	}
	std::vector<int> mass_temp;
	std::vector<int> error_temp;
	std::vector<std::vector<int>> results;
	for(unsigned int i=0;i<masses.size();i++){
		mass_temp.push_back(masses[i]);
		error_temp.push_back(errors[i]);
	}
	results.push_back(mass_temp);
	results.push_back(error_temp);
	return results;
}
std::vector<double> getMassList(PrmPeakMS ms){
	std::vector<double> results;
	for(int i=0;i<ms->size();i++){
		results.push_back(ms->getPeakPtr(i)->getPosition());
	}
	return results;
}
std::vector<double> getScoreList(PrmPeakMS ms){
	std::vector<double> results;
	for(int i=0;i<ms->size();i++){
		results.push_back(ms->getPeakPtr(i)->getScr());
	}
	return results;
}
} /* namespace prot */
