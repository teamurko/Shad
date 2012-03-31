package crawler;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.UnsupportedEncodingException;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.TimeUnit;

public class PageDownloader {
	PageDownloader(String filename, boolean save) {
		urlQueue = new LinkedBlockingQueue<String>();
		this.save = save;
		File dataFile = new File(filename);
		try {
			output = new BufferedWriter(new OutputStreamWriter(
					new FileOutputStream(dataFile), "UTF-8"));
		} catch (UnsupportedEncodingException e) {
			e.printStackTrace();
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		}
	}
	
	public void start() {
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

		private void download(String url) {
			try {
				URL page = new URL(url);
				String content = Utils.getContent(page);
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
	private BufferedWriter output;
	boolean save;
}
