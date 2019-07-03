/*	Spectrum start point */
this.drawSpectrum = function(peakValue){
	
	/*	Initialization of spectrum svg with id ms1svg---*/
	let spectrum = new Spectrum("#ms1svg",peakValue);
	
	/*	Remove axis and MyPeak, if exists and continue-- 
	 *	to redraw the spectrum--------------------------*/
	d3.selectAll(".axis").remove();
	d3.selectAll(".MyPeak").remove();
	d3.selectAll(".circles").remove();
	/*	Get the clicked Mass M/z value------------------*/
	spectrum.dataSource.MassMzCenterPointer = peakValue ;
	
	/*	Get the data from source file and convert the data 
	 *	to form spectrum---------------------------------*/
	spectrum.dataSource.axisConversion(spectrum);
	
	/*	Get the data that is to be shown on the spectrum-
	 * 	after conversion --------------------------------*/
	spectrum.dataSource.cmpMassIntensity();
	
	/*	Add peaks to the spectrum graph------------------*/
	addPeaks(spectrum);
	
	/*	Add Axis to the spectrum graph-------------------*/
	addAxis(spectrum);
	
	console.log("spectrum : ", spectrum);
	addEnvelopes(spectrum);
}

/* spectrum.js: spectrum initialization code and helper methods*/
function Spectrum(containerEl,peakValue) {
	/*	Add spectrum object to global scope------------*/
	self = this;
	this.containerEl = containerEl;
	this.minScaleExtent = 1;
	if(peakValue != null)
	{
		this.minScaleExtent = 3/(peakValue-5);
	}
	this.dataSource = new DataSource(this);
	this.x_Axis;
	this.y_Axis;
	
	/*	Set width and height of svg--------------------*/
	this.width = 910;
	this.height = 220;
	
	/*	Set padding values to the svg------------------*/
	this.padding = {left:100, right:10, top:10, bottom:50};
	
	/*	Initialize svg with width and height-----------*/
	this.svg =  d3.select("body").select(containerEl)
					.attr("preserveAspectRatio", "xMidYMid meet")
					.attr("viewBox", "0 0 "+this.width+" "+this.height)
					.attr("width", "100%")
					.attr("height", "100%")
	
	
	
	/*	Initialize svg with group bottom to invoke--- 
	 * 	Zoom function on scroll up and down---------*/
	this.group_bottom = this.svg.append("g")
						    .call(this.dataSource.xZoom)
							/*.call(this.dataSource.dragx)
							.on("mousedown.zoom", null)
						    .on("touchstart.zoom", null)
						    .on("touchmove.zoom", null)
						    .on("touchend.zoom", null);*/
						    
	
	/*	Initialize the tech bottom with the rect of height and width to zoom 
	 * 	in on xaxis on any part of the rect block in the bottom--------------*/
	this.tect_bottom = this.group_bottom.append("rect")
						  	.attr("x",this.padding.left)
						  	.attr("y",this.height - this.padding.bottom)
						  	.attr("width",this.width-this.padding.left-this.padding.right )
						  	.attr("height",this.padding.bottom)
						  	.attr("style","fill:rgb(255,255,255);stroke-width:0;stroke:rgb(0,0,0);fill-opacity:0");
	
	/*this.group_xy = this.svg.append("g")
							.call(this.dataSource.dragx)
							.call(this.dataSource.yZoom);
	
	this.tech_xy = this.group_xy.append("rect")
								.attr("class","drag_rect")
								.attr("x",this.padding.left)
								.attr("y",this.padding.top)
								.attr("width",this.width-this.padding.left-this.padding.right )
								.attr("height",this.height - this.padding.top-this.padding.bottom)
								.attr("style","fill:rgb(255,255,255);stroke-width:0;stroke:rgb(0,0,0);fill-opacity:0");*/
	
	/*	Initialize svg with y axis zoom functionality 
	 * 	when triggered--------------------------------*/				
	this.group_top = this.svg.append("g")
						.attr("class","yZoom")
						.call(this.dataSource.dragx)
						.call(this.dataSource.yZoom)
						.on("dblclick.zoom", null);
	
	/*	Initialize svg as tech_top with a rect to zoom
	 *  in at any part on y axis and at the peaks--*/
	this.tect_top = this.group_top.append("rect")
					  	.attr("x","0")
					  	.attr("y",this.padding.top)
					  	.attr("width",this.width-this.padding.right)
					  	.attr("height",this.height - this.padding.bottom - this.padding.top )
					  	.attr("style","fill:rgb(255,255,255);stroke-width:0;stroke:rgb(0,0,0);fill-opacity:0") ;
					  	/*.call(this.dataSource.yZoom);*/
	
	/*	Pass all the peaks into a peaks variable if exists ---------------------------*/
	this.peaks = this.group_top.selectAll(".MyPeak");
	
	this.circles = this.group_top.append("g").selectAll(".circles");
}