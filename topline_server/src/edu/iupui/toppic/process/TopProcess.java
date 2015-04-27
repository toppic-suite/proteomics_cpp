package edu.iupui.toppic.process;

import java.util.TimerTask;

import javax.servlet.ServletContext;

public class TopProcess extends TimerTask {

	private static boolean isRunning = false;
	private ServletContext context = null;

	public TopProcess(ServletContext context) {
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
			if (th[i].getName().equals("TopPIC")) {
				isRunning = true;
				break;
			}
		}

		if (!isRunning) {
			TopPIC t = new TopPIC(context);
			t.start();
		}
	}

}