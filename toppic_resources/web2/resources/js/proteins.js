function all_Proteins()
{
	//proteins data from data_protein.js file
	
	var l_proteins = prsm_data ;
	console.log("l_proteins : ", l_proteins);
	
	var count = l_proteins.protein_list.proteins.protein.length ;
	console.log("count : ", count);
	
	document.title = count + " proteins were identified" ;
	
	let div = document.getElementsByClassName("container")[0];
	
	let h2 = document.createElement('h2');
	h2.innerHTML = count + " proteins were identified." ;
	let br = document.createElement('br');
	
	div.appendChild(h2);
	div.appendChild(br);
	l_proteins.protein_list.proteins.protein.forEach(function(protein,index){
		let div_temp = build_AllProtein(protein);
		let p = best_PrSM(protein);
		let br1 = document.createElement('br');
		div_temp.appendChild(p);
		div_temp.appendChild(br1);
		div.appendChild(div_temp);
	})
	
}

function build_AllProtein(protein)
{
	let div  = document.createElement('div');
	let id = "p"+ protein.sequence_id ;
	div.setAttribute("id",id);
	
	let p = document.createElement('p');
	p.setAttribute("style", "font-size:16px;");
	
	let a  = document.createElement('a');
	//a.href = "proteins/protein"+ protein.sequence_id +".html";
	a.href = "protein"+".html" + "?protein="+ protein.sequence_id ;
	a.innerHTML = protein.sequence_name +" "+protein.sequence_description ;
	p.appendChild(a);
	div.appendChild(p);
	
	return div ;
}

function best_PrSM(protein)
{
	let best_e_value = 100;
	let prsm_id = "" ;
	let proteoform_count = protein.compatible_proteoform.length;
	if(proteoform_count > 1)
	{
		protein.compatible_proteoform.forEach(function(proteoform,index){
			
			[best_e_value,prsm_id] = proteoform_Multirow(proteoform,best_e_value,prsm_id);
			
		})
	}
	else
	{
		[best_e_value,prsm_id] = proteoform_Multirow(protein.compatible_proteoform,best_e_value,prsm_id);
	}
	
	
	let p = document.createElement('p');
	p.setAttribute("style", "font-size:16px;");
	
	let text1 = document.createElement("text");
	text1.innerHTML = "The ";
	p.appendChild(text1) ;
	let a  = document.createElement('a');
	a.href = "prsm.html?prsm_id="+prsm_id ;
	a.innerHTML = "best PrSM ";
	p.appendChild(a);
	
	let text2 = document.createElement("text");
	
	let val = "has an E-value "+best_e_value+". There";
	if(proteoform_count > 1)
	{
		val = val + " are "+proteoform_count + " proteoforms."
	}
	else
	{
		val = val + " is 1 proteoform."
	}
	text2.innerHTML = val ;
	p.appendChild(text2);
	
	return p;
}

function proteoform_Multirow(proteoform, best_e_value, prsm_id)
{
	let l_best_e_value = best_e_value ;
	let l_prsm_id = prsm_id ;
	if(proteoform.prsm.length > 1)
	{
		proteoform.prsm.forEach(function(prsm,index){
			if(parseFloat(l_best_e_value) < parseFloat(prsm.e_value))
			{
				l_best_e_value = l_best_e_value ;
				l_prsm_id = l_prsm_id ;
			}
			else
			{
				l_best_e_value = prsm.e_value ;
				l_prsm_id = prsm.prsm_id ;
			}
		})
	}
	else
	{
		if(parseFloat(l_best_e_value) < parseFloat(proteoform.prsm.e_value))
		{
			l_best_e_value = l_best_e_value ;
			l_prsm_id = l_prsm_id ;
		}
		else
		{
			l_best_e_value = proteoform.prsm.e_value ;
			l_prsm_id = proteoform.prsm.prsm_id ;
		}
	}
	
	return [l_best_e_value,l_prsm_id] ;
}

