$(function() {

	$("#arguments")
			.submit(
					function(event) {
						event.preventDefault();
						if (document.getElementById('task-title').value == "") {
							alert("Title can not be empty!");
							return;
						}

						if (document.getElementById('upload-db-file').value == "") {
							alert("Database file can not be empty!");
							return;
						}
						if (document.getElementById('upload-sp-file').value == "") {
							alert("Spectrum file can not be empty!");
							return;
						}

						if (document.getElementById('cutoff-type').value == "FDR") {
							if (!document.getElementById('search-type').checked) {
								alert("FDR cutoff can only be used with TARGET+DECOY search!");
								return;
							}
						}

						if (!document.getElementById('use-gf').checked) {
							if (document.getElementById('ppm').value != 15
									&& document.getElementById('ppm').value != 10
									&& document.getElementById('ppm').value != 5) {
								alert("Error tolerance can only be 5, 10 or 15 when the generating function approach for E-value computation is not selected!");
								return;
							}
						}

						$("#upload-task-title").val(
								document.getElementById('task-title').value);

						// serialize the form to a set of variables
						var form = $("#arguments");
						var params = form.serialize();

						// send the data to your server via XHR
						$.post(form.attr("action"), params).done(
								function(data) {
									$("#myModal").modal({
										show : true
									});
								}).fail(function(data) {
							// show the form validation errors here
						})
					});
});

function checkInt(d, v) {
	var t = d.value;
	if (t.length <= 0) {
		alert("The parameter can not be blank!");
	} else {
		for (var i = 0; i < t.length; i++) {
			if (t.charAt(i) in [ '0', '1', '2', '3', '4', '5', '6', '7', '8',
					'9' ]) {
			} else {
				alert("The paramter must be a positive integer!");
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
		alert("The parameter must be a number!");
		d.value = "0.01";
		d.focus();
	}
	if (Number(t) < 0) {
		alert("The parameter must be positive!");
		d.value = "0.01";
		d.focus();
	}

}

function showTask() {
	$('#myModal').modal("hide");
	document.getElementById("resultTab").click();
	document.getElementById('resultFrame').src = "./tasklist.jsp";
}

