package edu.iupui.toppic.servlet;

import java.io.File;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.Iterator;
import java.util.List;

import javax.servlet.ServletException;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.servlet.http.HttpSession;

import org.apache.commons.fileupload.FileItem;
import org.apache.commons.fileupload.disk.DiskFileItemFactory;
import org.apache.commons.fileupload.servlet.ServletFileUpload;
import org.json.JSONArray;
import org.json.JSONObject;

@WebServlet("/UploadServlet")
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

		String category = null;
		String fileName, filePath;

		File uploadDir = new File(basePath + File.separator + UPLOAD_DIRECTORY);
		if (!uploadDir.exists()) {
			uploadDir.mkdir();
		}

		uploadDir = new File(uploadPath);
		if (!uploadDir.exists()) {
			uploadDir.mkdir();
		}

		FileItem file = null;
		PrintWriter writer = response.getWriter();
		response.setContentType("application/json");
		JSONArray json = new JSONArray();

		try {
			// System.out.println("start servlet");
			List formItems = upload.parseRequest(request);
			Iterator iter = formItems.iterator();
			while (iter.hasNext()) {
				FileItem item = (FileItem) iter.next();
				if (!item.isFormField()) {
					file = item;
				} else {
					category = item.getFieldName();
				}
			}

			// System.out.println("category " + category);
			// System.out.println("file  " + file);
			fileName = file.getName();
			filePath = uploadPath + File.separator + fileName;
			File storeFile = new File(filePath);
			file.write(storeFile);

			if (category.equals("spec")) {
				session.setAttribute("spec", fileName);
				session.setAttribute("specPath", filePath);
			} else {
				session.setAttribute("db", fileName);
				session.setAttribute("dbPath", filePath);
			}

			JSONObject jsono = new JSONObject();
			jsono.put("name", file.getName());
			json.put(jsono);
		} catch (Exception e) {
			throw new RuntimeException(e);
		} finally {
			writer.write(json.toString());
			writer.close();
		}
	}
}
