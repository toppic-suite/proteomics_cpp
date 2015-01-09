<%@ page language="java" contentType="text/html; charset=ISO-8859-1"
	import="edu.iupui.toppc.util.*" import="java.io.*"
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
		Zip.zipFolder(basePath + File.separator + "process"
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
<link rel="stylesheet"
	href="//netdna.bootstrapcdn.com/bootstrap/3.0.3/css/bootstrap.min.css">
<link rel="stylesheet" href="css/style.css">
<link rel="stylesheet"
	href="http://blueimp.github.io/Gallery/css/blueimp-gallery.min.css">
<link rel="stylesheet" href="css/jquery.fileupload-ui.css">

</head>
<style type="text/css">
</style>
<body onload="window.parent.zipped();">

	<h4>Please download from the link below</h4>
	<br>
	<a href="<%=zipPath%>">TASK<%=id%></a>

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
