package edu.iupui.toppic.process;

import javax.servlet.ServletContextEvent;
import javax.servlet.ServletContextListener;

public class TopListener implements ServletContextListener {

	private java.util.Timer timer = null;

	@Override
	public void contextDestroyed(ServletContextEvent svrcontext) {
	}

	@Override
	public void contextInitialized(ServletContextEvent svrcontext) {
		timer = new java.util.Timer(true);
		svrcontext.getServletContext().log("");
		timer.schedule(new TopProcess(svrcontext.getServletContext()), 0, 5000);
		svrcontext.getServletContext().log("");
	}
}
