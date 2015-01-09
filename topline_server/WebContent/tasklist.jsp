<%@ page language="java" contentType="text/html; charset=ISO-8859-1"
	import="java.io.*" import="java.util.*" import="edu.iupui.toppc.util.*"
	import="java.text.DecimalFormat" pageEncoding="ISO-8859-1"%>
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<%
	String servpath = request.getContextPath();
	String basePath = request.getScheme() + "://"
	+ request.getServerName() + ":" + request.getServerPort()
	+ servpath + "/";
%>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=ISO-8859-1">
<title>Insert title here</title>
<link rel="stylesheet" href="<%=basePath%>css/bootstrap.min.css">
<script src="<%=basePath%>js/jquery.min.js"></script>
<script src="<%=basePath%>js/bootstrap.min.js"></script>
<script type="text/javascript">
	function deleteTask(id) {
		var arr = "Do you want to delete Task " + id + " ?";

		if (confirm(arr)) {
			document.getElementById('delName').value = id;
			document.getElementById(id).style.display = "none";
			document.getElementById('deleteFrm').submit();
		}
	}

	function stopTask(id) {
		var arr = "Do you want to stop Task " + id + " ?";

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

	function downloadTask(id) {

		document.getElementById("downloadFrm").src = "./download.jsp?id=" + id;

		$("#downloadModal").modal({
			show : true
		});
	}

	function zipped() {
		document.getElementById("zipping").innerHTML = "&nbsp;&nbsp;";
	}
</script>

<style type="text/css">
.table th {
	text-align: center;
}

.modal-dialog {
	position: relative;
	width: auto;
	margin: 0px;
}

#downloadModal {
	top: 100px;
	width: 500px;
	height: 370px;
	overflow: hidden;
	margin: 0 auto;
}
</style>

</head>
<body>
	<iframe name="hiddenFr" class="hide"></iframe>
	<table class="table table-striped" style="width: 820px;">
		<thead>

			<tr>
				<th align="center" width="10%">Task ID</th>
				<th align="center" width='15%'>Title</th>
				<th align="center">Submission Time</th>
				<th align="center">Running Time</th>
				<th align="center">Progress</th>
				<th><button type='button' class='btn btn-success btn-xs'
						onclick="window.location.reload()">&nbsp;&nbsp;Refresh
						All&nbsp;&nbsp;</button></th>
			</tr>
		</thead>
		<tbody>
			<%
				ArrayList<Task> taskList = TaskList.getTask(getServletContext()
						.getRealPath("")
						+ File.separator
						+ "log"
						+ File.separator
						+ "tasklist.xml");
				DecimalFormat df = new DecimalFormat("#.##");

				if (taskList.size() > 0) {
					for (int i = 0; i < taskList.size(); i++) {
						String status = taskList.get(i).getStatus();
						String id = taskList.get(i).getID();

						out.println("<tr id ='" + taskList.get(i).getID()
								+ "'><td align=\'center\'>"
								+ taskList.get(i).getID() + "</td>");

						out.println("<td align='center'>"
								+ taskList.get(i).getTitle() + "</td>");

						out.println("<td align='center'>"
								+ taskList.get(i).getSubmitTime() + "</td>");
						if (status.equals("finished")) {
							out.println("<td align='center'>"
									+ taskList.get(i).getRuntime() + "</td>");
							out.println("<td align='center'>100% (finished)</td><td align='center'>");
							out.println("<button type='button' class='btn btn-success btn-xs' onclick=downloadTask('"
									+ taskList.get(i).getID()
									+ "')>&nbsp;&nbsp;Download&nbsp;&nbsp;</button>");
							out.println("<button type='button' class='btn btn-warning btn-xs' onclick=deleteTask('"
									+ taskList.get(i).getID()
									+ "')>&nbsp;&nbsp;Delete&nbsp;&nbsp;</button>");
							out.println("</td></tr>");
						} else if (status.equals("waiting")) {
							double per = 0.0;
							out.println("<td align='center' id ='runtime"
									+ taskList.get(i).getID()
									+ "'>--:--</td><td id='progress"
									+ taskList.get(i).getID()
									+ "' align='center'>waiting</td>");
							out.println("<td align='center' id='BTN"
									+ taskList.get(i).getID() + "'>");
							out.println("<button type='button' class='btn btn-success btn-xs' onclick=refreshRow('"
									+ taskList.get(i).getID()
									+ "')>&nbsp;&nbsp;&nbsp;Refresh&nbsp;&nbsp;&nbsp;</button>");
							out.println("<button type='button' class='btn btn-warning btn-xs' onclick=deleteTask('"
									+ taskList.get(i).getID()
									+ "')>&nbsp;&nbsp;Delete&nbsp;&nbsp;</button>");
							out.println("</td></tr>");
						} else if (status.equals("running")) {
							double per = TaskList.getPercentage(getServletContext()
									.getRealPath("")
									+ File.separator
									+ "process"
									+ File.separator
									+ "TASK"
									+ id
									+ File.separator
									+ "log.txt");
							out.println("<td align='center' id='runtime"
									+ taskList.get(i).getID() + "'>"
									+ taskList.get(i).getRuntime() + "</td>");
							out.println("<td id='progress"
									+ taskList.get(i).getID() + "' align='center'>"
									+ df.format(per) + "% (" + status + ")</td>");
							out.println("<td align='center' id='BTN"
									+ taskList.get(i).getID() + "'>");
							out.println("<button type='button' id='BtnRefresh"
									+ taskList.get(i).getID()
									+ "' class='btn btn-success btn-xs' onclick=refreshRow('"
									+ taskList.get(i).getID()
									+ "')>&nbsp;&nbsp;&nbsp;Refresh&nbsp;&nbsp;&nbsp;</button>");
							out.println("<button type='button' id ='BtnStop"
									+ taskList.get(i).getID()
									+ "' class='btn btn-warning btn-xs' onclick=stopTask('"
									+ taskList.get(i).getID()
									+ "')>&nbsp;&nbsp;&nbsp;Stop&nbsp;&nbsp;&nbsp;</button>");
							out.println("</td></tr>");
						} else if (status.equals("stopped")) {
							double per = TaskList.getPercentage(getServletContext()
									.getRealPath("")
									+ File.separator
									+ "process"
									+ File.separator
									+ "TASK"
									+ id
									+ File.separator
									+ "log.txt");

							out.println("<td align='center'>"
									+ taskList.get(i).getRuntime() + "</td>");
							out.println("<td align='center'>" + df.format(per)
									+ "% (stopped)</td>");
							out.println("<td align='center'>");
							out.println("<button type='button' class='btn btn-success btn-xs' onclick=downloadTask('"
									+ taskList.get(i).getID()
									+ "')>&nbsp;&nbsp;Download&nbsp;&nbsp;</button>");
							out.println("<button type='button' class='btn btn-warning btn-xs' onclick=deleteTask('"
									+ taskList.get(i).getID()
									+ "')>&nbsp;&nbsp;Delete&nbsp;&nbsp;</button>");
							out.println("</td></tr>");
						}
					}
				}
			%>
		</tbody>
	</table>
	<form id="deleteFrm" action="TaskDeleteServlet" method="post"
		target="hiddenFr">
		<input type="hidden" name="del" id="delName" />
	</form>

	<form id="stopFrm" action="TaskStopServlet" method="post"
		target="hiddenFr">
		<input type="hidden" name="stop" id="stopName" />
	</form>

	<div class="modal fade" id="downloadModal">
		<div class="modal-dialog">
			<div class="modal-content">
				<div class="modal-header">
					<button type="button" class="close" data-dismiss="modal"
						aria-hidden="true">&times;</button>
					<h4 class="modal-title" id="zipping">Zipping files, please
						wait......</h4>
				</div>
				<div class="modal-body">
					<div>
						<iframe id="downloadFrm" src="" width="450px" height="150px"
							frameBorder="0"></iframe>
					</div>
				</div>
				<div class="modal-footer">
					<button type="button" class="btn btn-default" data-dismiss="modal">Close</button>
				</div>
			</div>
		</div>
	</div>
	<script
		src="//ajax.googleapis.com/ajax/libs/jquery/1.10.2/jquery.min.js"></script>
	<script src="js/vendor/jquery.ui.widget.js"></script>
	<script src="js/tmpl.min.js"></script>
	<script src="js/load-image.all.min.js"></script>
	<script src="js/canvas-to-blob.min.js"></script>
	<script src="js/bootstrap.min.js"></script>
	<script src="js/jquery.blueimp-gallery.min.js"></script>
	<script src="js/jquery.iframe-transport.js"></script>
	<script src="js/jquery.fileupload.js"></script>
	<script src="js/jquery.fileupload-fp.js"></script>
	<script src="js/jquery.fileupload-ui.js"></script>
	<script src="js/locale.js"></script>
	<script src="js/main.js"></script>
</body>
</html>