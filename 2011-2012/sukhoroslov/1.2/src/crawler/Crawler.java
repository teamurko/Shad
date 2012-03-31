package crawler;

import java.net.MalformedURLException;
import java.net.URL;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class Crawler implements Runnable {
	public static Pattern linkPattern = Pattern
			.compile("<a href=\"([^\"#]+)\"");

	Crawler(BFSTaskManager taskManager, PageDownloader downloader, UrlSeenChecker urlChecker,
			int id) {
		this.taskManager = taskManager;
		this.downloader = downloader;
		this.urlChecker = urlChecker;
		this.id = id;
	}

	public void run() {
		while (true) {
			Task task = taskManager.next();
			System.err.println("Crawler " + id + " took task " + task);
			if (task == null) {
				break;
			}
			List<String> adjacentUrls = adjacentUrls(task.getUrl());
			if (adjacentUrls != null) {
				for (String url : adjacentUrls) {
					Task nextTask = new Task(url, task.getDistance() + 1);
		 			taskManager.add(nextTask);
				}
			}
			task.done();
			System.err.println("Crawler " + id + " done task " + task);
			downloader.add(task.getUrl());
		}
		System.err.println("Crawler " + id + " finished its work");
	}

	private List<String> adjacentUrls(String url) {
		try {
			String content = Utils.getContent(new URL(url));
			if (content != null) {
				Set<URL> links = getLinks(new URL(url), content);
				List<String> result = new ArrayList<String>();
				for (URL link : links) {
					if (urlChecker.add(link.toString())) {
						result.add(link.toString());
					}	
				}
				return result;
			}
		} catch (MalformedURLException e) {
			e.printStackTrace();
		}
		return null;
	}

	private static Set<URL> getLinks(URL url, String content) {
		Set<URL> links = new HashSet<URL>();
		Matcher matcher = linkPattern.matcher(content);
		while (matcher.find()) {
			try {
				URL link = new URL(url, matcher.group(1));
				links.add(link);
			} catch (MalformedURLException e) {
			}
		}
		return links;
	}

	private BFSTaskManager taskManager;
	private PageDownloader downloader;
	private UrlSeenChecker urlChecker;
	private int id;
}
