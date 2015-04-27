package edu.iupui.toppic.task;

import java.io.File;
import java.io.IOException;
import java.text.DecimalFormat;
import java.util.ArrayList;

import javax.servlet.ServletException;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

@WebServlet("/TaskInfoServlet")
public class TaskInfoServlet extends HttpServlet {
	private static final long serialVersionUID = 1L;

	public TaskInfoServlet() {
		super();

	}

	protected void doGet(HttpServletRequest request,
			HttpServletResponse response) throws ServletException, IOException {

		DecimalFormat df = new DecimalFormat("#.##");
		String id = request.getParameter("id");
		String basePath = getServletContext().getRealPath("");
		String logPath = getServletContext().getRealPath("") + File.separator
				+ "process" + File.separator + "TASK" + id + File.separator
				+ "log.txt";
		double per = TaskList.getPercentage(logPath);// df.format(per)
		String status = null, runTime = null;
		ArrayList<Task> tasks = TaskList.getTask(basePath + File.separator
				+ "tasks" + File.separator + "tasklist.xml");
		for (int i = 0; i < tasks.size(); i++) {
			if (tasks.get(i).getID().equals(id)) {
				status = tasks.get(i).getStatus();
				runTime = tasks.get(i).getRuntime();
			}
		}

		response.getWriter().write(
				df.format(per) + "," + status + "," + runTime);
	}

}
