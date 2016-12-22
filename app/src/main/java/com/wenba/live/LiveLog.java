package com.wenba.live;

import android.annotation.SuppressLint;
import android.util.Log;

import java.text.SimpleDateFormat;
import java.util.Date;

@SuppressLint("SimpleDateFormat")
public class LiveLog {
	public static String DEBUG_TAG = "LiveDebug";
	private static volatile LiveLog sInstance;
	private static SimpleDateFormat mDateFormat = new SimpleDateFormat("MM-dd HH:mm:ss SSS");
	private static String LOG_FORMAT = "%s(%s)(%s): %s\r\n";
	private static Date DATE = new Date();
	private StringBuffer logBuffer = new StringBuffer();
	private static final int MAX_LENGTH = 512 * 1024;
	
	private static StringBuffer preLogBuffer = new StringBuffer();
	
	public static void e(String msg) {
		e(DEBUG_TAG, msg);
	}

	public static void e(String tag, String msg) {
		String logString = getFormatLog(tag,msg);

		LiveLog instance = sInstance;
		if (instance != null) {
			instance.inner(logString);
		}else{
			preLog(logString);
		}

		Log.d(tag, logString);
	}
	
	private static void preLog(String msg){
		synchronized (preLogBuffer) {
			if(preLogBuffer.length() > MAX_LENGTH){
				preLogBuffer.setLength(0);
			}
		}
		preLogBuffer.append(msg);
	}
	
	private static String getFormatLog(String tag, String msg){
		DATE.setTime(System.currentTimeMillis());

		if (msg != null && msg.length() > 1 && msg.endsWith("\n")) {
			msg = msg.substring(0, msg.length() - 1);
		}

		Thread current = Thread.currentThread(); 
		return String.format(LOG_FORMAT, mDateFormat.format(DATE), ""+current.getId(), tag, msg);
	}

	private void inner(String msg) {
		synchronized (logBuffer) {
			if(logBuffer.length() > MAX_LENGTH){
				logBuffer.setLength(0);
			}
		}
		
		logBuffer.append(msg);
	}
	
	public String getLogs(){
		String logs = logBuffer.toString();
		logBuffer.setLength(0);
		return logs;
	}

	public static LiveLog createInstance() {
		if (sInstance != null) {
			return sInstance;
		}
		synchronized (LiveLog.class) {
			if (sInstance == null) {
				sInstance = new LiveLog();
			}
		}
		sInstance.inner(preLogBuffer.toString());
		preLogBuffer.setLength(0);

		return sInstance;
	}

	public void destory() {
		sInstance = null;
		logBuffer.setLength(0);
		preLogBuffer.setLength(0);
	}

	
}
