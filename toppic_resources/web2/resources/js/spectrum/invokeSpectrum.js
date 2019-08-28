/*	Spectrum start point */
this.addSpectrum = function(monoMZ){
	let peakdata = new PeakData();
	peakdata.getPeakData(spectrum_data);
	peakdata.getEnvelopeData(spectrum_data);
	let specParameters = new SpectrumParameters(peakdata);
	let listSize = peakdata.peak_list.length;
	let minMzData = peakdata.peak_list[0].mz;
	let maxMzData = peakdata.peak_list[listSize-1].mz;
	peakdata.peak_list.sort(function(x,y){
		return d3.ascending(x.intensity, y.intensity);
	})
	let maxIntensity = peakdata.peak_list[listSize-1].intensity;
	let minIntensity = peakdata.peak_list[0].intensity;
	let currminMz = minMzData ;
	let currmaxMz = maxMzData ;
	let currentMaxIntensity = maxIntensity ;
	if(monoMZ != null)
	{
		currentMaxIntensity = 0 ;
		monoMZ = parseInt(monoMZ);
		currminMz = monoMZ - specParameters.onClickMassAdjacentRange ;
		currmaxMz = monoMZ + specParameters.onClickMassAdjacentRange ;
		for(let i = 0; i<listSize ; i++)
		{
			if(peakdata.peak_list[i].mz > currminMz && peakdata.peak_list[i].mz < currmaxMz)
			{
				if(peakdata.peak_list[i].intensity > currentMaxIntensity)
				{
					currentMaxIntensity = peakdata.peak_list[i].intensity;
				}
			}
		}
	}
	specParameters.initScale(currminMz,currmaxMz,maxIntensity,minIntensity,minMzData,maxMzData,currentMaxIntensity);
	console.log("peakdata : ",peakdata);
	let spectrumgraph = new SpectrumGraph("#ms1svg",specParameters,peakdata);

  return spectrumgraph;
}

