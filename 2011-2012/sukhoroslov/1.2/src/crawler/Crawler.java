package crawler;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.MalformedURLException;
import java.net.URL;
import java.net.URLConnection;
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
					System.err.println("Adding url " + url);
					taskManager.add(new Task(url, task.getDistance() + 1));
				}
			}
			task.done();
			System.err.println("Crawler " + id + " done task " + task);
			downloader.add(task.getUrl());
		}
	}
	
	//FIX copy-paste
	private static String getContent(URL url) {
        StringBuilder page = new StringBuilder ();
        BufferedReader in;
        try {
            URLConnection conn = url.openConnection();
            String contentType = conn.getContentType();
            if (contentType != null && contentType.startsWith("text/html")) {
                if (contentType.indexOf("charset=") == -1) {
                    in = new BufferedReader(new InputStreamReader(conn.getInputStream(), "UTF-8"));
                } else {
                    String encoding = contentType.substring(contentType.indexOf("charset=") + 8);
                    in = new BufferedReader(new InputStreamReader(conn.getInputStream(), encoding));
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

	private List<String> adjacentUrls(String url) {
		try {
			String content = getContent(new URL(url));
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
