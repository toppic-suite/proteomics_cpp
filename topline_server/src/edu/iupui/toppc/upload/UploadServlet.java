package edu.iupui.toppc.upload;

import java.io.File;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.Iterator;
import java.util.List;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.servlet.http.HttpSession;

import org.apache.commons.fileupload.FileItem;
import org.apache.commons.fileupload.FileUploadException;
import org.apache.commons.fileupload.disk.DiskFileItemFactory;
import org.apache.commons.fileupload.servlet.ServletFileUpload;

import org.json.JSONArray;
import org.json.JSONObject;

public class UploadServlet extends HttpServlet {
	private static final long serialVersionUID = 1L;

	private static final String UPLOAD_DIRECTORY = "upload";
	private static final int THRESHOLD_SIZE = 1024 * 1024 * 30; // 30MB
	private static final int MAX_FILE_SIZE = 1024 * 1024 * 400; // 400MB
	private static final int MAX_REQUEST_SIZE = 1024 * 1024 * 500; // 500MB

	@SuppressWarnings("rawtypes")
	protected void doPost(HttpServletRequest request,
			HttpServletResponse response) throws ServletException, IOException {
		// checks if the request actually contains upload file
		if (!ServletFileUpload.isMultipartContent(request)) {
			PrintWriter writer = response.getWriter();
			writer.println("Request does not contain upload data");
			writer.flush();
			return;
		}

		DiskFileItemFactory factory = new DiskFileItemFactory();
		factory.setSizeThreshold(THRESHOLD_SIZE);
		factory.setRepository(new File(System.getProperty("java.io.tmpdir")));

		ServletFileUpload upload = new ServletFileUpload(factory);
		upload.setFileSizeMax(MAX_FILE_SIZE);
		upload.setSizeMax(MAX_REQUEST_SIZE);

		HttpSession session = request.getSession(true);
		if (session.getAttribute("time") == null) {
			session.setAttribute("time", System.currentTimeMillis());
		}

		String sessionID = session.getId();

		String basePath = getServletContext().getRealPath("");

		String uploadPath = basePath + File.separator + UPLOAD_DIRECTORY
				+ File.separator + sessionID + session.getAttribute("time");

		String catogory = null, fileName, filePath;

		File uploadDir = new File(basePath + File.separator + UPLOAD_DIRECTORY);
		if (!uploadDir.exists()) {
			uploadDir.mkdir();
		}

		uploadDir = new File(uploadPath);
		if (!uploadDir.exists()) {
			uploadDir.mkdir();
		}

		FileItem files = null;
		PrintWriter writer = response.getWriter();
		response.setContentType("application/json");
		JSONArray json = new JSONArray();
		try {

			List formItems = upload.parseRequest(request);
			Iterator iter = formItems.iterator();

			while (iter.hasNext()) {
				FileItem item = (FileItem) iter.next();

				if (!item.isFormField()) {
					files = item;
				} else {

					catogory = item.getFieldName();

				}
			}

			JSONObject jsono = new JSONObject();

			if (catogory.equals("spec")) {

				fileName = files.getName();
				if (fileName.substring(fileName.lastIndexOf("."),
						fileName.length()).equals(".msalign")) {
					jsono.put("name", fileName);
					filePath = uploadPath + File.separator + fileName;
					session.setAttribute("spec", fileName);
					session.setAttribute("specPath", filePath);
					File storeFile = new File(filePath);
					files.write(storeFile);
				} else {
					jsono.put("name", "Pleaes upload msalign format!");
				}

			} else {
				fileName = files.getName();

				if (fileName.substring(fileName.lastIndexOf("."),
						fileName.length()).equals(".fasta")) {
					jsono.put("name", fileName);
					filePath = uploadPath + File.separator + fileName;
					session.setAttribute("db", fileName);
					session.setAttribute("dbPath", filePath);
					File storeFile = new File(filePath);
					files.write(storeFile);
				} else {
					jsono.put("name", "Pleaes upload fasta format!");
				}
			}
			jsono.put("size", files.getSize());
			jsono.put("url", "UploadServlet?getfile=" + files.getName());
			jsono.put("thumbnail_url",
					"UploadServlet?getthumb=" + files.getName());
			jsono.put("delete_url", "UploadServlet?delfile=" + files.getName());
			jsono.put("delete_type", "GET");
			json.put(jsono);

		} catch (FileUploadException e) {
			throw new RuntimeException(e);
		} catch (Exception e) {
			throw new RuntimeException(e);
		} finally {
			writer.write(json.toString());
			writer.close();
		}

	}
}