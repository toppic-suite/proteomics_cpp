package edu.iupui.toppc.util;

import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;
import java.util.concurrent.TimeUnit;

public class Task {

	public Task() {
		id = "";
		status = "";
		start = "";
		end = "";
		title = "";
		spec = "";
	}

	public String getID() {
		return id;
	}

	public void setID(String id) {
		this.id = id;
	}

	public String getStatus() {
		return status;
	}

	public void setStatus(String status) {
		this.status = status;
	}

	public String getStart() {
		return start;
	}

	public void setStart(String start) {
		this.start = start;
	}

	public String getEnd() {
		return end;
	}

	public void setEnd(String end) {
		this.end = end;
	}

	public String getTitle() {
		return title;
	}

	public void setTitle(String title) {
		this.title = title;
	}

	public void setSubmit(String sub) {
		submission = sub;
	}

	public String getSubmitTime() {
		SimpleDateFormat sdf = new SimpleDateFormat("MMM dd HH:mm",
				Locale.ENGLISH);
		long time = Long.parseLong(submission);
		Date resultdate = new Date(time);
		String timestr = sdf.format(resultdate);

		return timestr;
	}

	public String getRuntime() {
		long time;
		if (start.length() == 0) {
			time = 0;
		} else if (end.length() > 0) {
			time = Long.parseLong(end) - Long.parseLong(start);
		} else {
			time = System.currentTimeMillis() - Long.parseLong(start);
		}

		TimeUnit.MILLISECONDS.toMinutes(time);

		long h = TimeUnit.MILLISECONDS.toHours(time);
		long m = TimeUnit.MILLISECONDS.toMinutes(time)
				- TimeUnit.HOURS.toMinutes(h);
		long s = TimeUnit.MILLISECONDS.toSeconds(time)
				- TimeUnit.HOURS.toSeconds(h) - TimeUnit.MINUTES.toSeconds(m);
		String timestr = String.format("%d:%02d:%02d", h, m, s);
		return timestr;
	}

	private String id, status, start, end, submission, title, spec;

}
