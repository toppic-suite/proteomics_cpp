//receives file name to parse and replace "" and [] to make it same as the result from original JSON.parse function
function parsePeakMass(dataName){
	let data = window.localStorage.getItem(dataName);
	
	console.log(data == null)
	
	if (data != "" && data != null){
		data = data.replace("[", "")
		data = data.replace("]", "")
		data = data.replace(/,/g, "\n");
		data = data.replace(/"/g, "");
	}
	return data;
}
function parseSeq(dataName){
	let data = window.localStorage.getItem(dataName);
	
	if (data != "" && data != null){
		data = data.replace("[", "")
		data = data.replace("]", "")
		data = data.replace(/"/g, "");
	}
	return data;
}
function parsePTM(dataName){
	//ptm is an object inside array, so to preserve its structure, using JSON parse
	//it is relatively small to other lists so the performance should not deteriorate much
	let data = JSON.parse(window.localStorage.getItem(dataName));
	return data;
}

function parseUnknowmassList(dataName){
	let data = JSON.parse(window.localStorage.getItem(dataName));
	return data;
}
function parsePrecursorMass(dataName){
	let data = parseFloat(JSON.parse(window.localStorage.getItem(dataName)));
	return data;
}
