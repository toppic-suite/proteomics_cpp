$(function() {
  /* database file upload */
  var dbSelected = false;
  $("#dbprogress").hide();
  $('#dbfile').fileupload({
    url: "UploadServlet",
    maxFileSize: 1000000000,  
    dataType: 'json',
    formData: {database: ''},
    add: function (e, data) {
      //data.formData={database:''};
      var format = "none";
      var fileName = data.files[0].name;
      var index = fileName.lastIndexOf(".");
      if(index > 0){
        format = fileName.substring(index+1);
        format = format.toLowerCase();
      }
      if (format != "fasta") {
        alert("Please select a fasta file!");
      }
      else {
        $("#upload-db-file").val("");
        $("#dbfilename").html(data.files[0].name);
        $("#dbtd").css(
          'width', '300pt'
          );
        $("#dbprogress").show();
        $("#dbbar").css(
            'width', 0 + '%'
            );
        data.context = $('<button class="btn btn-primary"/>').text('Upload')
          .click(function () {
            $(this).hide();
            data.submit();
          });
        $("#dbbutton").html(data.context);
        dbSelected = true;
        
      }
  	
    },
    
    done: function (e, data) {
      if (dbSelected) {
        $('#debug').append('completed');
        $("#dbprogress").hide();
        $("#dbtd").css(
            'width', '0pt'
            );
        $("#dbfilename").html(data.result[0].name);
        $("#upload-db-file").val(data.result[0].name);
        data.context = $('<button class="btn btn-danger"/>').text('Delete')
          .appendTo($("#dbbutton"))
          .click(function () {
            $(this).hide();
            $("#dbfilename").html("");
            $("#upload-db-file").val("");
             $("#dbbar").css(
              'width', 0 + "%"
              );
            dbSelected = false;
          });
      }
    },
    
    progressall: function (e, data) {
      var progress = parseInt(data.loaded / data.total * 100, 10);
      $('#debug').append('uploading');
      $('#debug').append(progress);
      $("#dbbar").css(
          'width', progress + '%'
          );
      $("#dbbar").html(progress + '%'
          );
    }
    
  });

  /* spectrum file upload */
  var spSelected = false;
  $("#spprogress").hide();
  $('#spfile').fileupload({
    url: "UploadServlet",
    maxFileSize: 1000000000,  
    dataType: 'json',
    formData: {spec: ''},
    add: function (e, data) {
      var format = "none";
      var fileName = data.files[0].name;
      var index = fileName.lastIndexOf(".");
      if(index > 0){
        format = fileName.substring(index+1);
        format = format.toLowerCase();
      }
      if (format != "msalign") {
        alert("Please select a msalign file!");
      }
      else {
        $("#spfilename").html(data.files[0].name);
        $("#upload-sp-file").val("");
        $("#sptd").css(
          'width', '300pt'
          );
        $("#spprogress").show();
        $("#spbar").css(
            'width', 0 + '%'
            );
        data.context = $('<button class="btn btn-primary"/>').text('Upload')
            .click(function () {
            $(this).hide();
            data.submit();
          });
        $("#spbutton").html(data.context);
        spSelected = true;
      }
    },
    done: function (e, data) {
      if (spSelected) {
        $('#debug').append('completed');
        $("#spprogress").hide();
        $("#sptd").css(
            'width', '0pt'
            );
        $("#spfilename").html(data.result[0].name);
        $("#upload-sp-file").val(data.result[0].name);
        data.context = $('<button class="btn btn-danger"/>').text('Delete')
          .appendTo($("#spbutton"))
          .click(function () {
            $(this).hide();
            $("#spfilename").html("");
            $("#upload-sp-file").val("");
            $("#spbar").css(
              'width', 0 + "%"
              );
            spSelected = false;
          });
      }
    },
    progressall: function (e, data) {
      var progress = parseInt(data.loaded / data.total * 100, 10);
      $('#debug').append('uploading');
      $('#debug').append(progress);
      $("#spbar").css(
          'width', progress + '%'
          );
      $("#spbar").html(progress + '%'
          );
    }
  });
});
