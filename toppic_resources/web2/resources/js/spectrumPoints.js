/*	Add peak lines in the spectrum graph ---------------------------------------*/
var addPeaks = function(spectrum) {
	/*	Set the spectrum to global self object	--------------------------------*/
	var self = spectrum;
	self.peaks.data(self.dataSource.peakmassShow)
	.enter()
	.append("line")
	.attr("class","MyPeak")
	.attr("transform","translate(" + self.padding.left + "," + self.padding.top + ")")
	.attr("x1",function(d,i){
		return self.dataSource.xScale(d) - self.padding.left;
	})
	.attr("y1",function(d,i){
		return self.dataSource.yScale(self.dataSource.peakintensityShow[i]);
	})
	.attr("x2",function(d,i){
		return self.dataSource.xScale(d)-self.padding.left;
	})
	.attr("y2",self.height - self.padding.bottom - self.padding.top)
	.attr("stroke",function(d,i){
		return "black";
	})
	.attr("stroke-width","2")
	.on("mouseover",function(d,i){
		console.log("on Mouse Over");
		self.dataSource.over(this,d,i,null);
	})
	.on("mouseout",function(d,i){
		self.dataSource.circleOrPeak = "peak" ;
		self.dataSource.out(this,d,null);
	});  
}
/*	Add Axis X and Y --------------------------------------------------------------*/
var addAxis = function(spectrum) {
	var self = spectrum;
	/*	Draw X axis in the svg ----------------------------------------------------*/
	self.x_Axis = self.group_bottom.append("g")
		.attr("id","xAxis")
		.attr("class","axis")
		.attr("transform","translate(" + 0 + "," + (self.height - self.padding.bottom) + ")")
		.call(self.dataSource.xAxis) ;
	/*	Label x axis --------------------------------------------------------------*/
	d3.select("#xAxis").append("text") 
					.attr("class","mz")
				    .attr("transform",
				          "translate(" + ((self.width)/2) + " ," + 
				          (self.padding.bottom - self.padding.bottom/3) + ")")
						.style("fill","black")
					    .attr("font-family","sans-serif")
					    .attr("font-size","16px")
					    .text("m/z");
						     
	/* set ticks on the x axis ---------------------------------------------------*/
	d3.select("#xAxis").selectAll(" g.tick line")	
					.attr("y2", function(d){
							   return 7;
								})
					.style("stroke-width","1")
					.style("fill","none")
						.style("stroke","black");
	
	/*	Draw Y axis in the svg ----------------------------------------------------*/
	self.y_Axis = self.group_top.append("g")
		.attr("id","yAxis")
		.attr("class","axis")
		.attr("transform","translate(" + (self.padding.left) + "," + (self.padding.top) + ")")
		.call(self.dataSource.yAxis);
	/*	Label Y Axis ---------------------------------------------------------------*/
	self.y_Axis.append("text")
			.attr("class", "yaxis_label")
			.attr("text-anchor", "middle") 
			.attr("transform", "translate("+ (self.padding.left/3-self.padding.left) +","+((self.height-self.padding.bottom)/2)+")rotate(-90)")
			.style("fill","black")
		    .attr("font-family","sans-serif")
		    .attr("font-size","16px")
			.text("Intensity");
	
	/*	set ticks on the y axis --------------------------------------------------*/
	d3.select("#yAxis").selectAll("g.tick line")	
					.attr("x2", function(d){
							   return -7;
					})
					.style("stroke-width","1")
					.style("fill","none")
					.style("stroke","black");
	d3.selectAll("g.tick text")
	.style("font", "13px sans-serif");
}
var addEnvelopes = function(spectrum){
	//console.log("peakCircleData : ", peakCircleData);
	var self = spectrum;
	spectrum.dataSource.peakCircleDataShow.forEach(function(peakCircleDataShow,index){
		self.circles.data(peakCircleDataShow.env_peaks)
			.enter()
			.append("circle")
			.attr("class","circles")
			.attr("transform","translate(" + self.padding.left + "," + self.padding.top + ")")
			.attr("cx",function(d,i){
				return self.dataSource.new_Xscale(d.mz) - self.padding.left ;
			})
			.attr("cy",function(d,i){
				var tmpy = self.dataSource.new_Yscale(d.intensity);
  			  		/*	Check to restrict Y Scale with in axis range --------------------------*/
  			  		if (tmpy > self.height - self.padding.bottom){
  			  			return self.height - self.padding.bottom;
  			  		}else if (tmpy < self.padding.top){
  			  			return self.padding.top;
  			  		}else {
  			  			return tmpy;
  			  		}
			})
			.attr("r",function(d,i){
				var r = self.dataSource.radiusFactor/self.dataSource.Scale;
					if (r < self.dataSource.minRadius) r = self.dataSource.minRadius;
  					if (r > self.dataSource.maxRadius) r=self.dataSource.maxRadius;
  					return r ;
				return 2 ;
			})
			.style("fill","white")
			.style("opacity", "0.6")
			.style("stroke",peakCircleDataShow.color)
			.style("stroke-width","2")
			.on("mouseover",function(d,i){
					console.log("this : ", this);
					self.dataSource.over(this,null,i,peakCircleDataShow);
				})
			.on("mouseout",function(d,i){
				self.dataSource.out(this,d,peakCircleDataShow);
			});
	})
}
/*	Clear Axis groups when reset is triggered ------------------------------------*/
var clearAxis = function(spectrum) {
	var self = spectrum;
	self.group_bottom.selectAll("g").remove();
	self.group_top.selectAll("g").remove();
}
/*	Redraw Spectrum based on the new scale on zoom -------------------------------*/
var resetSpectrum = function(spectrum, new_Xscale, new_Yscale)
{
	/*	Pass the spectrum to global self variable --------------------------------*/
	var self = spectrum;
	self.dataSource.new_Yscale = new_Yscale ;
	/*	Get the new data based on the new xStartDomain and xEndDomain ------------*/
	self.dataSource.cmpMassIntensity();
	var updatePeak = self.group_top.selectAll(".MyPeak").data(self.dataSource.peakmassShow);
  	updatePeak	
	.attr("x1",function(d,i){
		var tmpx = self.dataSource.new_Xscale(d)- self.padding.left;
		//var tmpx = (new_Xscale(d)- self.padding.left);
  		if (tmpx > 0 && tmpx < (self.dataSource.xRange-self.padding.left)){
  			return tmpx;
  		}else{
  			return -1000;
  		}
	})	
	.attr("y1",function(d,i){
  		var tmpy = new_Yscale(self.dataSource.peakintensityShow[i]);
  		/*	Check to restrict Y Scale with in axis range --------------------------*/
  		if (tmpy > self.height - self.padding.bottom){
  			return self.height - self.padding.bottom;
  		}else if (tmpy < self.padding.top){
  			return self.padding.top;
  		}else {
  			return tmpy;
  		}
	})
	.attr("x2",function(d,i){
		var tmpx = self.dataSource.new_Xscale(d)- self.padding.left;
		//var tmpx = new_Xscale(d)- self.padding.left;
  		if (tmpx > 0 && tmpx < (self.dataSource.xRange -self.padding.left)){
  			return tmpx;
  		}else{
  			return -1000;
  		}
	})
	.attr("y2",self.height - self.padding.bottom - self.padding.top);
	/*	Add new peaks when new data is included as per new scale -------------------*/
	updatePeak
	.enter()
	.append("line")
	.attr("class","MyPeak")
	.attr("transform","translate(" + self.padding.left + "," + self.padding.top + ")")
	.attr("x1",function(d,i){
		var tmpx = self.dataSource.new_Xscale(d)- self.padding.left;
  		if (tmpx > 0 && tmpx < self.dataSource.xRange -self.padding.left){
  			return tmpx;
  		}else{
  			return -1000;
  		}
	})	
	.attr("y1",function(d,i){
		return new_Yscale(self.dataSource.peakintensityShow[i]) ;
	})
	.attr("x2",function(d,i){
		var tmpx = self.dataSource.new_Xscale(d)- self.padding.left;
  		if (tmpx > 0 && tmpx < self.dataSource.xRange -self.padding.left){
  			return tmpx;
  		}else{
  			return -1000;
  		}
	})
	.attr("y2",self.height - self.padding.bottom - self.padding.top)
	.attr("stroke",function(d,i){
		return "black";
	})
	.attr("stroke-width","2")
	.on("mouseover",function(d,i){
		self.dataSource.circleOrPeak = "peak" ;
		self.dataSource.over(this,d,i,self.dataSource.circleOrPeak);
	})
	.on("mouseout",function(d,i){
		self.dataSource.circleOrPeak = "peak" ;
		self.dataSource.out(this,d,self.dataSource.circleOrPeak);
	});
  	updatePeak.exit().remove();
	
	/*	Clear the axis	----------------------------------------------------------*/
    clearAxis(self);
	/*	Add the axis again	------------------------------------------------------*/
    addAxis(self); 
	
	self.circles = self.group_top.append("g").selectAll(".circles");
	addEnvelopes(spectrum);

};