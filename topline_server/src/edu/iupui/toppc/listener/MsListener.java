package edu.iupui.toppc.listener;

import javax.servlet.ServletContextEvent;
import javax.servlet.ServletContextListener;

import edu.iupui.toppc.task.*;

public class MsListener implements ServletContextListener {

	private java.util.Timer timer = null;

	@Override
	public void contextDestroyed(ServletContextEvent svrcontext) {

	}

	@Override
	public void contextInitialized(ServletContextEvent svrcontext) {

		timer = new java.util.Timer(true);
		svrcontext.getServletContext().log("");
		timer.schedule(new MsTask(svrcontext.getServletContext()), 0, 2000);
		svrcontext.getServletContext().log("");
	}
}
