package edu.iupui.toppc.task;

import java.util.TimerTask;

import javax.servlet.ServletContext;

public class MsTask extends TimerTask {

	private static boolean isRunning = false;
	private ServletContext context = null;

	public MsTask(ServletContext context) {
		this.context = context;
	}

	@Override
	public void run() {
		int count = Thread.activeCount();
		Thread th[] = new Thread[count];
		Thread.enumerate(th);

		isRunning = false;
		for (int i = 0; i < count; i++) {
			// System.out.println(th[i].getName());
			if (th[i].getName().equals("TopPC")) {
				isRunning = true;
				break;
			}
		}

		if (!isRunning) {

			TopPC t = new TopPC(context);
			t.start();
		}
	}

}
