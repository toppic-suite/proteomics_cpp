function proteoform_Url()
{
	l_proteoform_Url = prsm_data.compatible_proteoform.sequence_name+" " +prsm_data.compatible_proteoform.sequence_description;
	document.title = "Proteoform #"+prsm_data.compatible_proteoform.proteoform_id+" from " + l_proteoform_Url ;	
	document.getElementsByName("protein_url")[0].innerHTML = l_proteoform_Url;
	document.getElementsByName("protein_url")[0].href = "protein.html?protein_Id="+prsm_data.compatible_proteoform.sequence_id;
	document.getElementsByName("protein_url")[1].innerHTML = l_proteoform_Url;
	document.getElementsByName("protein_url")[1].href = "protein.html?protein_Id="+prsm_data.compatible_proteoform.sequence_id;
	//Determinign the header of the HTML file
	document.getElementById("proteoform_header").innerHTML ="Proteoform #"+prsm_data.compatible_proteoform.proteoform_id;
	
	if(Array.isArray(prsm_data.compatible_proteoform.prsm))
	{
		document.getElementById("prsm_count").innerHTML =prsm_data.compatible_proteoform.prsm.length+" PrSMs for this proteoform";
	}
	else
	{
		document.getElementById("prsm_count").innerHTML ="1 PrSM for this proteoform";
	}
}
function createTable_data(){
	
	var table = document.getElementById('proteoform_data');
	var tbdy = document.createElement('tbody');
	let count = 0;
	let sequence_name = prsm_data.compatible_proteoform.sequence_name ;
	
	if(Array.isArray(prsm_data.compatible_proteoform.prsm))
	{
		prsm_data.compatible_proteoform.prsm.forEach(function(prsm,i){
			let All_Peak_count = prsm.ms.peaks.peak.length;
			let tr = document.createElement('tr');
			for(let i=0;i<7;i++){
				var td = document.createElement('td');
				td.setAttribute("align","center");
				if(i === 0){
					td.innerHTML = prsm.ms.ms_header.scans ;
				}
				if(i === 1){
					td.innerHTML = sequence_name ;
				}
				if(i === 2){
					td.innerHTML = prsm.e_value ;
				}
				if(i === 3){
					td.innerHTML = All_Peak_count ;
				}
				if(i === 4){
					td.innerHTML = prsm.matched_peak_number;
				}
				if(i === 5){
					td.innerHTML = prsm.matched_fragment_number;
				}
				if(i === 6){
					let a = document.createElement('a')
					l_onfocus = "cur = "+count +";selectRow();" ;
					// unable to identify the link logic	
					l_href = "prsm.html?prsm_id="+prsm.prsm_id;
					l_link = "link" + (count +1) ;
					a.setAttribute("onfocus",l_onfocus);
					a.setAttribute("href", l_href);
					a.setAttribute("id", l_link);
					a.innerHTML = "See PrSM&gt;&gt;";
					td.appendChild(a);
					count ++ ;
				}
				tr.appendChild(td);
			}
			tbdy.appendChild(tr);
		})
		
	}
	else
	{
		prsm = prsm_data.compatible_proteoform.prsm ;

		let All_Peak_count = prsm.ms.peaks.peak.length;
		let tr = document.createElement('tr');
		for(let i=0;i<7;i++){
			var td = document.createElement('td');
			td.setAttribute("align","center");
			if(i === 0){
				td.innerHTML = prsm.ms.ms_header.scans ;
			}
			if(i === 1){
				td.innerHTML = sequence_name ;
			}
			if(i === 2){
				td.innerHTML = prsm.e_value ;
			}
			if(i === 3){
				td.innerHTML = All_Peak_count ;
			}
			if(i === 4){
				td.innerHTML = prsm.matched_peak_number;
			}
			if(i === 5){
				td.innerHTML = prsm.matched_fragment_number;
			}
			if(i === 6){
				let a = document.createElement('a')
				l_onfocus = "cur = "+count +";selectRow();" ;
				// unable to identify the link logic	
				l_href = "prsm.html?prsm_id="+prsm.prsm_id;
				l_link = "link" + (count +1) ;
				a.setAttribute("onfocus",l_onfocus);
				a.setAttribute("href", l_href);
				a.setAttribute("id", l_link);
				a.innerHTML = "See PrSM&gt;&gt;";
				td.appendChild(a);
				count ++ ;
			}
			tr.appendChild(td);
		}
		tbdy.appendChild(tr);
		
	}
	
	
	
	table.appendChild(tbdy);
}