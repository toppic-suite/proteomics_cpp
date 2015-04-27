<%@ page language="java" contentType="text/html; charset=ISO-8859-1"
	import="java.io.*" import="java.util.*"
	import="edu.iupui.toppic.task.*" import="java.text.DecimalFormat"
	pageEncoding="ISO-8859-1"%>
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
<link rel="stylesheet" href="css/bootstrap.min.css">
<link rel="stylesheet" href="css/result.css">

</head>
<body>
	<iframe name="hiddenFr" class="hide"></iframe>
	<table class="table table-striped" style="width: 815px;">
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
						+ "tasks"
						+ File.separator + "tasklist.xml");
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
	<script src="js/jquery-1.9.1.js"></script>
	<script src="js/jquery-ui.js"></script>
	<script src="js/bootstrap.min.js"></script>
	<script src="js/toppic-results.js"></script>

</body>
</html>