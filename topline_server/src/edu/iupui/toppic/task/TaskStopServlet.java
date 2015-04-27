package edu.iupui.toppic.task;

import java.io.File;
import java.io.IOException;

import javax.servlet.ServletException;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

@WebServlet("/TaskStopServlet")
public class TaskStopServlet extends HttpServlet {
	private static final long serialVersionUID = 1L;

	protected void doPost(HttpServletRequest request,
			HttpServletResponse response) throws ServletException, IOException {
		String basePath = getServletContext().getRealPath("");

		String id = request.getParameter("stop");

		int count = Thread.activeCount();
		Thread th[] = new Thread[count];
		Thread.enumerate(th);

		for (int i = 0; i < count; i++) {
			// System.out.println("thread name " + th[i].getName());
			if (th[i].getName().equals("TopPIC")) {
				th[i].interrupt();
				break;
			}
		}
		TaskList.setStopped(basePath + File.separator + "tasks"
				+ File.separator + "tasklist.xml", id);
		response.sendRedirect("results.jsp");

	}

}
