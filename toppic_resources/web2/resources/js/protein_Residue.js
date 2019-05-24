
//console.log("width: ",window.width);

function protein_Sequence(residue_sequence,index,residue_Data, unexpected_change_list){
	
	/*
	 * Sample input Data looks like
	 * residue_sequence = "MPRSLKKGPFIDLHLLKKVEKAVESGDKKPLRTWSRRSTIFPNMIGLTIAVHNGRQHVPVFVSDEMVGHKLGEFAPTRTYRGHAADKKAKKK";
	 * index = 0/1/2;
	 * residue_Data = {first_residue_position: 1, last_residue_position: 91, protein_length: 92, known_change: Array(0)};
	 * unexpected_change_list: [{match_seq: "14.95533", first_position: "13", color:"red",last_position: "43"},];
	 */
	console.log("unexpected_change_list : "+ unexpected_change_list);
	let svgContainer ;
	let color_Array ;
	[svgContainer,color_Array]= SVG_Container(Split_String(residue_sequence),index);
	AddBackgroundColor(unexpected_change_list, index);
	residue_position(residue_Data,index);
	
	return svgContainer ;
	
}
/* Function to split string as needed */
function Split_String(sentence){
	
	sentence = sentence.toUpperCase();	
	split_sentence_Of_30 = sentence.match(/.{1,30}/g);
	
	/* Remove Later */
	space_of_Sentence_at_10 = []
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

function SVG_Container(data_30,index){
	console.log("data_30 : ",data_30.length);
	var height = 50 ;
	var width = 1000;
	var splice_length = 30;
	var x_scale_start_value = 25;
	var x_scale_end_value = width-(width*(0.13));
	
	height = height*data_30.length;
	/*X axis co-ordinates */
	x_scale = d3.scaleLinear()
				.domain([0, splice_length])
				.range([x_scale_start_value, x_scale_end_value]);
				
	/* Y axis Co-ordinates */
	y_scale = d3.scaleLinear()
				.domain([0, data_30.length])
				.range([40, height]);
	
			
	/* Splitting Color into array of 30 each */
	l_color_array = []

	console.log("index : "+index);
	let id = "#svgContainer" + index;
	//id = "#"+id ;
	l_svgContainer = d3.select(id).append("svg").attr("width",width)
									.attr("height",height) ;
	
	l_svgContainer = l_svgContainer.append("g");							
	text = 	l_svgContainer.selectAll("text");
	
	/* Dummy value for calculating the indexes of the row */
	k = 0;
	class_value = 0;
	
	/* code to write the text elements onto svg */
	data_30.forEach(function(input){
					length_withoutspaces = input.replace(/\s/g, "").length;
					if(k == data_30.length){k=0}
					//else{k++}
					text.data(input)
							.enter()
							.append("text")
							.attr("id",function(d,i){
								return "l_id_"+i
							})
							.attr("class", function(d,i){ 
							if(d == " " || d == null ){
							return "l_class_null_"+i
							}
							else{
								class_value++; 
								return "l_class_"+(class_value-1)
							}})
							
							.attr("x", function(d,i){ 
							return x_scale(i)})
							.attr("y", function(d,i){ 
							return y_scale(k)})
							.text(function(d,i){
							/* Index value calculation */
								if(i == 0)
								{
									d = k*30 +1 ;
								}
								else if((k == (data_30.length -1))&& (i == 33))
								{
									d = length_withoutspaces + k*30;
									d= '\u00A0' + d ;
								}
								else if( i == 33)
								{
									d = 30 + k*30;
									d= '\u00A0' + d ;
								}
								
							return d})
							k++;
							
							
		})
		return [l_svgContainer,l_color_array];	
}

function AddBackgroundColor(unexpected_change_list, index){
	
	let splice_length = 30;
	
	unexpected_change_list.forEach(function(input, i){
	
		// ---- for each input column '0' contain start position of background color, --
		// ---- position '1' contain the end position of color and '2' contain the color value --
		
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
			class_1 = ".l_class_" + input.left_position;
			class_2 = ".l_class_"+input.right_position ;
			color = input.color ;
			let bbox = d3.select(class_1).node().getBBox();
			console.log("d3.select(class_1).node() : ", d3.select(class_1).node());
			
			let bbox_2 = d3.select(class_2).node().getBBox();
			
			let width = bbox_2.x - bbox.x ;
			rect_Backgroundcolor(bbox,width,index);
			annotation_Mass_Shift(bbox,input.anno,index);
			
		})
	})	
}

function rect_Backgroundcolor(bbox,width,index){
	let id = "#svgContainer" + index;
	var rect = d3.select(id).select("svg");
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

function annotation_Mass_Shift(bbox,value,index)
{
	let id = "#svgContainer" + index;
	d3.select(id).select("svg").append("text")
				   .attr("x", bbox.x)
				   .attr("y", bbox.y)
				   .attr("dy", "-.15em") // this code gives the shift on y axis from the co-ordinates provided similarly dx does the same
				   .text(function(){
				   return value ;
				   });
}

function residue_position(residue_Data,index)
{
	console.log("residue_Data : ", residue_Data);
	var height = 50 ;
	var width = 1000;
	var splice_length = 30;
	var x_scale_start_value = 20;
	var x_scale_end_value = width-120;
	let id = "#svgContainer" + index;
	let x ;
	let y;
	let l_class_id_1 = ""
	let	l_class_id_2 = "";
	if(! ("0" == residue_Data.first_residue_position))
	{
		l_class_id_1 = ".l_class_" + (parseInt(residue_Data.first_residue_position)-1);
		
		x = parseFloat(d3.select(l_class_id_1).node().getAttribute('x')) + ((x_scale_end_value-x_scale_start_value)/(splice_length*2)) ;
		y = parseFloat(d3.select(l_class_id_1).node().getAttribute('y')) - 10 ;
		x = x;
		y = y-1;
		let first_position_coordinates = (x)+","+(y)+ " " +(x+5)+","+ (y)+" "+(x+5)+","+(y+12)+ " "+(x) + ","+(y+12);
		d3.select(id).select("svg").append("polyline")
					.attr("class", "l_polyclass_"+ residue_Data.first_residue_position)
					.attr("points", first_position_coordinates)
					.style("fill", "none")
					.style("stroke", "red")
					.style("stroke-width", "1.3")	
					
		for(let i=0; i < parseInt(residue_Data.first_residue_position) ; i++)
		{
			l_class_id = ".l_class_"+i;
			console.log("d3.select(l_class_id).node() : ", d3.select(l_class_id).node	);
			d3.select(id).select(l_class_id).style("fill", "grey") 
		}
		
	}
	if(!(parseInt(residue_Data.protein_length) == parseInt(residue_Data.last_residue_position) +1 ))
	{
		l_class_id_2 = ".l_class_" + (parseInt(residue_Data.last_residue_position));
		x = parseFloat(d3.select(l_class_id_2).node().getAttribute('x')) + ((x_scale_end_value-x_scale_start_value)/(splice_length*2)) ;
		y = parseFloat(d3.select(l_class_id_2).node().getAttribute('y')) - 10 ;
		x =x+9;
		let first_position_coordinates = (x)+","+(y)+ " " +(x-5)+","+ (y)+" "+(x-5)+","+(y+12)+ " "+(x) + ","+(y+12);
		d3.select(id).select("svg").append("polyline")
					.attr("class", "l_polyclass_"+ residue_Data.first_residue_position)
					.attr("points", first_position_coordinates)
					.style("fill", "none")
					.style("stroke", "red")
					.style("stroke-width", "1.3")
		for(let i=parseInt(residue_Data.last_residue_position)+1; i <= parseInt(residue_Data.protein_length) ; i++)
		{
			let l_class_id = ".l_class_"+i;
			console.log("d3.select(l_class_id).node() : ", d3.select(l_class_id).node);
			d3.select(id).select(l_class_id).style("fill", "grey") 
		}
	}
	
	residue_Data.known_change.forEach(function(known_change,i){
		
		//residue_position start with 0, so add one to the residue_position
		
		let l_class_id = ".l_class_" + (parseInt(known_change.residue_position));
		d3.select(id).select(l_class_id).style("fill", "red") 
	})

}