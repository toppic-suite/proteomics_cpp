package edu.iupui.toppc.util;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.transform.OutputKeys;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

public class TaskList {

	public static int getTaskId(String filename) {
		Document data = load(filename);
		Node id = data.getElementsByTagName("id").item(0);
		return (Integer.parseInt(id.getTextContent()) + 1);
	}

	public static void setTaskId(String filename, int k) {
		Document data = load(filename);
		Node id = data.getElementsByTagName("id").item(0);
		id.setTextContent(k + "");
		doc2XmlFile(data, filename);
	}

	public static Document load(String filename) {
		Document document = null;
		try {
			DocumentBuilderFactory factory = DocumentBuilderFactory
					.newInstance();
			DocumentBuilder builder = factory.newDocumentBuilder();
			document = builder.parse(new File(filename));
			document.normalize();
		} catch (Exception ex) {
			ex.printStackTrace();
		}
		return document;
	}

	public static boolean doc2XmlFile(Document document, String filename) {
		boolean flag = true;
		try {
			TransformerFactory tFactory = TransformerFactory.newInstance();
			Transformer transformer = tFactory.newTransformer();
			transformer.setOutputProperty(OutputKeys.INDENT, "yes");
			DOMSource source = new DOMSource(document);
			StreamResult result = new StreamResult(new File(filename));
			transformer.transform(source, result);
		} catch (Exception ex) {
			flag = false;
			ex.printStackTrace();
		}
		return flag;
	}

	public static void addTaskList(int id, String file) {
		Document data = load(file);
		Element root = data.getDocumentElement();
		Element task = data.createElement("task");
		Element ide = data.createElement("id");
		Element status = data.createElement("status");
		Element start = data.createElement("start");
		Element end = data.createElement("end");
		start.setTextContent("");
		status.setTextContent("waiting");
		ide.setTextContent(id + "");
		root.appendChild(task);
		task.appendChild(ide);
		task.appendChild(status);
		task.appendChild(start);
		task.appendChild(end);
		doc2XmlFile(data, file);
	}

	public static void addTaskList(int id, String title, String file) {
		Document data = load(file);
		Element root = data.getDocumentElement();
		Element task = data.createElement("task");
		Element ide = data.createElement("id");
		Element status = data.createElement("status");
		Element start = data.createElement("start");
		Element end = data.createElement("end");
		Element t = data.createElement("title");
		Element sub = data.createElement("submit");
		start.setTextContent("");
		status.setTextContent("waiting");
		ide.setTextContent(id + "");
		t.setTextContent(title + "");
		sub.setTextContent(System.currentTimeMillis() + "");
		root.appendChild(task);
		task.appendChild(ide);
		task.appendChild(status);
		task.appendChild(start);
		task.appendChild(end);
		task.appendChild(t);
		task.appendChild(sub);
		doc2XmlFile(data, file);
	}

	public static void delTaskList(String id, String file) {
		Document data = load(file);

		for (int i = 0; i < data.getElementsByTagName("task").getLength(); i++) {
			Node task = data.getElementsByTagName("task").item(i);
			NodeList taskinfo = task.getChildNodes();
			for (int j = 0; j < taskinfo.getLength(); j++) {
				if (taskinfo.item(j).getNodeName().equals("id")) {
					if (taskinfo.item(j).getTextContent().equals(id)) {
						task.getParentNode().removeChild(task);
						break;
					}
				}
			}
		}
		doc2XmlFile(data, file);
	}

	public static ArrayList<String> getTaskList(String file) {
		ArrayList<String> res = new ArrayList<String>();
		Document data = load(file);
		for (int i = 0; i < data.getElementsByTagName("task").getLength(); i++) {
			Node task = data.getElementsByTagName("task").item(i);
			NodeList taskinfo = task.getChildNodes();
			for (int j = 0; j < taskinfo.getLength(); j++) {
				if (taskinfo.item(j).getNodeName().equals("id")) {
					res.add(taskinfo.item(j).getTextContent());
				}
			}
		}

		return res;
	}

	public static int getNextId(String file) {
		Document data = load(file);
		int id = 0;
		boolean flag = false;
		// System.out.println("data.getElementsByTagName(\"task\").getLength() "
		// + data.getElementsByTagName("task").getLength());
		for (int i = 0; i < data.getElementsByTagName("task").getLength(); i++) {
			Node task = data.getElementsByTagName("task").item(i);
			NodeList taskinfo = task.getChildNodes();
			for (int j = 0; j < taskinfo.getLength(); j++) {
				if (taskinfo.item(j).getNodeName().equals("status")) {

					if (taskinfo.item(j).getTextContent().equals("waiting")) {
						flag = true;
					}
				} else if (taskinfo.item(j).getNodeName().equals("id")) {
					id = Integer.parseInt(taskinfo.item(j).getTextContent());
				}
			}

			if (flag)
				return id;
		}

		return 0;
	}

	public static int getRunningId(String file) {
		Document data = load(file);
		int id = 0;
		boolean flag = false;
		for (int i = 0; i < data.getElementsByTagName("task").getLength(); i++) {
			Node task = data.getElementsByTagName("task").item(i);
			NodeList taskinfo = task.getChildNodes();
			for (int j = 0; j < taskinfo.getLength(); j++) {
				if (taskinfo.item(j).getNodeName().equals("status")) {
					if (taskinfo.item(j).getTextContent().equals("running")) {
						flag = true;
					}
				} else if (taskinfo.item(j).getNodeName().equals("id")) {
					id = Integer.parseInt(taskinfo.item(j).getTextContent());
				}
			}

			if (flag)
				return id;
		}

		return 0;
	}

	public static void setFinished(String file, int id) {

		Document data = load(file);

		for (int i = 0; i < data.getElementsByTagName("task").getLength(); i++) {
			Node task = data.getElementsByTagName("task").item(i);
			NodeList taskinfo = task.getChildNodes();

			if (Integer.parseInt(taskinfo.item(1).getTextContent()) == id) {
				taskinfo.item(3).setTextContent("finished");
				taskinfo.item(7)
						.setTextContent(System.currentTimeMillis() + "");
			}

		}
		doc2XmlFile(data, file);
	}

	public static void setStopped(String file, String id) {
		Document data = load(file);
		for (int i = 0; i < data.getElementsByTagName("task").getLength(); i++) {
			Node task = data.getElementsByTagName("task").item(i);
			NodeList taskinfo = task.getChildNodes();
			if (taskinfo.item(1).getTextContent().equals(id)) {
				taskinfo.item(3).setTextContent("stopped");
				taskinfo.item(7)
						.setTextContent(System.currentTimeMillis() + "");
			}

		}
		doc2XmlFile(data, file);
	}

	public static void setStart(String file, int id) {
		Document data = load(file);
		for (int i = 0; i < data.getElementsByTagName("task").getLength(); i++) {
			Node task = data.getElementsByTagName("task").item(i);
			NodeList taskinfo = task.getChildNodes();
			if (Integer.parseInt(taskinfo.item(1).getTextContent()) == id) {
				taskinfo.item(3).setTextContent("running");
				taskinfo.item(5)
						.setTextContent(System.currentTimeMillis() + "");
			}

		}
		doc2XmlFile(data, file);
	}

	public static ArrayList<Task> getTask(String file) {
		ArrayList<Task> res = new ArrayList<Task>();
		Document data = load(file);
		for (int i = 0; i < data.getElementsByTagName("task").getLength(); i++) {
			Task t = new Task();
			Node task = data.getElementsByTagName("task").item(i);
			NodeList taskinfo = task.getChildNodes();
			for (int j = 0; j < taskinfo.getLength(); j++) {
				if (taskinfo.item(j).getNodeName().equals("id")) {
					t.setID(taskinfo.item(j).getTextContent());
				} else if (taskinfo.item(j).getNodeName().equals("status")) {
					t.setStatus(taskinfo.item(j).getTextContent());
				} else if (taskinfo.item(j).getNodeName().equals("start")) {
					t.setStart(taskinfo.item(j).getTextContent());
				} else if (taskinfo.item(j).getNodeName().equals("end")) {
					t.setEnd(taskinfo.item(j).getTextContent());
				} else if (taskinfo.item(j).getNodeName().equals("title")) {
					t.setTitle(taskinfo.item(j).getTextContent());
				} else if (taskinfo.item(j).getNodeName().equals("submit")) {
					t.setSubmit(taskinfo.item(j).getTextContent());
				}
			}
			res.add(t);
		}
		return res;
	}

	public static double getPercentage(String file) throws IOException {
		String lastline = "", currentline = "";
		File in = new File(file);
		if (!in.exists()) {
			return 0.0;
		}
		BufferedReader log = new BufferedReader(new FileReader(new File(file)));
		while ((currentline = log.readLine()) != null) {
			lastline = currentline;
		}
		log.close();

		if (lastline.length() == 0) {
			return 0.0;
		} else {
			return Double.parseDouble(lastline) * 100;
		}

	}

	public static String getSpec(String file) {
		String spec = "";
		Document data = load(file);
		spec = data.getElementsByTagName("spectrum_file_name").item(0)
				.getTextContent().toString();
		spec = spec.substring(spec.lastIndexOf(File.separator) + 1,
				spec.lastIndexOf("."));
		return spec;
	}

	public static String getStatusByID(String id, String file) {
		String status = "";
		Document data = load(file);
		for (int i = 0; i < data.getElementsByTagName("task").getLength(); i++) {
			Node task = data.getElementsByTagName("task").item(i);
			NodeList taskinfo = task.getChildNodes();
			if (taskinfo.item(1).getTextContent().equals(id)) {
				status = taskinfo.item(3).getTextContent();
			}
		}
		return status;
	}

	public static String getDB(String file) {
		String db;
		Document data = load(file);
		db = data.getElementsByTagName("database_file_name").item(0)
				.getTextContent();
		return db;
	}

	public static String getSp(String file) {
		String db;
		Document data = load(file);
		db = data.getElementsByTagName("spectrum_file_name").item(0)
				.getTextContent();
		return db;
	}
}
