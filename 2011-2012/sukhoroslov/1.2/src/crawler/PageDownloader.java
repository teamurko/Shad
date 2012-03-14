package crawler;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.UnsupportedEncodingException;
import java.net.MalformedURLException;
import java.net.URL;
import java.net.URLConnection;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.TimeUnit;

public class PageDownloader {
	PageDownloader(String filename, boolean save) {
		urlQueue = new LinkedBlockingQueue<String>();
		File dataFile = new File(filename);
		BufferedWriter output = null;
		try {
			output = new BufferedWriter(new OutputStreamWriter(
					new FileOutputStream(dataFile), "UTF-8"));
		} catch (UnsupportedEncodingException e) {
			e.printStackTrace();
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		}
		//TODO move to separate method
		new Thread(new Downloader(output, urlQueue, save)).start();
	}
	
	public void add(String url) {
		urlQueue.add(url);
	}

	
	private class Downloader implements Runnable {
		Downloader(BufferedWriter output, BlockingQueue<String> queue, boolean save) {
			this.queue = queue;
			this.output = output;
			this.save = save;
		}
		
		public void run() {
			try {
				while (true) {
					String url = queue.poll(1000, TimeUnit.MILLISECONDS);
					if (url == null) {
						break;
					}
					download(url);
				}
			} catch (InterruptedException e) {
				e.printStackTrace();
			} finally {
				try {
					output.close();
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
		}
		
		private String getContent(URL url) {
			StringBuilder page = new StringBuilder();
			BufferedReader in;
			try {
				URLConnection conn = url.openConnection();
				String contentType = conn.getContentType();
				if (contentType != null && contentType.startsWith("text/html")) {
					if (contentType.indexOf("charset=") == -1) {
						in = new BufferedReader(new InputStreamReader(
								conn.getInputStream(), "UTF-8"));
					} else {
						String encoding = contentType.substring(contentType
								.indexOf("charset=") + 8);
						in = new BufferedReader(new InputStreamReader(
								conn.getInputStream(), encoding));
					}
					String str;
					while ((str = in.readLine()) != null) {
						page.append(str);
					}
					in.close();
					return page.toString();
				}
			} catch (IOException e) {
				e.printStackTrace();
			}
			return null;
		}

		private void download(String url) {
			try {
				URL page = new URL(url);
				String content = getContent(page);
				if (content != null && save) {
					output.write(page + "\t" + content + "\n");
				}
			} catch (MalformedURLException e) {
				e.printStackTrace();
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
		
		BlockingQueue<String> queue;
		BufferedWriter output;
		boolean save;
	}

	private BlockingQueue<String> urlQueue;
}
