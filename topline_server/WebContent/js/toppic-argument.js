var submitted = false;
var msalign_check_id;
var cur_process = 0;

function submitArgument() {

	document.getElementById('proTitle').value = document
			.getElementById('title').value;

	if (document.getElementById('title').value == "") {
		alert("Title can not be empty!");
		return;
	}

	if (document.getElementById('db').value == "") {
		alert("Please upload database file!");
		return;
	}

	if (document.getElementById('sp').value == "") {
		alert("Please upload spectrum file!");
		return;
	}

	if (document.getElementById('cutofftype').value == "FDR") {
		if (!document.getElementById('stype').checked) {
			alert("FDR cutoff can only be used with TARGET+DECOY search!");
			return;
		}
	}

	if (!document.getElementById('table').checked) {
		if (document.getElementById('ppm').value != 15
				&& document.getElementById('ppm').value != 10
				&& document.getElementById('ppm').value != 5) {
			alert("Error tolerance can only be 15, 10 or 5 when not using generating function!");
			return;
		}
	}

	var formobject = document.getElementById("arguments");
	formobject.submit();
	submitted = true;
	$("#myModal").modal({
		show : true
	});

}

function checkInt(d, v) {
	var t = d.value;
	if (t.length <= 0) {
		alert("can not be blank!");
	} else {
		for (var i = 0; i < t.length; i++) {
			if (t.charAt(i) in [ '0', '1', '2', '3', '4', '5', '6', '7', '8',
					'9' ]) {
			} else {
				alert("must be positive");
				d.value = v;
				d.focus();
				break;
			}
		}
	}
}

function checkNum(d) {
	var t = d.value;
	if (isNaN(Number(t))) {
		alert("must be number!");
		d.value = "0.01";
		d.focus();
	}
	if (Number(t) < 0) {
		alert("must be positive!");
		d.value = "0.01";
		d.focus();
	}

}

function showTask() {
	$('#myModal').modal("hide");
	document.getElementById("resultTab").click();
	document.getElementById('resultFrame').src = "./tasklist.jsp";
	document.getElementById('title').value = '';
	document.getElementById('sp').value = '';
	document.getElementById('db').value = '';
}

function refreshtab() {
	if (submitted) {
		window.location.reload();
	}
}