<%@ page session="true"%>
<!DOCTYPE html>
<%
	String path = request.getContextPath();
	String basePath = request.getScheme() + "://"
			+ request.getServerName() + ":" + request.getServerPort()
			+ path + "/";
%>
<html>
<head>
<title>TopPIC</title>
<link rel="stylesheet" href="css/bootstrap.min.css">
<link rel="stylesheet" href="css/toppic.css">
<link rel="stylesheet" href="css/blueimp-gallery.min.css">
<link rel="stylesheet" href="css/jquery.fileupload-ui.css">
</head>
<body>


	<div class="container" id="content">
		<div class="header">

			<div class="heading" style="color: #0099DD;">TopPIC</div>
		</div>
	</div>

	<div class="container bs-docs-container">

		<ul class="nav nav-tabs">
			<li class="active"><a href="#toppc" data-toggle="tab"
				onclick="refreshtab();">TopPIC</a></li>
			<li><a href="#result" data-toggle="tab" id="resultTab"
				onclick="document.getElementById('resultFrame').src='./tasklist.jsp';">Results</a></li>
		</ul>

		<div class="tab-content">

			<div class="tab-pane active" id="toppc" style="width: 900px">
				<br>
				<iframe name="hiddenFrame" class="hide"></iframe>
				<div class="panel panel-default"
					style='padding: 20px 20px 20px 20px; width: 860px'>
					<div class="panel-heading">Search Information</div>
					<div class="panel-body">
						<form method="post" target="hiddenFrame">
							<div class="input-group input-group">
								<span class="input-group-addon">Title:</span> <input type="text"
									class="form-control" id="title" name="title" value="">
							</div>
						</form>
						<br>

						<form id="database" class="fileupload" action="UploadServlet"
							method="POST" enctype="multipart/form-data">
							<div class="row">
								<div class="col-xs-3">
									<table class="fileupload-buttonbar">
										<tr>
											<td><input type="hidden" name="database" id="db"
												value="" /></td>
											<td><span class="btn btn-primary fileinput-button">
													<i class="icon-plus icon-white"></i><span>Add
														database file...</span> <input type="file" id="dbPath"
													name="files[]">
											</span></td>
										</tr>
									</table>
								</div>
								<div class="col-xs-9">
									<table role="presentation">
										<tbody class="files" data-toggle="modal-gallery"
											data-target="#modal-gallery"></tbody>
									</table>
								</div>
							</div>
						</form>
						<br />
						<form id="spectra" class="fileupload" action="UploadServlet"
							method="POST" enctype="multipart/form-data">
							<div class="row">
								<div class="col-xs-3">
									<table class="fileupload-buttonbar">
										<tr>
											<td><input type="hidden" name="spec" id="sp" /></td>
											<td><span class="btn btn-primary fileinput-button">
													<i class="icon-plus icon-white"></i><span>Add
														spectrum file...</span> <input type="file" id="specPath"
													name="files[]">
											</span></td>
										</tr>
									</table>
								</div>
								<div class="col-xs-9">
									<table role="presentation">
										<tbody class="files" data-toggle="modal-gallery"
											data-target="#modal-gallery"></tbody>
									</table>
								</div>
							</div>
						</form>
					</div>
				</div>
				<div class="panel panel-default"
					style='padding: 20px 20px 20px 20px; width: 860px'>
					<div class="panel-heading">Search Arguments</div>
					<div class="panel-body"></div>
					<form id="arguments" method="post" action="ArgumentServlet"
						target="hiddenFrame">
						<input type="hidden" class="form-control" id="proTitle"
							name="proTitle" />
						<div class="input-group input-group">
							<label class="col-sm-2 control-label" style="width: 250px">Fragmentation
								Method:</label> <select class="form-control" name="fragment"
								style="width: 250px">
								<option>FILE</option>
								<option>CID</option>
								<option>ETD</option>
								<option>HCD</option>
							</select>
						</div>
						<br />
						<div class="input-group input-group">
							<label class="col-sm-2 control-label" style="width: 250px">N-Terminal
								Variable PTM:</label> <select class="form-control" name="nptm"
								style="width: 250px">
								<option value="nptm1">NONE</option>
								<option value="nptm2">NME</option>
								<option value="nptm3" selected="selected">NME and
									N-Terminal Acetylation</option>
							</select>
						</div>
						<br />
						<div class="input-group input-group">
							<label class="col-sm-2 control-label" style="width: 250px">Error
								Tolerance (PPM):</label> <input type="text" class="form-control"
								name="ppm" id="ppm" style="width: 250px" value="15"
								check-type="required" onblur="checkInt(this,15)">
						</div>
						<br />
						<div class="input-group input-group">
							<label class="col-sm-2 control-label" style="width: 250px">Cysteine
								Protecting Group:</label> <select class="form-control" name="cprotect"
								style="width: 250px">
								<option value="C0">NONE</option>
								<option value="C57">Carbamidomethylation (C57)</option>
								<option value="C58">Carboxymethylation (C58)</option>
							</select>
						</div>
						<br />
						<div class="input-group input-group">
							<label class="col-sm-2 control-label" style="width: 250px">Decoy
								Database Searching:</label> <input type="checkbox" name="stype"
								id="stype" value="TARGET+DECOY">
						</div>
						<br />
						<div class="input-group input-group">
							<label class="col-sm-2 control-label" style="width: 250px">Cutoff
								Type:</label> <select class="form-control" name="cutofftype"
								id="cutofftype" style="width: 250px">
								<option>EVALUE</option>
								<option>FDR</option>
							</select>
						</div>
						<br />
						<div class="input-group input-group">
							<label class="col-sm-2 control-label" style="width: 250px">Cutoff
								Value:</label> <input type="text" class="form-control" name="cutoff"
								style="width: 250px" value="0.01" onblur="checkNum(this)">
						</div>
						<br />
						<div class="input-group input-group">
							<label class="col-sm-2 control-label" style="width: 250px">Number
								of Unexpected PTMs:</label> <select class="form-control" name="ptm"
								style="width: 250px">
								<option value="0">NONE</option>
								<option value="1">1</option>
								<option value="2" selected="selected">2</option>
							</select>
						</div>
						<br />
						<div class="input-group input-group">
							<label class="col-sm-2 control-label" style="width: 250px">Max
								PTM Mass:</label> <input type="text" class="form-control" name="mptm"
								style="width: 250px" value="1000000"
								onblur="checkInt(this,1000000)">
						</div>
						<br />
						<div class="input-group input-group">
							<label class="col-sm-2 control-label" style="width: 250px">
								Generating Function for E-value Computation:</label> <input
								type="checkbox" name="table" id="table" value="use_gf">
						</div>
						<br />
						<div class="input-group input-group">
							<div class="col-xs-3">
								<button type="button" id="submitBtn"
									data-loading-text="Submit..." class="btn btn-primary"
									onclick="submitArgument();">Submit</button>
							</div>
						</div>


					</form>
				</div>
			</div>
			<div class="tab-pane" id="result" style="width: 890px">
				<br />
				<div class="panel panel-default"
					style='padding: 20px 20px 20px 20px; width: 860px'>
					<iframe id="resultFrame" name="file" src="./tasklist.jsp"
						width="820px" height="877px"></iframe>
				</div>
			</div>
		</div>

		<div class="modal fade" id="myModal">
			<div class="modal-dialog">
				<div class="modal-content">
					<div class="modal-body" align="center">
						<button type="button" class="close" data-dismiss="modal"
							aria-hidden="true">&times;</button>
						<h3 class="modal-title">Task submitted</h3>
						<br /> <br />
						<button type="button" class="btn btn-default" data-dismiss="modal"
							onclick="window.location.reload();Session.Clear();">Submit
							Another</button>
						<button type="button" class="btn btn-primary"
							onclick="showTask();">See Results</button>
					</div>
				</div>
			</div>
		</div>

	</div>
	<script id="template-upload" type="text/x-tmpl">
		{% for (var i=0, file; file=o.files[i]; i++) { %}
    		<tr class="template-upload fade">
        	<td class="name"><span>{%=file.name%}</span></td>
        	{% if (file.error) { %}
            	<td class="error" colspan="2"><span class="label label-important">{%=locale.fileupload.error%}</span> {%=locale.fileupload.errors[file.error] || file.error%}</td>
        	{% } else if (o.files.valid && !i) { %}
            	<td>
                	<div class="progress progress-success progress-striped active" role="progressbar" aria-valuemin="0" aria-valuemax="100" aria-valuenow="0"><div class="progress-bar" style="width:0%;"></div></div>
            	</td>
            	<td class="start">{% if (!o.options.autoUpload) { %}
                	<button class="btn btn-primary">
                    	<i class="icon-upload icon-white"></i>
                    	<span>Upload</span>
                	</button>
            {% } %}</td>
        	{% } else { %}
            	<td colspan="2"></td>
        	{% } %}
        	<td class="cancel">{% if (!i) { %}
            	<button class="btn btn-warning">
                <i class="icon-ban-circle icon-white"></i>
                <span>{%=locale.fileupload.cancel%}</span>
            	</button>
        	{% } %}</td>
    	</tr>
	{% } %}
	</script>
	<!-- The template to display files available for download -->
	<script id="template-download" type="text/x-tmpl">
		{% for (var i=0, file; file=o.files[i]; i++) { %}
    		<tr class="template-download fade">
        		{% if (file.error) { %}
            		<td></td>
            		<td class="name"><span>{%=file.name%}</span></td>
            		<td class="error" colspan="2"><span class="label label-important">{%=locale.fileupload.error%}</span> {%=locale.fileupload.errors[file.error] || file.error%}</td>
        		{% } else { %}
            		<td class="name">
                		{%=file.name%}
            		</td>
            		<td colspan="2"></td>
        		{% } %}
        	<td class="delete">
            	<button class="btn btn-danger" data-type="{%=file.delete_type%}" data-url="{%=file.delete_url%}">
            	    <i class="icon-trash icon-white"></i>
            	    <span>{%=locale.fileupload.destroy%}</span>
            	</button>
        	</td>
    	</tr>
		{% } %}
</script>
	<script src="js/jquery.min.js"></script>
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
	<script src="js/toppic-argument.js"></script>
</body>
</html>
