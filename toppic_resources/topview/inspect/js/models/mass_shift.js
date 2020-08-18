class MassShifts {
	constructor(){
	}
	/**
	 * @param {string} seq - an argument with mass shift changes embeded in [] square bracket.
	 * @return {string} parsedseq - sequence after removing the mass
	 * Shifts. 
	 * @returns {Array} massShiftList - Array with {position,mass} position-position at which 
	 * mass shift occured, mass- mass shift value.
	 */
	getMassShiftList(seq){
		let massShiftList = [] ;
		let parsedseq = "";
		let splitStr = seq.split(/\[(.*?)\]/);
		let splitArraylen = splitStr.length;
		let position = 0 ;
		
		for(let i = 0 ; i<splitArraylen;i++)
		{
			if(isNaN(splitStr[i]))
			{
				parsedseq = parsedseq + splitStr[i] ;
				position = position + splitStr[i].length ;
			}
			else
			{
				let mass = parseFloat(splitStr[i]);
				/**
				 * remove 1 as the data starts from 0 and length starts from 1
				 */
				let tempPosition = position - 1;
				//Initially set the bg_color to null
				let shiftobj = {mass:mass,position:tempPosition,bg_color:null};
				/**
				 * when the split occur at the end we get an extra "" in 
				 * the list. This is to check if the mass is numeric.
				 */
				if(!isNaN(mass))
				{
					massShiftList.push(shiftobj);
				}
			}
		}
		return [parsedseq,massShiftList] ;
	}
	/**
	 * @param {integer} shiftPosition - contains the position of the new mass
	 * shift entered in text box on click of any amino acid.
	 * @param {integer} massShiftVal - contains value of the mass entered.
	 * @param {Array} massShiftList - contains list of all existing mass 
	 * shifts with positions.
	 * @return {Array} with the new mass Shift value and position entered
	 * or changes the existing mass shift value.
	 */
	appendtoMassShiftList(shiftPosition,massShiftVal,massShiftList,bg_color){
		let newMassShiftList = [];
		let len = massShiftList.length;
		let matchFound = false ;
		for(let i=0; i<len;i++)
		{
			if(shiftPosition == massShiftList[i].position)
			{
				massShiftList[i].mass = massShiftVal;
				massShiftList[i].bg_color = bg_color;
				matchFound = true ;
			}
		}
		if(!matchFound)
		{
			let tempShiftObj = {mass:massShiftVal,position:shiftPosition,bg_color:bg_color};
			massShiftList.push(tempShiftObj);
			matchFound = false;
		}
		let newlen = massShiftList.length;
		for(let j=0; j<newlen; j++)
		{
			if(massShiftList[j].mass != 0)
			{
				newMassShiftList.push(massShiftList[j]);
			}
		}
		return newMassShiftList ;
	}
	/**
	 * Get the fixed ptm mass list with position
	 * @param {string} seq - plain sequence entered by the user.
	 * @param {Array} selectedFixedMassShiftList - an array which contains fixed mass shift selected by users, EX. [{acid: X, mass: 12}]
	 * @return {Array} - Returns an Array of fixed mass shift with positions.
	 */
	getFixedMassList(seq, selectedFixedMassShiftList){
		let fixedShiftList = [];
		let fixedPtmAcid = null;
		let fixedPtmMass = null;
		// let selectedFixedMassShiftList = getFixedPtmCheckList();
		for(let k=0; k<selectedFixedMassShiftList.length; k++)
		{
			fixedPtmAcid = selectedFixedMassShiftList[k].acid;
			fixedPtmMass = selectedFixedMassShiftList[k].mass;
			for(let i = 0 ; i<seq.length;i++)
			{
				if(seq[i] === fixedPtmAcid)
				{
					let tempObj = {position:i, mass:fixedPtmMass, bg_color:null}
					fixedShiftList.push(tempObj);
				}
			}
		}
		return fixedShiftList;
	}
	/**
	 * Remove acid mass shift from fixed mass list
	 * @param {string} sequence - sequence without mass shifts
	 * @param {Array} fixedMassShiftList - Fixed mass shift list
	 * @param {Char} removeAcid - mass to be removed of a specific Acid
	 */
	removeFixedMassList(sequence,fixedMassShiftList,removeAcid)
	{
		let result  = [];
		removeAcid = removeAcid.toUpperCase()
		for(let i=0;i<fixedMassShiftList.length;i++)
		{
			let pos = fixedMassShiftList[i].position;
			if(sequence[pos] !== removeAcid)
			{
				result.push(fixedMassShiftList[i]);
			}
		}
		return result ;
	}
	/**
	 * This returns combined List of both Fixed and user entered mass shifts
	 * @param {Array} massShiftList - List of all the mass Shifts from sequence
	 * @param {Array} fixedMassShiftList - List of all selected fixed masses
	 * @returns {Array} combinedMassShiftList - List of combined lists by checking 
	 * over lapping posiitons
	 */
	getCombinedMassShiftList(massShiftList,fixedMassShiftList){
		let combinedMassShiftList = massShiftList;
		let fixedMasslen = fixedMassShiftList.length;
		let massShiftlen = massShiftList.length;
		
		for(let i=0; i<fixedMasslen ; i++)
		{
			let matched = false;
			for(let j=0;j<massShiftlen;j++)
			{
				/**
				 * Check if both has mass shift at common position and over ride the mass shift with fixed mass shift
				 */
				if(combinedMassShiftList[j].position == fixedMassShiftList[i].position)
				{
					combinedMassShiftList[j].mass = fixedMassShiftList[i].mass ;
					combinedMassShiftList[j].bg_color = fixedMassShiftList[i].bg_color ;
					matched = true;
					break;
				}
			}
			/**
			 * If no match found then copy to the new combined list
			 */
			if(!matched)
			{
				combinedMassShiftList.push(fixedMassShiftList[i]);
			}
		}
		
		return combinedMassShiftList ;
	}
	/**
	 * forms the seq with all the mass lists and selected fixed ptms
	 * @param {string} seq - sequence with only aminoacids and without mass lists embedded
	 * @param {Array} massShiftList - List with all the combined mass shifts
	 * @return {string} newSeq - sequence with mass shifts embedded in []
	 */
	formSequence(seq,massShiftList){
		let newSeq = seq;
		let len = massShiftList.length; 
		let seqLen = seq.length ;
		let count = 0;
		/**
		 * sorting the lists with position
		 */
		massShiftList.sort(function(x,y){
            return x.position - y.position;
			// return d3.ascending(x.position, y.position);
		})
		for(let i=0;i<len;i++)
		{
			let newSeqlen = newSeq.length ;
			/**
			 * Dont show when the mass is 0 in the string
			 */
			if(massShiftList[i].mass !== 0){
				if(i === 0)
				{
					/**
					 * Add +1 as we need to append the mass after the current position
					 */
					let tempPosition = massShiftList[i].position+1 ;
					newSeq = newSeq.slice(0, tempPosition) + "["+ massShiftList[i].mass + "]"+newSeq.slice(tempPosition, newSeqlen);
				}
				else
				{
					/**
					 * Form the mass between []
					 */
					let tempString = "["+massShiftList[i-1].mass+"]";
					count = count + tempString.length;
					/**
					 * add +1 as the position need to be added after 
					 * the position of the acid.
					 */
					let tempPosition = massShiftList[i].position + 1 + count ;
					newSeq = newSeq.slice(0, tempPosition) + "["+ massShiftList[i].mass + "]" + newSeq.slice(tempPosition, newSeqlen);
				}
			}
		}
		return newSeq ;
	}
}