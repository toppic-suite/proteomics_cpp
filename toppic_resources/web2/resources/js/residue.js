
function build_svg(id,width,height){
	
	let sentence = "";
	let known_Change = [];
	console.log("checking id : ", id);
	let annotation_array = [];
	[sentence, known_Change] = json2DataString();
	console.log("sentence : ", sentence);
	annotation_array = json2annotationpositions();
	let Background_color = json2Background_color();
	let protein_length_array = get_Ion_First_last_position() ;
	// Hard coding values to set the dimensions of svg
	width = width ;
	height = height ;
	
	splice_length = 30;
	x_scale_start_value = 30;
	x_scale_end_value = width-(width*(0.14));
					
	let List_of_Numbers = [];
	
	//change starts
	
	let protein_len = protein_length_array.protein_length ;
	let first_position = protein_length_array.first_residue_position ;
	let last_position = protein_length_array.last_residue_position ;
	
	console.log("protein_len : ", protein_len);
	console.log("first_position : ", first_position);
	console.log("last_position : ", last_position);
	
	let new_first_position = first_position ;
	let initial_skip_count = 0 ;
	let start_info = "" ;
	let boolean_start_info = false ;
	
	if(first_position > (splice_length + first_position %splice_length ) )
	{
		new_first_position = first_position - ((first_position%splice_length)+splice_length );
		initial_skip_count = new_first_position-1 ;
		start_info = "... "+initial_skip_count + " amino acid residues are skipped at the N-terminus ... ";
		boolean_start_info = true ;
	}
	let new_last_position = last_position ;
	let final_skip_count = 0 ;
	let end_info = "" ;
	let boolean_end_info = false ;
	
	if(last_position+(splice_length - (last_position%splice_length) +splice_length) < protein_len)
	{
		new_last_position = last_position+(splice_length - (last_position%splice_length) +splice_length)  ;
		final_skip_count = protein_len-new_last_position ;
		end_info = "... "+final_skip_count + " amino acid residues are skipped at the C-terminus ... ";
		boolean_end_info = true ;
	}
	
	console.log("new_first_position : ",new_first_position);
	console.log("new_last_position : ", new_last_position);
	console.log("initial_skip_count : ", initial_skip_count);
	console.log("final_skip_count : ", final_skip_count);
	console.log("start_info : ", start_info);
	console.log("end_info : ", end_info);
	console.log("boolean_start_info : ",boolean_start_info);
	console.log("boolean_end_info : ", boolean_end_info);
	
	sentence = sentence.substring(new_first_position,new_last_position) ;
	console.log("sentence : ", sentence);
	
	// change ends
	
	SVG_Container(Split_String(sentence), id, width, height,new_first_position,boolean_start_info,boolean_end_info,start_info,end_info);
	adding_Color(known_Change,id);
	
	//console.log("Background_color : ", Background_color) ;
	
	AddBackgroundColor(Background_color,id,splice_length);
	
	ion_position(annotation_array,id);
	
	residue_position(protein_length_array,id,width,height);
	//adding_AnnotaionLinks();
	
	d3.select('#saveImage_png').on("click", function(){
		
			let l_svgContainer = d3.select("svg");
			
			let svgString = getSVGString(l_svgContainer.node());
			
			let svg_element = document.getElementById('l_svg');
			let bBox = svg_element.getBBox();
			let width = bBox.width;
			let height = bBox.height ;
			svgString2Image( svgString, 2*width, 2*height, 'png', save ); // passes Blob and filesize String to the callback
			function save( dataBlob, filesize ){
				saveAs( dataBlob, 'PRSM.png' ); // FileSaver.js function
			}
		})	;
	
	d3.select('.resize').on("click", function(){
		
		let l_group_class_name = "."+"l_popup_svg"+"_g" ;
		
		d3.selectAll(l_group_class_name).remove();
		
		let l_width_span = document.getElementById("width-span").value;
		let l_height_span = document.getElementById("height-span").value ;
		let l_sequence_size = document.getElementById("sequence-span").value ;
		
		[width,height] = modal_window(l_width_span, l_height_span, l_sequence_size);
		
		build_svg('#l_popup_svg',width,height);
		
	});
	
	d3.select('#download').on("click",function(){
		
		d3.selectAll("#l_id_0").style("text-anchor","end");
		let svg_element = d3.select("#l_popup_svg").node();
		
		svg2svg(svg_element);
		
	});
	
	d3.select('#saveImage_svg').on("click",function(){
		let l_group_class_name = "."+"l_popup_svg"+"_g" ;
		
		d3.selectAll(l_group_class_name).remove();
		
		build_svg('#l_popup_svg',width,height);
		
	});
	
}
/*Function to split string as needed */
function Split_String(sentence){
	
	sentence = sentence.toUpperCase();	
	let split_sentence_Of_30 = sentence.match(/.{1,30}/g);
	
	/*Remove Later*/ 
	let space_of_Sentence_at_10 = [] ;
	split_sentence_Of_30.forEach(function(element){
		position = 10 ;
		second_position = 22 ;
		length_of_the_row = 34 ;
		element = " " +element.substr(0,position)+ " "+element.substr(position) ;
		element = element.substr(0,second_position)+ " " +element.substr(second_position) + " ";
		
		if(element.length < 34){
			element = element + ' '.repeat(length_of_the_row - element.length )
		}
		space_of_Sentence_at_10.push(element);
	})
	return space_of_Sentence_at_10;
}

function SVG_Container(data_30, id, width, height, new_first_position,boolean_start_info, boolean_end_info, start_info, end_info){
	
	let l_height_length = data_30.length ;
	
	if(boolean_start_info == true && boolean_end_info == true)
	{
		l_height_length = l_height_length + 2 ;
	}
	else if(boolean_start_info == true && boolean_end_info == false)
	{
		l_height_length = l_height_length + 1 ;
	}
	else if(boolean_start_info == false && boolean_end_info == true)
	{
		l_height_length = l_height_length + 1 ;
	}
	
	
	let l_height = height*l_height_length;
	console.log("data_30 : ", data_30);
	
	/* X axis co-ordinates*/ 
	let x_scale = d3.scaleLinear()
				.domain([0, splice_length])
				.range([x_scale_start_value, x_scale_end_value]);
				
	/* Y axis Co-ordinates */
	let y_scale = d3.scaleLinear()
				.domain([0, l_height_length])
				.range([40, l_height]);	
	
	svgContainer = d3.select(id).attr("width",width)
									.attr("height",l_height)
	
	let l_group_class_name = id.split("#")[1] +"_g" ;
	svgContainer = svgContainer.append("g").attr("class",l_group_class_name)
											.attr("id",l_group_class_name);									
	text = 	svgContainer.selectAll("text")
	
	/*  Dummy value for calculating the indexes of the row */
	let k = 0;
	if(boolean_start_info == true)
	{
		k = new_first_position/30 ;
	}
	console.log("k : ", k)
	
	let m = 0;
	let class_value = new_first_position;
	
	let id_temp = id + "skip_list_start" ;
	
/*	text.append("text")
		.attr("id", id_temp) 
		.attr("x",25)
		.attr("y", y_scale(m) )
		.text("sdfdjsfnksdfn")
*/
	l_list = [];
	l_list.push(start_info);
	
	if(boolean_start_info ==  true)
	{
		text.data(l_list)
			.enter()
			.append("text")
			.attr("id", "temp_id")
			.attr("x", x_scale(7))
			.attr("y",y_scale(m))
			.text(function(d,i){
				return d ;
			}) 
			.attr("font-family","'FreeMono',Miltonian,monospace")
			.attr("font-size","16px") ;
		m++ ;
	}
	
	
	
	/* code to write the text elements onto svg */
	data_30.forEach(function(input){
		
		length_withoutspaces = input.replace(/\s/g, "").length;
		
		text.data(input)
				.enter()
				.append("text")
				.attr("id",function(d,i){
					return "l_id_"+i
				})
				.attr("class", function(d,i){
					
					if(d == " " || d == null ){
						
						let class_name = "class_null_" +id.split("#")[1]+"_" + i ;
						return class_name ;
					}
					else{
						class_value++; 
						let class_name = "class_" +id.split("#")[1]+"_" + (class_value-1) ;
						return class_name ;
					}})
				.attr("x", function(d,i){ 
				return x_scale(i)})
				.attr("y", function(d,i){ 
					
				return y_scale(m)})
				.text(function(d,i){
				/* Index value calculation */
					if(i == 0)
					{
						d = k*30 +1 ;
						console.log("d : ", d);
					}
					else if((k == (data_30.length -1))&& (i == 33))
					{
						d = length_withoutspaces + k*30;
						d= '\u00A0' + d ;
					}
					else if( i == 33)
					{
						d = 30 + k*30;
						d = '\u00A0' + d ;
					}
					
				return d})
				.attr("font-family","'FreeMono',Miltonian,monospace")
				.attr("font-size","16px") ;
				k++;
				m++ ;
		})
		l_list = [] ;
		l_list.push(end_info);
	
		if(boolean_end_info ==  true)
		{
			text.data(l_list)
				.enter()
				.append("text")
				.attr("id", "temp_id")
				.attr("x", x_scale(7))
				.attr("y",y_scale(m))
				.text(function(d,i){
					return d ;
				}) 
				.attr("font-family","'FreeMono',Miltonian,monospace")
				.attr("font-size","16px") ;
			m++ ;
		}
}

/* Modifying the color array to match the array of letters and positions */
function adding_Color(known_Change,id){
	
	add_style = d3.select("svg").selectAll("text") ;
	
	known_Change.forEach(function(position,i){
		l_class_id = ".class_" +id.split("#")[1]+"_"+  + (parseInt(position))
		d3.select(l_class_id)
			.style("fill", "red")
	})
					
}

function ion_position(annotation_array,id){
	let l_group_id_name = id.split("#")[1] +"_p_g" ;//polyline group
	let l_group_class_name = id.split("#")[1] + "_g";
	let svgContainer = d3.select(id).append("g").attr("class",l_group_class_name)
												.attr("id",l_group_id_name);	
	
	annotation_array.forEach(function(data, i){
		// Data contains two variable, "first" one denote how the annotation should be and "second" one gives the position of annotation
		let l_class_id = ".class_" +id.split("#")[1]+"_"+ (parseInt(data.position)-1); // the cleavage position is in between the positions of acids, hence reducing by 1
		
		let x = parseFloat(d3.select(l_class_id).node().getAttribute('x')) + ((x_scale_end_value-x_scale_start_value)/(splice_length*2))
		let y = parseFloat(d3.select(l_class_id).node().getAttribute('y')) - 10 ;
		coordinates = " ";
		
		if(data.ion_type == "B")
		{
			coordinates = coordinates +(x)+","+(y-3)+ " " +(x+5)+","+ (y)+" "+(x+5)+","+(y+12);
		}
		if(data.ion_type == "Y")
		{
			coordinates = coordinates + (x+5)+","+ (y-1)+" "+(x+5)+","+(y+11)+ " "+(x+10) + ","+(y+14);
		}
		let l_charge = floating_Ion_Charge(annotation_array,data.position)
		
		var div = d3.select("body").append("div")	
					.attr("class", "tooltip")				
					.style("opacity", 0); 
								  
		let l_polyline = svgContainer.append("polyline")
						.attr("class", "l_polyclass_"+data.position)
						.attr("points", coordinates)
						.style("fill", "none")
						.style("stroke", "1e90ff")
						.style("stroke-width", "1");
		
		svgContainer.append("rect")
					.attr("x", x)
					.attr("y", y-5)
					.attr("width", 10)
					.attr("height", 20)
					//.attr("fill", "white")
					.style("opacity", 0)
					.attr("cursor", "pointer")
					.on("mouseover", function(){
							div.transition()		
								.duration(200)		
								.style("opacity", .9);
							div.html(l_charge)	
						       .style("left", (d3.event.pageX)  + "px")		
							   .style("top", (d3.event.pageY - 28)+ "px") ;
						})
						.on("click",function(){
								input = data.ion_position + ",";
								showIonPeaks(input);
							})
						.on("mouseout", function(d){
							div.transition()		
								.duration(500)		
								.style("opacity", 0);	
						});
	})
}

function floating_Ion_Charge(l_annotation_array,position){
	
	let l_charge = "";
	
	for(let j=0;j<l_annotation_array.length;j++){
		
		if(position == l_annotation_array[j].position )
		{
			 l_charge = l_charge + l_annotation_array[j].ion_type+l_annotation_array[j].ion_display_position
			 						+" "+l_annotation_array[j].peak_charge+"+ " ;
		}
	}
	return l_charge ;
}

/* code to add background color to specific set by taking array as input */
function AddBackgroundColor(l_Background_color, id,splice_length){
	
	l_Background_color.forEach(function(input, i){
		
		let first_position = parseInt(input.left_position) ;
		let second_position = parseInt(input.right_position) ;
		let position_list = []; 
		let position_array = {} ;
		
		for(let i = first_position ; i < second_position ; i++ )
		{
			if(i == first_position)
			{
				position_array["left_position"] = first_position ;
				position_array["anno"] = input.anno;
				position_array["color"] = input.color ;
			}
			
			if((i!=0) && i%splice_length == 0)
			{
				
				position_array["right_position"] = i ;
				position_list.push(position_array);
				position_array = {} ;
				position_array["left_position"] = i ;
				position_array["color"] = input.color ;
				
			}
			else if(i == second_position-1 )
			{
				position_array["right_position"] = second_position ;
				position_list.push(position_array);
			}
			
			
			
		}
		
		position_list.forEach(function(input,i){
			
			input.left_position = parseInt(input.left_position) ;
			input.right_position = (parseInt(input.right_position)-1) ;
			class_1 = ".class_" +id.split("#")[1]+"_"+input.left_position;
			class_2 = ".class_" +id.split("#")[1]+"_"+input.right_position ;
			color = input.color ;
			let bbox = d3.select(class_1).node().getBBox();
			console.log("d3.select(class_1).node() : ", d3.select(class_1).node());
			
			let bbox_2 = d3.select(class_2).node().getBBox();
			
			let width = bbox_2.x - bbox.x ;
			rect_Backgroundcolor(bbox,id,width);
			annotation_Mass_Shift(bbox,id,input.anno);
			
		})
		
	})
}

/*code to color the background of a set*/
function rect_Backgroundcolor(bbox,id,width){
	id = id + "_g";
	var rect = d3.select(id);
			rect.append("rect")
					.attr("x", bbox.x)
					.attr("y", bbox.y)
					.attr("width", width+12)
					.attr("height", bbox.height)
					//.attr("dy", "30%")
					.attr("dy", ".75em")
					.style("fill", color)
					.style("fill-opacity", ".4")
					.style("stroke-width", "1.5px");
}
function annotation_Mass_Shift(bbox,id,value)
{
	id = id + "_g" ;
	d3.select(id).append("text")
				   .attr("x", bbox.x)
				   .attr("y", bbox.y)
				   .attr("dy", "-.15em") // this code gives the shift on y axis from the co-ordinates provided similarly dx does the same
				   .text(function(){
					   return value ;
				   })
				   .attr("font-family","'FreeMono',Miltonian,monospace")
				   .attr("font-size","16px");
}

function residue_position(protein_length_array, id, width, height)
{
	let l_id = id + "_p_g" ;
	
	if(!(protein_length_array.first_residue_position == 0) )
	{
		l_class_first_position = ".class_" +id.split("#")[1]+"_"+(parseInt(protein_length_array.first_residue_position)-1);
		let x = parseFloat(d3.select(l_class_first_position).node().getAttribute('x')) + ((x_scale_end_value-x_scale_start_value)/(splice_length*2)) ;
		let y = parseFloat(d3.select(l_class_first_position).node().getAttribute('y')) -12 ;
		
		let first_position_coordinates = (x)+","+(y)+ " " +(x+5)+","+ (y)+" "+(x+5)+","+(y+14)+ " "+(x) + ","+(y+14);
		
		//let group_polyline_container =  d3.select(l_id)
		d3.select(l_id).append("polyline")
					.attr("class", "l_polyclass_"+ protein_length_array.first_residue_position)
					.attr("points", first_position_coordinates)
					.style("fill", "none")
					.style("stroke", "red")
					.style("stroke-width", "1.3") ;
					
		for(let i=0; i < parseInt(protein_length_array.first_residue_position) ; i++)
		{
			let l_class_list = ".class_" +id.split("#")[1]+"_"+i;
			d3.select(l_class_list).style("fill", "grey") 
		}
	}
	
	if(!(parseInt(protein_length_array.protein_length) == parseInt(protein_length_array.last_residue_position) +1 ))
	{
		let l_class_last_position = ".class_" + id.split("#")[1]+"_"+(parseInt(protein_length_array.last_residue_position));
		x = parseFloat(d3.select(l_class_last_position).node().getAttribute('x')) + ((x_scale_end_value-x_scale_start_value)/(splice_length*2)) ;
		y = parseFloat(d3.select(l_class_last_position).node().getAttribute('y')) -12 ;
		x =x+9;
		let last_position_coordinates = (x)+","+(y)+ " " +(x-5)+","+ (y)+" "+(x-5)+","+(y+14)+ " "+(x) + ","+(y+14);
		
		
		d3.select(l_id).append("polyline")
					.attr("class", "l_polyclass_"+ protein_length_array.first_residue_position)
					.attr("points", last_position_coordinates)
					.style("fill", "none")
					.style("stroke", "red")
					.style("stroke-width", "1.3") ;
					
		for(let i=parseInt(protein_length_array.last_residue_position)+1; i <= parseInt(protein_length_array.protein_length) ; i++)
		{
			let l_class_list = ".class_" +id.split("#")[1]+"_"+i;
			d3.select(l_class_list).style("fill", "grey") 
		}
	}
	
}

function modal_window(width_span, height_span, sequence_size )
{
	const ref_width = 1000;
	const ref_width_span = 28.2;
	const ref_height = 60 ;
	const ref_height_span = 50 ;
	
	//let width = screen.width ;
	let width = (ref_width*width_span)/ref_width_span ;
	
	let height = (ref_height*height_span)/ref_height_span ;
	//let height = screen.height ;
	
	return [width,height];
	
}
