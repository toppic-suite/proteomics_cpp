<%@ page language="java" contentType="text/html; charset=ISO-8859-1"
	import="edu.iupui.toppic.task.TaskZip" import="java.io.*"
	pageEncoding="ISO-8859-1"%>
<%
	String id = request.getParameter("id").toString();
	String basePath = getServletContext().getRealPath("");
	File download = new File(basePath + File.separator + "download");
	if (!download.exists())
		download.mkdir();
	String zipPath = "download/TASK" + id + ".zip";

	File zipFile = new File(zipPath);
	if (!zipFile.exists()) {
		TaskZip.zipFolder(basePath + File.separator + "process"
				+ File.separator + "TASK" + id, basePath
				+ File.separator + "download" + File.separator + "TASK"
				+ id + ".zip");
	}
%>
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>

<head>
<meta http-equiv="Content-Type" content="text/html; charset=ISO-8859-1">
<title>Zipping Files</title>
<link rel="stylesheet" href="css/bootstrap.min.css">
<link rel="stylesheet" href="css/toppic.css">
</head>

<body onload="window.parent.zipped();">

	<h4>Please download from the link below</h4>
	<br>
	<a href="<%=zipPath%>">TASK<%=id%></a>

	<script src="js/bootstrap.min.js"></script>
</body>

</html>
