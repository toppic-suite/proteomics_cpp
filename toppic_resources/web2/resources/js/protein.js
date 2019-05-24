function build_protein(){
	//let protein = protein ;
	let residue_Data ={};
	let unexpected_change_list = [];
	let residue_sequence = "";
	
	console.log("protein : ", prsm_data);
	
	document.title = "proteoforms for protein " + prsm_data.protein.sequence_name + " "+ prsm_data.protein.sequence_description;
	document.getElementById('sequence_description').innerHTML = prsm_data.protein.compatible_proteoform_number+" proteoforms for protein "
																+ prsm_data.protein.sequence_name + " "+ prsm_data.protein.sequence_description; 
	
	if(Array.isArray(prsm_data.protein.compatible_proteoform))
	{
		prsm_data.protein.compatible_proteoform.forEach(function(compatible_proteoform,index){
			build_HTML(compatible_proteoform,index)
		})
	}
	else
	{
		build_HTML(prsm_data.protein.compatible_proteoform,0)
	}
	
}

function build_HTML(compatible_proteoform,index)
{
	var div_container = document.getElementsByClassName("container")[0];
	
	var div = document.createElement('div');
	let id = "p"+ compatible_proteoform.proteoform_id;
	div.setAttribute("id",id);
	var h2 = document.createElement('h2');
	let p = document.createElement("p");
	let e_value;
	let BestPrSM ;
	
	//Temporary Fix on precursor_inte
	
	
	if(compatible_proteoform.prsm.length > 0)
	{
		h2.innerHTML = "Proteoform #" + compatible_proteoform.proteoform_id + " Feature intensity: "
						+ compatible_proteoform.prsm[0].ms.ms_header.precursor_inte;
		
		let precursor_mass;
		let prsm_id ;
		[e_value,precursor_mass,prsm_id] = Best_PrSM(compatible_proteoform.prsm);
		
		p = Build_BestPrSM(e_value,precursor_mass,prsm_id,compatible_proteoform.proteoform_id, compatible_proteoform.prsm.length);
		
		for(let i = 0; i< compatible_proteoform.prsm.length ; i++)
		{
			if(prsm_id == compatible_proteoform.prsm[i].prsm_id)
			{
				
				BestPrSM = compatible_proteoform.prsm[i] ;
			}
		}
		
	}
	else
	{
		h2.innerHTML = "Proteoform #" + compatible_proteoform.proteoform_id + " Feature intensity: "
						+ compatible_proteoform.prsm.ms.ms_header.precursor_inte;
		p.setAttribute("style","font-size:16px;");
		let text1 = document.createElement("text");
		text1.innerHTML = "There is only ";
		p.appendChild(text1);
		let a_prsm = document.createElement("a");
		a_prsm.href = "prsm.html?prsm_id="+compatible_proteoform.prsm.prsm_id;
		a_prsm.innerHTML = " 1 PrSM ";
		p.appendChild(a_prsm);
		let text2 = document.createElement("text");
		text2.innerHTML = "with an E-value "+compatible_proteoform.prsm.e_value +" and a precursor mass "+
							compatible_proteoform.prsm.ms.ms_header.precursor_mono_mass +".";
		p.appendChild(text2);
		
		e_value = compatible_proteoform.prsm.e_value ;
		
		BestPrSM = compatible_proteoform.prsm ;
	}
	
	[residue_sequence,residue_Data] = residue_Data_n_Sequence(compatible_proteoform.prsm , e_value);
	
	div_container.appendChild(div);
	div_container.appendChild(h2);
	div_container.appendChild(p);
	
	let unexpected_change_list = unexpected_change(BestPrSM);
	
	let Svg_id = "svgContainer" + index;
	let svgContainer = document.createElement("div");
	svgContainer.setAttribute("id",Svg_id);
	svgContainer.setAttribute("class","svgContainer");
	div_container.appendChild(svgContainer);
	let SVG = protein_Sequence(residue_sequence,index, residue_Data, unexpected_change_list);
	//div_container.appendChild(SVG);

}

function Best_PrSM(prsm)
{
	let e_value = " " ;
	let precursor_mass = " " ;
	let prsm_id = "";
	let temp = 0;
	temp = parseFloat(prsm[0].e_value);
	
	for(let i = 1 ; i < (prsm.length) ; i++)
	{
		if(temp < parseFloat(prsm[i].e_value))
		{
			temp = temp ;
			if(i == 1)
			{
				e_value = prsm[0].e_value;
				precursor_mass = prsm[0].ms.ms_header.precursor_mono_mass;
				prsm_id = prsm[0].prsm_id;
			}
		}
		else
		{
			temp = parseFloat(prsm[i].e_value)
			e_value = prsm[i].e_value;
			precursor_mass = prsm[i].ms.ms_header.precursor_mono_mass;
			prsm_id = prsm[i].prsm_id;
		}
	}
	return [e_value,precursor_mass,prsm_id];
}

function Build_BestPrSM(e_value,precursor_mass,prsm_id,proteoform_id, PrSM_Count){
	
	let p = document.createElement("p");
	p.setAttribute("style","font-size:16px;");
	
	let text1 = document.createElement("text");
	text1.innerHTML = "The ";
	p.appendChild(text1);
	let a_prsm = document.createElement("a");
	a_prsm.href = "prsm.html?prsm_id="+prsm_id;
	a_prsm.innerHTML = " best PrSM ";
	p.appendChild(a_prsm);
	
	let text2 = document.createElement("text");
	text2.innerHTML = "has an E-value "+e_value+" and a precursor mass "+precursor_mass+". There are ";
	p.appendChild(text2);
	let a_proteoform = document.createElement("a");
	a_proteoform.href = "proteoform.html?proteoform_id=" + proteoform_id;
	a_proteoform.innerHTML = PrSM_Count + " PrSMs ";
	p.appendChild(a_proteoform);
	
	let text3 = document.createElement("text");
	text3.innerHTML = "in total.";
	p.appendChild(text3);
	
	return p ;
}

function residue_Data_n_Sequence(prsm,e_value)
{
	//This function return Protein Sequence in the attribute residue_Sequence and also returns residue_Data which
	//looks as residue_Data : {"first_residue_position":1,"last_residue_position":91,"protein_length":92,"residue_type":{}}
	console.log("prsm : ",prsm);
	let residue_Data = {}; // gives a json formatted data back
	let g_prsm = prsm;
	let residue_Sequence = "";
	let known_residue_change = {}; // contain known_change residue position and color 
	let known_change = []; // List to contain all the know_change residues
	
	if(prsm.length > 0)
	{
		for(let i = 0; i<prsm.length;i++)
		{
			if(e_value === prsm[i].e_value)
			{
				g_prsm = prsm[i];
				residue_Data["first_residue_position"]  = parseInt(prsm[i].annotated_protein.annotation.first_residue_position);
				residue_Data["last_residue_position"]  = parseInt(prsm[i].annotated_protein.annotation.last_residue_position);
				residue_Data["protein_length"] = parseInt(prsm[i].annotated_protein.annotation.protein_length) ;
				break;
			}
		}
	}
	else
	{
		residue_Data["first_residue_position"] = parseInt(prsm.annotated_protein.annotation.first_residue_position);
		residue_Data["last_residue_position"]  = parseInt(prsm.annotated_protein.annotation.last_residue_position);
		residue_Data["protein_length"] = parseInt(prsm.annotated_protein.annotation.protein_length) ;
	}
	
	g_prsm.annotated_protein.annotation.residue.forEach(function(residue,index){
		
		residue_Sequence = residue_Sequence + residue.acid;
	})
	
	if(g_prsm.annotated_protein.annotation.hasOwnProperty("ptm") )
	{
		if(g_prsm.annotated_protein.annotation.ptm.hasOwnProperty("occurence"))
		{
			if(Array.isArray(g_prsm.annotated_protein.annotation.ptm.occurence))
			{
				g_prsm.annotated_protein.annotation.ptm.occurence.forEach(function(occurence,index){
					
					known_residue_change["residue_position"] = parseInt(occurence.left_pos);
					known_residue_change["color"] = "red";
					known_change.push(known_residue_change);
					
				})
			}
			else
			{
					known_residue_change["residue_position"] = parseInt(g_prsm.annotated_protein.annotation.ptm.occurence.left_pos);
					known_residue_change["color"] = "red";
					known_change.push(known_residue_change);
			}
		}
	}
	
	residue_Data["known_change"] = known_change ;
	return [residue_Sequence, residue_Data] ;
}
	/*
	 * Get the data from unexpected_change attribute which returned data in the format ----------------------------------------
	 * unexpected_change_list: [{match_seq: "14.95533", first_position: "13", color:"red",last_position: "43"},];--------------
	*/
function unexpected_change(prsm){
	
	let l_unexpected_change_list = [];
	let mass_shift = {} ;
	
	/*
	 * Data can be too arbitrary, the unexpected_change can be an array or not and we are returning list 
	 * to be in sync with protein_residue.js and can be used in future if the list has multiple data
	 */
	if(prsm.annotated_protein.annotation.hasOwnProperty('mass_shift'))
	{
		if(Array.isArray(prsm.annotated_protein.annotation.mass_shift))
		{
			prsm.annotated_protein.annotation.mass_shift.forEach(function(mass_shift,index){
				
				mass_shift["first_position"] = mass_shift.left_position ;// here the left_position and right_position are clevage positions which are between residue acids
				mass_shift["last_position"] = mass_shift.right_position ;
				mass_shift["color"] = "#64E9EC";
				mass_shift["anno"] = mass_shift.anno ;
					
					l_unexpected_change_list.push(mass_shift);
			})
		}
		else
		{
			let mass_shift = prsm.annotated_protein.annotation.mass_shift ;
			
			mass_shift["first_position"] = mass_shift.left_position ;
			mass_shift["last_position"] = mass_shift.right_position ;
			mass_shift["color"] = "#64E9EC";
			mass_shift["anno"] = mass_shift.anno;
			
			l_unexpected_change_list.push(mass_shift);
		}
		
	}
	console.log("l_unexpected_change_list : ", l_unexpected_change_list);
	return l_unexpected_change_list;
}
