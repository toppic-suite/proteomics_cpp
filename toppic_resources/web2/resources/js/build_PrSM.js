function BuildUrl()
{
	console.log("PRSM : ", prsm_data);
	document.title = "Protein-Spectrum-Match for Spectrum #"+prsm_data.prsm.ms.ms_header.ids;
	let l_protein_URL = prsm_data.prsm.annotated_protein.sequence_name + " " + prsm_data.prsm.annotated_protein.sequence_description;
	document.getElementById("protien_url").innerHTML = l_protein_URL;
	document.getElementById("protien_url").href = "protein.html?protein_Id="+prsm_data.prsm.annotated_protein.sequence_id;
	document.getElementById("protien_url_end").innerHTML = l_protein_URL;
	document.getElementById("protien_url_end").href = "protein.html?protein_Id="+prsm_data.prsm.annotated_protein.sequence_id;
	let l_protroform_URL = "Proteoform #"+prsm_data.prsm.annotated_protein.proteoform_id ;
	document.getElementById("proteoform_url").innerHTML = l_protroform_URL;
	document.getElementById("proteoform_url").href = "proteoform.html?proteoform_Id="+prsm_data.prsm.annotated_protein.proteoform_id;
	
	document.getElementById("proteoform_url_end").innerHTML = l_protroform_URL;
	document.getElementById("proteoform_url_end").href ="proteoform.html?proteoform_Id="+prsm_data.prsm.annotated_protein.proteoform_id;
	
	document.getElementById("Protein-Spectrum-Match-Id-SpecId").innerHTML ="Protein-Spectrum-Match #"+prsm_data.prsm.prsm_id+" for Spectrum #" + prsm_data.prsm.ms.ms_header.ids;
}

function loadDatafromJson2Html(){
	//Writing data to the first table from json file
	document.getElementById("File_name").innerHTML = prsm_data.prsm.ms.ms_header.spectrum_file_name;
	document.getElementById("PrSM_ID").innerHTML = prsm_data.prsm.prsm_id;
	document.getElementById("Scan").innerHTML = prsm_data.prsm.ms.ms_header.scans;
	document.getElementById("Precursor_charge").innerHTML = prsm_data.prsm.ms.ms_header.precursor_charge;
	document.getElementById("Precursor_m/z").innerHTML = prsm_data.prsm.ms.ms_header.precursor_mz;
	document.getElementById("Precursor_mass").innerHTML = prsm_data.prsm.ms.ms_header.precursor_mono_mass;
	document.getElementById("Proteoform_mass").innerHTML = prsm_data.prsm.annotated_protein.proteoform_mass;
	document.getElementById("matched_peaks").innerHTML = prsm_data.prsm.matched_peak_number;
	document.getElementById("matched_fragment_ions").innerHTML = prsm_data.prsm.matched_fragment_number;
	document.getElementById("unexpected_modifications").innerHTML = prsm_data.prsm.annotated_protein.unexpected_shift_number;
	document.getElementById("E_value").innerHTML = prsm_data.prsm.e_value;
	document.getElementById("P_value").innerHTML = prsm_data.prsm.p_value;
	document.getElementById("Q_value").innerHTML = prsm_data.prsm.fdr;
}


//Code to read the json file and draw the annotations between sequence
function json2annotationpositions()
{
	let matched_ion = [] ;
//	let matched_ion_temp = [] ;
	prsm_data.prsm.annotated_protein.annotation.cleavage.forEach(function(input,i){
		if(input.matched_peaks != null)
		{
			let matched_ion_temp_array = {} ;
			
			if(input.matched_peaks.matched_peak.length > 1)
			{
				input.matched_peaks.matched_peak.forEach(function(l_input,i){
					
					matched_ion_temp_array["position"] = input.position ;
					// Ion Type => "Y"/"B"
					matched_ion_temp_array["ion_type"] = l_input.ion_type ;
					// Ion Display position
					matched_ion_temp_array["ion_display_position"] = l_input.ion_display_position ;
					// Ion Charge
					matched_ion_temp_array["peak_charge"] = l_input.peak_charge ;
					// ion_position
					matched_ion_temp_array["ion_position"] = l_input.ion_position ;
	
					matched_ion.push(matched_ion_temp_array) ;
					matched_ion_temp_array = {} ;
				})
			}
			else
			{
				matched_ion_temp_array["position"] = input.position ;
				// Ion Type => "Y"/"B"
				matched_ion_temp_array["ion_type"] = input.matched_peaks.matched_peak.ion_type ;
				// Ion Display position
				matched_ion_temp_array["ion_display_position"] = input.matched_peaks.matched_peak.ion_display_position ;
				// Ion Charge
				matched_ion_temp_array["peak_charge"] = input.matched_peaks.matched_peak.peak_charge ;
				// ion_position
				matched_ion_temp_array["ion_position"] = input.matched_peaks.matched_peak.ion_position ;
				
				matched_ion.push(matched_ion_temp_array) ;
				matched_ion_temp_array = {} ;
			}
		}
	})
	return matched_ion ;
}

function json2DataString(){
	let known_Change = [] ;
	let l_dataString = "" ;
	prsm_data.prsm.annotated_protein.annotation.residue.forEach(function(residue, i){
		l_dataString = l_dataString + residue.acid;
	})	
	
	if(prsm_data.prsm.annotated_protein.annotation.hasOwnProperty("ptm") )
	{
		if(Array.isArray(prsm_data.prsm.annotated_protein.annotation.ptm))
		{
			prsm_data.prsm.annotated_protein.annotation.ptm.forEach(function(ptm,index){
				
				if(ptm.hasOwnProperty("occurence"))
				{
					if(Array.isArray(ptm.occurence))
					{
						ptm.occurence.forEach(function(occurence,i){
							known_Change.push(occurence.left_pos);
						});
					}
					else
					{
						known_Change.push(ptm.occurence.left_pos);
					}
				}
				
			})
		}
		else
		{
			if(prsm_data.prsm.annotated_protein.annotation.ptm.hasOwnProperty("occurence"))
			{
				if(Array.isArray(prsm_data.prsm.annotated_protein.annotation.ptm.occurence))
				{
					prsm_data.prsm.annotated_protein.annotation.ptm.occurence.forEach(function(occurence,i){
						known_Change.push(occurence.left_pos);
					});
				}
				else
				{
					known_Change.push(prsm_data.prsm.annotated_protein.annotation.ptm.occurence.left_pos);
				}
			}
		}
		
		
	}
return [l_dataString,known_Change] ;
}

function json2Background_color()
{
	let background_array = [];
	
	if(prsm_data.prsm.annotated_protein.annotation.hasOwnProperty('mass_shift'))
	{
		if(Array.isArray(prsm_data.prsm.annotated_protein.annotation.mass_shift)){
			prsm_data.prsm.annotated_protein.annotation.mass_shift.forEach(function(mass_shift,i){
				
				if(mass_shift.right_position != "0")
				{
					let background_array_temp = unexpected_change(mass_shift);
					background_array.push(background_array_temp)
				}
				
			})
		}
		else
		{
			i_mass_shift = prsm_data.prsm.annotated_protein.annotation.mass_shift ;
			
			if(i_mass_shift.right_position != "0")
			{
				
				let background_array_temp = unexpected_change(i_mass_shift);
				
				background_array.push(background_array_temp) ;
				
				console.log("background_array _1 : ", background_array);
			}
			
		}
	}
	console.log("background_array :  ", background_array);
	return background_array ;
}
function createTableElements(){
	var table = document.getElementById('spectrum');
	var tbdy = document.createElement('tbody');
	
	l_scans = prsm_data.prsm.ms.ms_header.scans ;
	
	let l_even = 0 ;
	let l_matched_peak_count = 0;
	
	prsm_data.prsm.ms.peaks.peak.forEach(function(peak,i){
		
		if(peak.hasOwnProperty('matched_ions_num') && parseInt(peak.matched_ions_num)>1)
		{
			peak.matched_ions.matched_ion.forEach(function(matched_ion,i){
				peak.matched_ions.matched_ion = matched_ion ;
					loop_matched_ions(peak,i) ;
					
			})
		}
		else
		{
			loop_matched_ions(peak,i) ;
		}
		
		
	})
	
	function loop_matched_ions(peak,i){

		//Creating tr element under table for every peak
		var tr = document.createElement('tr');
		id = peak.spec_id+"peak"+peak.peak_id;
		if((parseInt(peak.peak_id) + 1)%2 == 0)
		{
			l_class = "unmatched_peak even";
		}
		else
		{
			l_class = "unmatched_peak odd";
		}
		
		if(peak.hasOwnProperty('matched_ions_num'))
		{
			id = id + peak.matched_ions.matched_ion.ion_type;
			if((parseInt(peak.peak_id) + 1)%2 == 0)
			{
				l_class = "matched_peak even";
			}
			else
			{
				l_class = "matched_peak odd";
			}
			l_matched_peak_count++;
			l_name = peak.matched_ions.matched_ion.ion_position;
			tr.setAttribute("name",l_name);
		}
		tr.setAttribute("id", id);
		tr.setAttribute("class",l_class);
		tr.setAttribute("role","row");
		//Creating td element for every tr
		for(let i = 0;i<11;i++){
			var td = document.createElement('td');
			td.setAttribute("align","center");
			if(i == 0)
			{
				td.innerHTML = l_scans ;
			}
			if(i == 1)
			{
				td.innerHTML = parseInt(peak.peak_id) + 1 ;
			}
			if(i == 2)
			{
				td.innerHTML = peak.monoisotopic_mass;
			}
			if(i == 3)
			{
				td.innerHTML = peak.monoisotopic_mz;
			}
			if(i == 4)
			{
				td.innerHTML = peak.intensity;
			}
			if(i == 5)
			{
				td.innerHTML = peak.charge;
			}
			if(peak.hasOwnProperty('matched_ions_num'))
			{
				if(i == 6)
				{
					td.innerHTML = peak.matched_ions.matched_ion.theoretical_mass;
				}
				if(i == 7)
				{
					td.innerHTML = peak.matched_ions.matched_ion.ion_type+peak.matched_ions.matched_ion.ion_display_position;
				}
				if(i == 8)
				{
					td.innerHTML = peak.matched_ions.matched_ion.ion_position;
				}
				if(i == 9)
				{
					td.innerHTML = peak.matched_ions.matched_ion.mass_error;
				}
				if(i == 10)
				{
					td.innerHTML = peak.matched_ions.matched_ion.ppm;
				}
			}
			tr.appendChild(td)
		}
	  tbdy.appendChild(tr);
		
	}
	l_even = l_even + 1;
	let l_All_Peaks = prsm_data.prsm.ms.peaks.peak.length;
	let l_not_matched_peak_count = l_All_Peaks - l_matched_peak_count; 
	document.getElementById("all_peak_count").innerHTML = "All peaks (" + l_All_Peaks + ")" ;
	document.getElementById("matched_peak_count").innerHTML = "Matched peaks (" + l_matched_peak_count + ")" ;
	document.getElementById("not_matched_peak_count").innerHTML = "Not Matched peaks (" + l_not_matched_peak_count + ")" ;
	
	table.appendChild(tbdy);
}
function unexpected_change(l_unexpected_change){
	
	let background_array_temp = {} ;
	
	background_array_temp["left_position"] = l_unexpected_change.left_position;
	background_array_temp["right_position"] = l_unexpected_change.right_position ;
	background_array_temp["color"] = "#64E9EC";
	background_array_temp["anno"] = l_unexpected_change.anno ;
	return background_array_temp ;
}

function occurence_PTM()
{
	if(prsm_data.prsm.annotated_protein.annotation.hasOwnProperty('ptm'))
	{
		let ptm = prsm_data.prsm.annotated_protein.annotation.ptm ;
		let occurence_ptms = "";
		let l_array_ptm = {} ;
		let l_list_ptm = [];
		
		if(Array.isArray(ptm))
		{
			ptm.forEach(function(ptm, index){
				ptm_loop(ptm);
			})
		}
		else
		{
			ptm_loop(ptm);
		}
		
		function ptm_loop(ptm)
		{
			if(ptm.ptm_type == "Fixed")
			{
				l_array_ptm = {} ;
				if(Array.isArray(ptm.occurence))
				{
					ptm.occurence.forEach(function(occurence, inner_index){
						
						occurence_ptms = occurence_ptms + occurence.anno + (parseInt(occurence.left_pos)) + " " ;
					})
				}
				else if((ptm.occurence != null))
				{
					occurence_ptms = ptm.occurence.anno + 
												(parseInt(ptm.occurence.left_pos)) + " ";
				}
				l_array_ptm["ptm_type"] = "Fixed" ;
				l_array_ptm["abbreviation"] = ptm.ptm.abbreviation ;
				l_array_ptm["occurence"] = occurence_ptms.trim() ;
				l_list_ptm.push(l_array_ptm);
			}
			else if(ptm.ptm_type == "Protein variable")
			{
				l_array_ptm = {} ;
				if(Array.isArray(ptm.occurence))
				{
					ptm.occurence.forEach(function(occurence, inner_index){
						
						occurence_ptms = occurence_ptms + occurence.anno + (parseInt(occurence.left_pos)) + " " ;
					})
				}
				else if((ptm.occurence != null))
				{
					occurence_ptms = ptm.occurence.anno + 
												(parseInt(ptm.occurence.left_pos)) + " ";
				}
				l_array_ptm["ptm_type"] = "Protein variable" ;
				l_array_ptm["abbreviation"] = ptm.ptm.abbreviation ;
				l_array_ptm["occurence"] = occurence_ptms.trim() ;
				l_list_ptm.push(l_array_ptm);
			}
		}
		
		let div = document.getElementById("ptm_abbreviation") ;
		
		l_list_ptm.forEach(function(ptm_input,i){
			
			let text1 = document.createElement("text");
			let a = document.createElement("a");
			a.href = "http://www.unimod.org/modifications_view.php?editid1=-1";
			a.innerHTML = ptm_input.abbreviation +" [" + ptm_input.occurence + "]" ;
			a.target = "_blank";
			a.style = "color:red" ;
			
			if(ptm_input.ptm_type == "Fixed")
			{
				text1.innerHTML = "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"+"Fixed PTMs: ";
				//a.innerHTML = "Carbamidomethylation [" + occurence_ptms + "]" ;
			}
			if(ptm_input.ptm_type == "Protein variable")
			{
				text1.innerHTML = "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"+"Variable PTMs: ";
			}
			
			div.appendChild(text1);
			div.appendChild(a);
		})
		
	}

}

function get_Ion_First_last_position()
{
	let protein_length_array = {} ;
	
	protein_length_array["protein_length"] = parseInt(prsm_data.prsm.annotated_protein.annotation.protein_length) ;
	protein_length_array["first_residue_position"] = parseInt(prsm_data.prsm.annotated_protein.annotation.first_residue_position) ;
	protein_length_array["last_residue_position"] = parseInt(prsm_data.prsm.annotated_protein.annotation.last_residue_position) ;
	
	//console.log("protein_length_array : ", protein_length_array);
	
	return protein_length_array ;
	
}
