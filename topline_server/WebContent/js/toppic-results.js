function deleteTask(id) {
	var arr = "Are you sure you want to delete Task " + id + "?";

	if (confirm(arr)) {
		document.getElementById('delName').value = id;
		document.getElementById(id).style.display = "none";
		document.getElementById('deleteFrm').submit();
	}
}

function refreshRow(id) {

	htmlobj = $.ajax({
		url : "TaskInfoServlet?id=" + id,
		async : false
	});

	var arr = htmlobj.responseText.split(',');
	var per = arr[0];
	var status = arr[1];
	var runtime = arr[2];

	if (status == "running") {
		document.getElementById("runtime" + id).innerHTML = runtime;
		document.getElementById("progress" + id).innerHTML = per
				+ "% (running)";
		document.getElementById("BTN" + id).innerHTML = "<button type='button' id='BtnRefresh"
				+ id
				+ "' class='btn btn-success btn-xs' onclick=refreshRow('"
				+ id
				+ "')>&nbsp;&nbsp;&nbsp;Refresh&nbsp;&nbsp;&nbsp;</button>&nbsp;"
				+ "<button type='button' id ='BtnStop"
				+ id
				+ "' class='btn btn-warning btn-xs' onclick=stopTask('"
				+ id
				+ "')>&nbsp;&nbsp;&nbsp;Stop&nbsp;&nbsp;&nbsp;</button></td></tr>";
	} else if (status == "finished") {
		document.getElementById("runtime" + id).innerHTML = runtime;
		document.getElementById("progress" + id).innerHTML = "100%(finished)";
		document.getElementById("BTN" + id).innerHTML = "<button type='button' class='btn btn-success btn-xs' onclick=downloadTask('"
				+ id
				+ "')>&nbsp;&nbsp;Download&nbsp;&nbsp;</button>&nbsp;"
				+ "<button type='button' class='btn btn-warning btn-xs' onclick=deleteTask('"
				+ id
				+ "')>&nbsp;&nbsp;Delete&nbsp;&nbsp;</button></td></tr>";
	}
}

function stopTask(id) {
	var arr = "Are you sure you want to stop Task " + id + "?";

	if (confirm(arr)) {
		document.getElementById('stopName').value = id;
		document.getElementById('stopFrm').submit();
		htmlobj = $.ajax({
			url : "TaskInfoServlet?id=" + id,
			async : false
		});

		var arr = htmlobj.responseText.split(',');
		var per = arr[0];
		var status = arr[1];
		var runtime = arr[2];
		document.getElementById("runtime" + id).innerHTML = runtime;
		document.getElementById("progress" + id).innerHTML = per
				+ "%(stopped)";

		document.getElementById("BTN" + id).innerHTML = "<button type='button' class='btn btn-success btn-xs' onclick=downloadTask('"
				+ id
				+ "')>&nbsp;&nbsp;Download&nbsp;&nbsp;</button>&nbsp;"
				+ "<button type='button' class='btn btn-warning btn-xs' onclick=deleteTask('"
				+ id + "')>&nbsp;&nbsp;Delete&nbsp;&nbsp;</button>";

	}
}

function downloadTask(id) {

	document.getElementById("downloadFrm").src = "./download.jsp?id=" + id;

	$("#downloadModal").modal({
		show : true
	});
}

function zipped() {
	document.getElementById("zipping").innerHTML = "&nbsp;&nbsp;";
}


