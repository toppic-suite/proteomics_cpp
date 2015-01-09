package edu.iupui.toppc.task;

import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStreamReader;

import javax.servlet.ServletContext;

import edu.iupui.toppc.util.TaskList;

public class TopPC extends Thread {

	private Process p = null;

	private ServletContext context = null;

	public TopPC(ServletContext context) {
		this.context = context;
	}

	@Override
	public void run() {
		try {
			String basePath = context.getRealPath("");
			int id = TaskList.getNextId(basePath + File.separator + "log"
					+ File.separator + "tasklist.xml");

			if (id != 0) {
				String arguments = basePath + File.separator + "process"
						+ File.separator + "TASK" + id + File.separator
						+ "arguments.xml";

				String db = TaskList.getDB(arguments);

				String spec = TaskList.getSp(arguments);

				Thread.currentThread().setName("TopPC");

				ProcessBuilder pb;

				if (System.getProperty("os.name").toLowerCase().equals("linux")) {
					pb = new ProcessBuilder("bash", "-c", basePath
							+ File.separator + "tools" + File.separator
							+ "toppic -f " + arguments + " " + db + " " + spec
							+ " > " + basePath + File.separator + "process"
							+ File.separator + "TASK" + id + File.separator
							+ "run.log");
				} else {
					pb = new ProcessBuilder("cmd.exe", "/c", basePath
							+ File.separator + "tools" + File.separator
							+ "toppic.exe -f " + arguments + " " + db + " "
							+ spec + " > " + basePath + File.separator
							+ "process" + File.separator + "TASK" + id
							+ File.separator + "run.log");
				}

				TaskList.setStart(basePath + File.separator + "log"
						+ File.separator + "tasklist.xml", id);

				p = pb.start();

				BufferedReader buf = new BufferedReader(new InputStreamReader(
						p.getInputStream()));

				String line = "";
				while ((line = buf.readLine()) != null) {
					System.out.println(line);
				}

				int exitValue = p.waitFor();

				if (exitValue == 0) {
					TaskList.setFinished(basePath + File.separator + "log"
							+ File.separator + "tasklist.xml", id);
				}
			}

		} catch (IOException e) {
			e.printStackTrace();
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
	}

	@Override
	public void interrupt() {
		if (p != null) {
			p.destroy();

		}
	}
}
