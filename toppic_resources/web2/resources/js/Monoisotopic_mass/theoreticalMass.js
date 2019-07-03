var peakCalData = function(){
	let prsm = prsm_data.prsm;
	this.massList = [] ;
	this.monoMassList = [];
	this.thresholdVal = 1 ;
	prsm.ms.peaks.peak.forEach(function(peak,i){
		let tempPeak = {};
		tempPeak.peak_id = peak.peak_id ;
		tempPeak.monoisotopic_mass = peak.monoisotopic_mass;
		this.monoMassList.push(tempPeak);
	})
	let shift = 0 ;
	this.thMassList = theoreticalMass(prsm,massList);
	this.prefixMassList = getPrefixMass(prsm,massList,shift);
	this.suffixMassList = getSuffixMass(prsm,massList,shift);
	this.completeCalData = getCalData(this.thMassList,this.monoMassList,this.thresholdVal);
	
	console.log("thMassList : ",thMassList);
	console.log("prefixMassList : ", prefixMassList);
	console.log("suffixMassList : ",suffixMassList);
} 
function theoreticalMass(prsm,massList,shift)
{
	let first_residue_position = prsm.annotated_protein.annotation.first_residue_position ;
	let last_residue_position = prsm.annotated_protein.annotation.last_residue_position ;
	let acidList =[] ;
	let zero = 0;
	prsm.annotated_protein.annotation.residue.forEach(function(residue,i){
		if(parseInt(residue.position) >= parseInt(first_residue_position) && parseInt(residue.position) <= parseInt(last_residue_position))
		{
			massList.push(zero);
			acidList.push(residue);
		}
	})
	return getCalculatedMass(acidList,massList,shift,first_residue_position);
}
this.getCalculatedMass = function(acidList, massList,shift,first_residue_position){
	let tMassList = [];
	console.log("acidList",acidList);
	console.log("massList",massList);
	acidList.forEach(function(acidList,i){
		console.log("i : ",i);
		let tempTMass = {} ;
		tempTMass.position = parseInt(acidList.position) ;
		tempTMass.mass = massAtPosition(tMassList,acidList,massList[i],first_residue_position);
		tMassList.push(tempTMass);
	})
	return tMassList ;
}
this.massAtPosition = function(tMassList,acidResidue,mass,first_residue_position){
	let acidMass = parseFloat(AminoAcid.getAcidMass(acidResidue.acid).mono);
	console.log("acidResidue : ", acidResidue);
	if(parseInt(acidResidue.position) == parseInt(first_residue_position))
	{
		return parseFloat(acidMass)+mass ;
	}
	else
	{
		/*Mass of previous position + new mass of acid*/
		return (tMassList[parseInt(acidResidue.position)-parseInt(first_residue_position)-1].mass+acidMass+mass) ;
	}
}
function getPrefixMass(prsm,massList,shift){
	shift = parseInt(shift) ;
	let prefixMassList =  theoreticalMass(prsm,massList,shift);
	return prefixMassList ;
}
function getSuffixMass(prsm,massList,shift){
	let suffixMassList = [] ;
	let residueList =  prsm.annotated_protein.annotation.residue ;
	let i = residueList.length ;
	let k = 0 ;
	while(i--){
		let suffixmass = {} ;
		let acidMass = AminoAcid.getAcidMass(residueList[i].acid).mono ;
		suffixmass.position = residueList[i].position ;
		
		if(i == residueList.length-1)
		{
			suffixmass.mass = massList[i] + acidMass;
		}
		else
		{
			suffixmass.mass = suffixMassList[k-1].mass + massList[i] + acidMass ;
		}
		suffixMassList.push(suffixmass);
		k++;
	}
	return suffixMassList ;
}
function getCalData(thMassList,monoMassList,thresholdVal){
	console.log("monoMassList : ",monoMassList);
	let calculatedDataList = [] ;
	for(let i = 0; i < monoMassList.length; i++)
	{
		for(let j=0; j<thMassList.length ; j++)
		{
			if((monoMassList[i].monoisotopic_mass - thMassList[j].mass) < thresholdVal && (monoMassList[i].monoisotopic_mass - thMassList[j].mass) > 0-thresholdVal)
			{
				let calculatedData = {} ;
				calculatedData.peak_id = monoMassList[i].peak_id ;
				calculatedData.massError = thMassList[j].mass - monoMassList[i].monoisotopic_mass;
				calculatedData.thMass = thMassList[j].mass ;
				calculatedData.position = thMassList[j].position ;
				calculatedData.monoisotopic_mass = monoMassList[i].monoisotopic_mass ;
				calculatedDataList.push(calculatedData);
			}
		}
	}
	console.log("calculatedDataList : ", calculatedDataList);
}