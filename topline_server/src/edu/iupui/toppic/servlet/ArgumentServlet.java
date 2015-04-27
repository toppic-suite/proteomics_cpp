package edu.iupui.toppic.servlet;

import static java.nio.file.StandardCopyOption.REPLACE_EXISTING;

import java.io.File;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;

import javax.servlet.ServletException;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.servlet.http.HttpSession;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.transform.OutputKeys;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;

import org.w3c.dom.Document;
import org.w3c.dom.Element;

import edu.iupui.toppic.task.TaskList;

@WebServlet("/ArgumentServlet")
public class ArgumentServlet extends HttpServlet {

	private static final long serialVersionUID = 1L;

	protected void doPost(HttpServletRequest request,
			HttpServletResponse response) throws ServletException, IOException {

		String basePath = getServletContext().getRealPath("");
		String title = request.getParameter("upload-task-title");
		String fragment = request.getParameter("fragment");
		String nptm = request.getParameter("n-ptm");
		String ppm = request.getParameter("ppm");
		String cysProtect = request.getParameter("cys-protect");
		String cutoffType = request.getParameter("cutoff-type");
		String cutoff = request.getParameter("cutoff");
		String ptmNum = request.getParameter("ptm-num");
		String maxPtm = request.getParameter("max-ptm");
		String searchType = request.getParameter("search-type");
		String useGf = request.getParameter("use-gf");

		HttpSession session = request.getSession(true);

		int id = TaskList.getTaskId(basePath + File.separator + "tasks"
				+ File.separator + "taskid.xml");

		String spec = session.getAttribute("spec").toString();
		String db = session.getAttribute("db").toString();
		String specPath = session.getAttribute("specPath").toString();
		String dbPath = session.getAttribute("dbPath").toString();

		session.invalidate();

		File processDir = new File(basePath + File.separator + "process");
		if (!processDir.exists()) {
			processDir.mkdir();
		}

		String processPath = basePath + File.separator + "process"
				+ File.separator + "TASK" + id;
		processDir = new File(processPath);

		if (!processDir.exists()) {
			processDir.mkdir();
		}

		Files.copy(Paths.get(specPath),
				Paths.get(processPath + File.separator + spec),
				REPLACE_EXISTING);

		specPath = processPath + File.separator + spec;

		Files.copy(Paths.get(dbPath),
				Paths.get(processPath + File.separator + db), REPLACE_EXISTING);

		dbPath = processPath + File.separator + db;

		DocumentBuilderFactory docFactory = DocumentBuilderFactory
				.newInstance();
		try {
			DocumentBuilder docBuilder = docFactory.newDocumentBuilder();
			Document doc = docBuilder.newDocument();
			Element rootElement = doc.createElement("arguments");
			doc.appendChild(rootElement);

			Element taskTitle = doc.createElement("title");
			rootElement.appendChild(taskTitle);
			taskTitle.appendChild(doc.createTextNode(title));

			Element database_file_name = doc
					.createElement("database_file_name");
			rootElement.appendChild(database_file_name);
			database_file_name.appendChild(doc.createTextNode(dbPath));

			Element spectrum_file_name = doc
					.createElement("spectrum_file_name");
			rootElement.appendChild(spectrum_file_name);
			spectrum_file_name.appendChild(doc.createTextNode(specPath));

			Element log_file_name = doc.createElement("log_file_name");
			rootElement.appendChild(log_file_name);
			log_file_name.appendChild(doc.createTextNode(processPath
					+ File.separator + "log.txt"));

			Element fragmentation_method = doc
					.createElement("fragmentation_method");
			rootElement.appendChild(fragmentation_method);
			fragmentation_method.appendChild(doc.createTextNode(fragment));

			Element cysteine_protecting_group = doc
					.createElement("cysteine_protecting_group");
			rootElement.appendChild(cysteine_protecting_group);
			cysteine_protecting_group.appendChild(doc
					.createTextNode(cysProtect));

			Element search_type = doc.createElement("search_type");
			rootElement.appendChild(search_type);
			if (searchType == null) {
				search_type.appendChild(doc.createTextNode("TARGET"));
			} else {
				search_type.appendChild(doc.createTextNode("TARGET+DECOY"));
			}

			Element shift_number = doc.createElement("shift_number");
			rootElement.appendChild(shift_number);
			shift_number.appendChild(doc.createTextNode(ptmNum));

			Element error_tolerance = doc.createElement("error_tolerance");
			rootElement.appendChild(error_tolerance);
			error_tolerance.appendChild(doc.createTextNode(ppm));

			Element cutoff_type = doc.createElement("cutoff_type");
			rootElement.appendChild(cutoff_type);
			cutoff_type.appendChild(doc.createTextNode(cutoffType));

			Element cutoff_value = doc.createElement("cutoff_value");
			rootElement.appendChild(cutoff_value);
			cutoff_value.appendChild(doc.createTextNode(cutoff));

			Element max_ptm_mass = doc.createElement("max_ptm_mass");
			rootElement.appendChild(max_ptm_mass);
			max_ptm_mass.appendChild(doc.createTextNode(maxPtm));

			Element use_gf = doc.createElement("use_gf");
			rootElement.appendChild(use_gf);

			if (useGf == null) {
				use_gf.appendChild(doc.createTextNode("true"));
			} else {
				use_gf.appendChild(doc.createTextNode("false"));
			}

			Element protein_variable_ptm_list = doc
					.createElement("protein_variable_ptm_list");
			rootElement.appendChild(protein_variable_ptm_list);

			Element protein_variable_ptm = doc
					.createElement("protein_variable_ptm");
			protein_variable_ptm_list.appendChild(protein_variable_ptm);
			protein_variable_ptm.appendChild(doc.createTextNode("NONE"));

			if (nptm.equals("nptm2")) {
				protein_variable_ptm = doc
						.createElement("protein_variable_ptm");
				protein_variable_ptm_list.appendChild(protein_variable_ptm);
				protein_variable_ptm.appendChild(doc.createTextNode("NME"));
			} else if (nptm.equals("nptm3")) {
				protein_variable_ptm = doc
						.createElement("protein_variable_ptm");
				protein_variable_ptm_list.appendChild(protein_variable_ptm);
				protein_variable_ptm.appendChild(doc.createTextNode("NME"));
				protein_variable_ptm = doc
						.createElement("protein_variable_ptm");
				protein_variable_ptm_list.appendChild(protein_variable_ptm);
				protein_variable_ptm.appendChild(doc
						.createTextNode("NME_ACETYLATION"));
			}

			TransformerFactory tFactory = TransformerFactory.newInstance();
			Transformer transformer = tFactory.newTransformer();
			transformer.setOutputProperty(OutputKeys.INDENT, "yes");
			DOMSource source = new DOMSource(doc);
			StreamResult result = new StreamResult(new File(processPath
					+ File.separator + "arguments.xml"));
			transformer.transform(source, result);

		} catch (Exception e) {
			e.printStackTrace();
		}

		TaskList.addTaskList(id, title, basePath + File.separator + "tasks"
				+ File.separator + "tasklist.xml");

		TaskList.setTaskId(basePath + File.separator + "tasks" + File.separator
				+ "taskid.xml", id);

		// response.sendRedirect("index.jsp");
	}

}
