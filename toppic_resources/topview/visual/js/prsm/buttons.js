/*	On-click Actions to show spectrum graph	*/
function buttons(){
	/*	Invocation on click of Mono M/z at the table	*/
	$( ".peakRows" ).click(function() {
		document.getElementById("ms1svg").style.display = "block";
		document.getElementById("spectrum_help").style.display = "block";
		/*	get Mono M/z value till 3 decimal values	*/
		let peak_value = parseFloat(this.innerHTML).toFixed(3) ;
    ms2_graph.redraw(peak_value);
	});
	/*	Invocation on click of show spectrum button		*/
  /*
	$("#show_ms1_spectrum" ).click(function() {
		$("#show_ms2_spectrum" ).text("Show Ms2");

		let peak_data = new PeakData();
    let ms1_peak_list = peak_data.getPeakData(ms1_data);
		let ms1_envelope_list = peak_data.getEnvelopeData(ms1_data);
		ms1_graph = addSpectrum("ms1svg", ms1_peak_list,
		  								ms1_envelope_list,null);
		  
		 if($.trim($(this).text()) === 'Show Ms1')
		 {
			 document.getElementById("ms1svg").style.display = "block";
			 document.getElementById("spectrum_help").style.display = "block";
			 //document.getElementById("a_show_spectrum").href = "#"; 
			 $(this).text('Hide Ms1');
		 }
		 else
		 {
			 $(this).text('Show Ms1'); 
			 document.getElementById("ms1svg").style.display = "none";
			 document.getElementById("spectrum_help").style.display = "none";
			 //document.getElementById("download_spectrum").style.display = "none";
			 //document.getElementById("spectrum_help").style.display = "none";
		 }
	});
  */
	$("#show_ms2_spectrum" ).click(function() {
		//$("#show_ms1_spectrum" ).text("Show Ms1");

    /*
		let peak_data = new PeakData();
    let ms1_peak_list = peak_data.getPeakData(ms2_data);
		let ms1_envelope_list = peak_data.getEnvelopeData(ms2_data);
		ms1_graph = addSpectrum("ms1svg", ms1_peak_list,
		  								ms1_envelope_list,null);
                      */
		 
		 if($.trim($(this).text()) === 'Show Spectrum')
		 {
			 document.getElementById("ms1svg").style.display = "block";
			 document.getElementById("spectrum_help").style.display = "block";
			 document.getElementById("graph_download").style.display = "block";
			 //document.getElementById("a_show_spectrum").href = "#"; 
			 $(this).text('Hide Spectrum');
		 }
		 else
		 {
			 $(this).text('Show Spectrum'); 
			 document.getElementById("ms1svg").style.display = "none";
			 document.getElementById("spectrum_help").style.display = "none";
			 document.getElementById("graph_download").style.display = "none";
			 //document.getElementById("download_spectrum").style.display = "none";
			 //document.getElementById("spectrum_help").style.display = "none";
		 }
	});
	
	let specParameters = new SpectrumParameters();
	let spectrumDownload = new SpectrumDownload();
	let x,y;
	d3.select("#graph_download_png").on("click",function(){
		x = d3.event.pageX;
		y = d3.event.pageY + 80;
		//function in prsmtohtml
		popupnamewindow("png", "ms1svg",x,y)
	})
	d3.select("#graph_download_svg").on("click",function(){
		x = d3.event.pageX;
		y = d3.event.pageY + 40;
		popupnamewindow("png", "ms1svg",x,y)
	})
	d3.select("#download_popup_png").on("click",function(){
		x = d3.event.pageX;
		y = d3.event.pageY;
		popupnamewindow("png", "popupspectrum",x,y)
	})
	d3.select("#download_popup_svg").on("click",function(){
		x = d3.event.pageX;
		y = d3.event.pageY;
		popupnamewindow("png", "popupspectrum",x,y)
	})

	$("#precursormz").click(function(){
		let prec_mz = $("#precursormz").html();
    ms1_graph.redraw(prec_mz);
    $("#spectrumpop").draggable({
      appendTo: "body"
    });
	});

	$("#spectrum_help").click(function(){
    $("#spectrumHelp").draggable({
      appendTo: "body"
    });
	});

}
