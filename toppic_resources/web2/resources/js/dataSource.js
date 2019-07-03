/*	Get data from global variable spectrum_data and utilities to manupulate----
 * 	the data-------------------------------------------------------------------*/
DataSource = function(spectrum){
var self = this ;
this.spectrum = spectrum ;

/*	variables for the start and end points on the axis, range of the axis and--- 
 * scales on x and y axises	----------------------------------------------------*/
this.xStartDomain ;
this.xEndDomain ;
this.xRange;
this.MassMzCenterPointer = null ;
this.adjacentRange = 5 ;
this.xScale;
this.yScale;
this.Scale;

/*	New scales after zoom In and drag -------------------------------------------*/
this.new_Xscale = null;
this.new_Yscale = null;
this.tempMaxValue;
this.tempMax;
/*	Data containers - peakMass and peakIntensity contains all the mass&peak values-
 *  and peakmassShow and peakintensityShow contains only the data to be show on the
 *  graph limited to 1000 first peaks---------------------------------------------*/ 
this.peakmass = [];
this.peakintensity = [];
this.peakCircleData = [];
this.peakmassShow = [];
this.peakintensityShow = [];
this.peakCircleDataShow = [];
this.maxPeakIntensity;
this.maxPeakmass;
this.MAX_PEAKS_COUNT = 1000;
this.spectrumColorArray = ["red","orange","blue","black"]

this.radiusFactor = 0.04;
this.minRadius = 2;
this.maxRadius = 6;
this.dragxs = 0;
/*	Variable to show the data on hover of the peak lines--------------------------*/
this.textMZ;
this.textIN;
this.circleOrPeak;
this.mouseOverPadding = {massTop : 10, IntensityTop : 5, massTop2: 15, chargeGap:2} ;
this.zoomFactor = 1;
this.shiftX = null;
this.shiftY = null ;
this.onClickPeakYFactor = 2 ;


/*	Get the data from source file(from spectrum_data variable) and pass the data-- 
 * 	into peakmass and peakintensity ----------------------------------------------*/
this.axisConversion = function(spectrum){
	
	let i = spectrum_data.peaks.length ;
	
	/*	Pass the data into peakmass and peakintensity ----------------------------*/
	while(i--)
	{
		self.peakmass[i] = parseFloat(spectrum_data.peaks[i].mz);
		self.peakintensity[i] = parseFloat(spectrum_data.peaks[i].intensity);
	}
	
	let count = 0;
	
	self.peakCircleData = spectrum_data.envelopes ;
	self.peakCircleData.sort(function(x,y){
		return d3.ascending(x.env_peaks[0].mz, y.env_peaks[0].mz);
	})
	/*	Initialize the startDomain and endDomain and set range --------------------*/
	self.xStartDomain = 0 ;
	self.xEndDomain = d3.max(self.peakmass);
	self.xRange = spectrum.width - spectrum.padding.right ;
	
	/*	Check whether the spectrum is invoked by clicking on Mass M/z or by--------- 
	 * 	clicking on "view Spectrum". MassMzCenterPointer is set to null when clicked
	 *  on "view spectrum" ---------------------------------------------------------*/
	if(spectrum.dataSource.MassMzCenterPointer != null)
	{
		self.xStartDomain = parseFloat(self.MassMzCenterPointer)-this.adjacentRange ;
		self.xEndDomain = parseFloat(self.MassMzCenterPointer)+this.adjacentRange ;
		self.minScaleExent = 0.1;
	}
	self.xScale = d3.scaleLinear().domain([self.xStartDomain ,self.xEndDomain])
					.range([spectrum.padding.left, self.xRange]);
	self.yScale = d3.scaleLinear().domain([0,d3.max(self.peakintensity)])
					.range([spectrum.height - spectrum.padding.top - spectrum.padding.bottom, spectrum.padding.top]);
	
	
	self.tempMax = d3.max(self.peakintensity);
	self.tempMaxValue = d3.min(self.peakintensity);
	self.maxPeakIntensity = d3.max(self.peakintensity);
	
	this.new_Xscale = self.xScale.copy() ;
	this.new_Yscale = self.yScale.copy()  ;
	
	/*	Scale the xAxis into 10 ticks with mass M/z values on the x-axis------------ */
	self.xAxis = d3.axisBottom(self.xScale)
					.ticks(10)
					.tickSize(2)
					.tickPadding(6);
	
	/*	Scale the yAxis into 5 ticks with Intensity on the y-axis------------------- */
	self.yAxis = d3.axisLeft(self.yScale)
					.tickFormat(d3.format(".1e"))
					.ticks(5)
					.tickSize(2)
					.tickPadding(6);
}

/*	Passing the data to peakmassShow&peakintensityShow based on the zoom factor, -----
 *  domain,range and MAX_PEAKS_COUNT -------------------------------------------------*/
this.cmpMassIntensity = function() {
	var peakCount = 0;
	for(var i = 0; i < self.peakmass.length; i++) {
		var tmpx = self.new_Xscale(self.peakmass[i]);
		if(self.peakmass[i] > self.xStartDomain && self.peakmass[i] < self.xEndDomain)
		{
			if(tmpx > 0 && tmpx < self.xRange)
			{
				if(peakCount < self.MAX_PEAKS_COUNT)
				{
					self.peakmassShow[peakCount] = self.peakmass[i];
					self.peakintensityShow[peakCount] = self.peakintensity[i];
					peakCount++;
				}
			}
		}
	}
	self.xStartDomain = d3.min(self.new_Xscale.domain());
	self.xEndDomain = d3.max(self.new_Xscale.domain());
	
	self.Scale = (self.xEndDomain-self.xStartDomain)/self.xRange ;
	
	self.peakCircleDataShow = [];
	self.peakCircleData.forEach(function(peakCircleData,i){
		let tempEnvelopes = {};
		let colorIndex = i%(self.spectrumColorArray.length);
		tempEnvelopes.mono_mass = peakCircleData.mono_mass;
		tempEnvelopes.charge = peakCircleData.charge ;
		tempEnvelopes.color = self.spectrumColorArray[colorIndex]
		tempEnvelopes.env_peaks = [];
		peakCircleData.env_peaks.forEach(function(env_peaks,j){
			var tmpx = self.new_Xscale(env_peaks.mz);
			if(env_peaks.mz > self.xStartDomain && env_peaks.mz < self.xEndDomain)
			{
				if(tmpx > 0 && tmpx < self.xRange)
				{
					if(peakCount < self.MAX_PEAKS_COUNT)
					{
						let tempEnvPeaks = [];
						tempEnvPeaks.mz = env_peaks.mz;
						tempEnvPeaks.intensity = env_peaks.intensity;
						tempEnvelopes.env_peaks.push(tempEnvPeaks);
					}
				}
			}
		})
		if(tempEnvelopes.env_peaks.length > 0)
		{
			self.peakCircleDataShow.push(tempEnvelopes);
		}
	})
	if(spectrum.dataSource.MassMzCenterPointer != null)
	{
		if(d3.max(self.peakintensityShow) == d3.max(self.peakintensity))
		{
			self.onClickPeakYFactor = 1 ;
		}
		
		self.yScale = d3.scaleLinear().domain([0,d3.max(self.peakintensityShow)*self.onClickPeakYFactor])
									.range([spectrum.height - spectrum.padding.top - spectrum.padding.bottom, spectrum.padding.top]);
		this.new_Yscale = self.yScale.copy()  ;	
		self.maxPeakIntensity = d3.max(self.peakintensityShow)*self.onClickPeakYFactor;
		self.yAxis = d3.axisLeft(self.yScale)
							.tickFormat(d3.format(".1e"))
							.ticks(5)
							.tickSize(2)
							.tickPadding(6);
								
		spectrum.dataSource.MassMzCenterPointer = null ;	
	}
	
	
}

/*	Data to be show on hover of the peak lines -----------------------------------------*/
this.over = function(this_element,peakMass,index,EnvData){
	var tmpData ;
	let x,y;
	let color ;
	
	if(EnvData == null)
	{
		x = self.new_Xscale(peakMass) - spectrum.padding.left;
		tmpData = [[peakMass,this.new_Yscale(self.peakintensityShow[index])]];
		y = this.new_Yscale(self.peakintensityShow[index]);
		color = "red"
	}
	else
	{
		x = d3.select(this_element).attr("cx");
		tmpData = [[EnvData.mono_mass]];
		console.log("this_element.cy : ", d3.select(this_element).attr("cy"));
		y = parseFloat(d3.select(this_element).attr("cy")) ;
		color = "orange" ;
	}
	console.log("y : ", y);
	console.log("spectrum.height - spectrum.padding.bottom-spectrum.padding.top : ", spectrum.height - spectrum.padding.bottom-spectrum.padding.top);
	d3.select(this_element).attr("stroke",color)
		.attr("stroke-width","2");
		self.textMZ = spectrum.group_top.selectAll(".MyTextMZ")
		.data(tmpData)
		.enter()
		.append("text")
		.attr("class","MyTextMZ")
		.attr("transform","translate(" + spectrum.padding.left + "," + spectrum.padding.top + ")")
		.attr("x", function(d) {
			/*	Show the M/Z data in the svg box range ---------------------------------------*/ 
		 	if (x > 0 && x < self.xRange){
		 		return x;
		 	}else{
		 		return -1000;
		 	}
		})
	.attr("y", function(d) {
		/*	Show the data with in the height range, reduce it by 20 to be in the range---- 
 		 * 	and over the top of the peak--------------------------------------------------*/
		console.log("y : ", y);
	 	if (y > spectrum.height - spectrum.padding.bottom-spectrum.padding.top){
	 		return spectrum.height - spectrum.padding.bottom - spectrum.padding.top ;
	 	}else if (y < 0){
	 		return 0 ;
	 	}else {
	 		return y - self.mouseOverPadding.massTop;
	 	}
	})
	.text(function(d){
		/*	d[0] contains the first element of the data array tmpData---------------------*/
		if(EnvData == null)
		{
			return "M:"+d[0];
		}
		else
		{
			return "Mass:"+parseFloat(d[0]).toFixed(2);;
		}
		
	});
	self.textIN = spectrum.group_top.selectAll(".MyTextIN")
	.data(tmpData)
	.enter()
	.append("text")
	.attr("class","MyTextIN")
	.attr("transform","translate(" + spectrum.padding.left + "," + spectrum.padding.top + ")")
	.attr("x", function(d) {
	 	if (x > 0 && x < self.xRange){
	 		return x;
	 	}else{
	 		return -1000;
	 	}
	})
	.attr("y", function(d) {
		/*	Add -5 to have gap between M/Z data and Intensity Value -----------------------*/
	 	if (y > spectrum.height - spectrum.padding.bottom-spectrum.padding.top){
	 		return spectrum.height - spectrum.padding.bottom - spectrum.padding.top;
	 	}else if (y < 0){
	 		return 0 + self.mouseOverPadding.massTop2 ;
	 	}else {
	 		return y + self.mouseOverPadding.chargeGap;
	 	}
	})
	.text(function(d){
		if(EnvData == null)
		{
			return "I:"+self.peakintensityShow[index];
		}
		else
		{
			return "Charge:"+ EnvData.charge;
		}
	});
	
}
/*	Remove the data shown on coming out of hover on Peak line ------------------------------*/
this.out = function(this_element,d,EnvData){
	console.log("mouseOut");
	self.textIN = spectrum.group_top.selectAll(".MyTextIN")
	self.textMZ = spectrum.group_top.selectAll(".MyTextMZ")
	d3.select(this_element).transition()
	       		.duration(10)
				.attr("stroke",function(d){
					if(EnvData == null)
					{
						return "black";	
					}
					else
					{
						return d3.select(this_element).attr("stroke") ;
					}
				})
				.attr("stroke-width","2");
				
	self.textMZ.remove();
	spectrum.group_top.selectAll(".MyTextIN").remove();
}
console.log("self.minScaleExent : ", self.minScaleExent);
/*	Zoom method declaration and invoke Zoomed function ----------------------------------------*/
this.xZoom = d3.zoom().scaleExtent([spectrum.minScaleExtent,10000]).on("zoom", xZoomed);
this.yZoom = d3.zoom().scaleExtent([0,100000]).on("zoom", yZoomed);
this.dragx = d3.drag().on("start", started).on("drag", dragged).on("end", dragended);

function started()
{
	 d3.event.sourceEvent.stopPropagation();
	  d3.select(this).classed("dragging", true);
		spectrum.svg.attr("cursor","move");
}
function dragged(d) {
	
	console.log("d3.event.pageX : ", d3.event.pageX);
	/*
	/*console.log("d3.event.dx : ",d3.event.dx );*/
	/*console.log("d3.event: ",d3.event);*/
	/*if(self.Scale > 1) d3.event.dx = d3.event.dx*2;*/
/*	let temp_range = (self.xEndDomain -self.xStartDomain)/self.xRange ; 
	console.log("this.shiftX : ", self.shiftX, " d3.event.dx : ", d3.event.dx);
	console.log("self.zoomFactor : ", self.zoomFactor);
	
	self.dragxs += d3.event.dx/self.zoomFactor * 0.5;
	
	self.shiftX = (self.xEndDomain*self.dragxs)/self.xRange;
	
	console.log("after this.shiftX : ", self.shiftX, " self.dragxs : ", self.dragxs);
	/*console.log("d3.event.dx : ",d3.event.dx );*/
	/*console.log("d3.event: ",d3.event);*/
	/*if(self.Scale > 1) d3.event.dx = d3.event.dx*2;*/
	let temp_range = (self.xEndDomain -self.xStartDomain)/self.xRange ; 
	self.dragxs = d3.event.dx ;
	
	self.xStartDomain = d3.min(self.new_Xscale.domain()) ;
	self.xEndDomain = d3.max(self.new_Xscale.domain()) ;
	/*self.xStartDomain -= self.dragxs ;
	self.xEndDomain -= self.dragxs ;*/
	self.xStartDomain = self.xStartDomain - temp_range*d3.event.dx;
	self.xEndDomain =  self.xEndDomain - temp_range*d3.event.dx;
	
	console.log("self.xStartDomain : ", self.xStartDomain);
	console.log("self.xEndDomain : ", self.xEndDomain);
	self.new_Xscale = self.new_Xscale.domain([self.xStartDomain,self.xEndDomain]);
	/*	On Zoom changes the cooordinates in the X-axis	---------------------------------------*/
	console.log("d3.min(self.new_Xscale.domain()) ; ", d3.min(self.new_Xscale.domain()) );
	console.log("d3.max(self.new_Xscale.domain()) : ", d3.max(self.new_Xscale.domain()) );
	spectrum.x_Axis.call(self.xAxis.scale(self.new_Xscale));
	
	resetSpectrum(spectrum,self.new_Xscale,self.new_Yscale);
}
function dragended(){
	d3.select(this).classed("dragging", false);
	spectrum.svg.attr("cursor","default");
}
/*	Zoom on scroll at X axis ------------------------------------------------------------------*/
function xZoomed() {
	
	/*console.log("d3.event.pageX : ", d3.event.pageX);
	console.log("d3.event.transform : ", d3.event.transform, " d3.event : ", d3.event);
	console.log("manual scale : ", self.xScale(300)*d3.event.transform.k + d3.event.transform.x + spectrum.padding.left*(d3.event.transform.k - 1));
	console.log("new scale : ", self.new_Xscale(300));
	
	console.log("After d3.event.transform : ", d3.event.transform, " d3.event : ", d3.event);*/
	
	//self.xStartDomain = self.xStartDomain - self.xScale()*d3.event.transform.k + d3.event.transform.x + spectrum.padding.left*(d3.event.transform.k - 1) ;
	/*let x = spectrum.padding.left * (d3.event.transform.k - 1);
	console.log("x : ", x);
	console.log("self.xStartDomain 11 : ", self.xStartDomain)
	d3.event.transform
	 /*	New d3 scale after zooming ------------------------------------------------------------*/
	
	/*if(self.zoomFactor != null && self.shiftX !=null & self.shiftY != null)
	{
		console.log("in factor");
		d3.event.transform.k = self.zoomFactor ;
		d3.event.transform.x = self.shiftX ;
		d3.event.transform.y = self.shiftY;
	}*/
	self.new_Xscale = d3.event.transform.rescaleX(self.xScale) ;
	self.zoomFactor = d3.event.transform.k ;
	self.shiftX = d3.event.transform.x ;
	self.shiftY = d3.event.transform.y;
	
/*	let temp_range = (self.xEndDomain -self.xStartDomain)/self.xRange ; 
	//console.log("d3.event.transform : ", d3.event.transform, " d3.event : ", d3.event);
	self.dragxs = d3.event.transform.k;*/
	/*	Change the domains as we zoom in and zoom out to the peakmassShow and peakIntentisyShow*/
	self.xStartDomain = d3.min(self.new_Xscale.domain());
	
	self.xEndDomain = d3.max(self.new_Xscale.domain());
	//self.new_Xscale = self.new_Xscale.domain([self.xStartDomain,self.xEndDomain]);
	
	/*	On Zoom changes the cooordinates in the X-axis	---------------------------------------*/
	spectrum.x_Axis.call(self.xAxis.scale(self.new_Xscale));
	
	resetSpectrum(spectrum,self.new_Xscale,self.new_Yscale);
};
/*	Zoom on scrolling at Y axis ---------------------------------------------------------------*/
function yZoomed() {
	/*	New d3 scale after zooming ------------------------------------------------------------*/
	
	//console.log("Y d3.event.transform : ", d3.event.transform);
	
	let scaleY = self.new_Yscale.copy();
	var dY = event.deltaY;
	var dX = event.deltaX;
	//console.log("self.tempMaxValue : ", self.tempMaxValue);
	
	if(dY < 0 && self.tempMaxValue >= d3.min(self.peakintensity))
	{
		self.tempMaxValue =  self.maxPeakIntensity/1.2;
		self.maxPeakIntensity =  self.tempMaxValue ;
		
		if(self.tempMaxValue > 0)
		{
			scaleY = d3.event.transform.rescaleY(self.yScale).domain([0,self.tempMaxValue])
									.range([spectrum.height - spectrum.padding.top - spectrum.padding.bottom, spectrum.padding.top]) ;
			self.new_Yscale = scaleY ;
		}
	}
	/*console.log("d3.max(self.peakintensity) : ", d3.max(self.peakintensity));
	console.log("self.tempMaxValue : ", self.tempMaxValue);*/
	if(dY > 0 && self.tempMaxValue <= d3.max(self.peakintensity))
	{
		self.tempMaxValue =  self.maxPeakIntensity * 1.2;
		self.maxPeakIntensity =  self.tempMaxValue ;
		if(self.tempMaxValue <= d3.max(self.peakintensity))
		{
			scaleY = d3.event.transform.rescaleY(self.yScale).domain([0,self.tempMaxValue])
									.range([spectrum.height - spectrum.padding.top - spectrum.padding.bottom, spectrum.padding.top]);
			 self.new_Yscale = scaleY ;
		}
		
		if (typeof scaleY === 'undefined')
		{
			console.log("IN undefined");
			scaleY = self.yScale ;
			 self.new_Yscale = scaleY;
		}
	}
	
	//console.log("self.new_YScale : ", self.new_Yscale);
	spectrum.y_Axis.call(self.yAxis.scale(self.new_Yscale));
	resetSpectrum(spectrum,self.new_Xscale,scaleY);
};

}