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
						<div class="input-group input-group">
							<span class="input-group-addon">Title:</span> <input type="text"
								class="form-control" id="task-title" name="task-title" value="">
						</div>
						<br>
						<div>
							<input type="hidden" id="upload-db-file" name="upload-db-file"
								value="">
							<table>
								<tr>
									<td><span class="btn btn-primary fileinput-button">
											<i class="icon-plus icon-white"></i> <span>Select
												database file...</span> <input id="dbfile" type="file"
											name="dbfile">
									</span></td>
									<td width="10pt"></td>
									<td><div id="dbfilename"></div></td>
									<td width="10pt"></td>
									<td id="dbtd">
										<div id="dbprogress" class="progress">
											<div id="dbbar" class="progress-bar" role="progressbar"
												style="width: 0%">0%</div>
										</div>
									</td>
									<td width="10pt"></td>
									<td>
										<div id="dbbutton"></div>
									</td>
								</tr>
							</table>
						</div>
						<br />
						<div>
							<input type="hidden" id="upload-sp-file" name="upload-sp-file"
								value="">
							<table>
								<tr>
									<td><span class="btn btn-primary fileinput-button">
											<i class="icon-plus icon-white"></i> <span>Select
												spectrum file...</span> <input id="spfile" type="file"
											name="spfile">
									</span></td>
									<td width="10pt"></td>
									<td><div id="spfilename"></div></td>
									<td width="10pt"></td>
									<td id="sptd">
										<div id="spprogress" class="progress">
											<div id="spbar" class="progress-bar" role="progressbar"
												style="width: 0%">0%</div>
										</div>
									</td>
									<td width="10pt"></td>
									<td>
										<div id="spbutton"></div>
									</td>
								</tr>
							</table>
						</div>
					</div>
				</div>
				<div class="panel panel-default"
					style='padding: 20px 20px 20px 20px; width: 860px'>
					<div class="panel-heading">Search Arguments</div>
					<div class="panel-body"></div>
					<form id="arguments" method="post" action="ArgumentServlet">
						<!-- argument -->
						<input type="hidden" id="upload-task-title"
							name="upload-task-title" value="">
						<div class="panel panel-default"
							style='padding: 20px 20px 20px 20px;'>
							<div class="panel-heading">Parameters</div>
							<div class="panel-body"></div>
							<br />
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
									Variable PTM:</label> <select class="form-control" name="n-ptm"
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
									onblur="checkInt(this,15)">
							</div>
							<br />
							<div class="input-group input-group">
								<label class="col-sm-2 control-label" style="width: 250px">Cysteine
									Protecting Group:</label> <select class="form-control"
									name="cys-protect" style="width: 250px">
									<option value="C0">NONE</option>
									<option value="C57">Carbamidomethylation (C57)</option>
									<option value="C58">Carboxymethylation (C58)</option>
								</select>
							</div>
							<br />
							<div class="input-group input-group">
								<label class="col-sm-2 control-label" style="width: 250px">Decoy
									Database Searching:</label> <input type="checkbox" name="search-type"
									id="stype" value="TARGET+DECOY">
							</div>
							<br />
							<div class="input-group input-group">
								<label class="col-sm-2 control-label" style="width: 250px">Cutoff
									Type:</label> <select class="form-control" name="cutoff-type"
									id="cutoff-type" style="width: 250px">
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
									of Unexpected PTMs:</label> <select class="form-control" name="ptm-num"
									style="width: 250px">
									<option value="0">NONE</option>
									<option value="1">1</option>
									<option value="2" selected="selected">2</option>
								</select>
							</div>
							<br />
							<div class="input-group input-group">
								<label class="col-sm-2 control-label" style="width: 250px">Max
									PTM Mass:</label> <input type="text" class="form-control"
									name="max-ptm" style="width: 250px" value="1000000"
									onblur="checkInt(this,1000000)">
							</div>
							<br />
							<div class="input-group input-group">
								<label class="col-sm-2 control-label" style="width: 250px">
									Generating Function for E-value Computation:</label> <input
									type="checkbox" id="use-gf" name="use-gf">
							</div>
						</div>

						<!-- submit button -->
						<div class="input-group input-group">
							<div class="col-xs-3">
								<input type="submit" id="submitBtn" class="btn btn-primary"
									value="Submit">
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

	<script src="js/jquery-1.9.1.js"></script>
	<script src="js/jquery-ui.js"></script>
	<script src="js/bootstrap.min.js"></script>
	<script src="js/jquery.fileupload.js"></script>
	<script src="js/toppic-upload.js"></script>
	<script src="js/toppic-argument.js"></script>
</body>
</html>
