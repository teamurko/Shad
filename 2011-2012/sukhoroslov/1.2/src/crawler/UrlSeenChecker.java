package crawler;

import java.util.HashSet;
import java.util.Set;

public class UrlSeenChecker {
	UrlSeenChecker() {
		this.cache = new HashSet<String>();
	}
	private synchronized boolean check(String url) {
		return cache.contains(url);
	}
	private synchronized void see(String url) {
		if (check(url)) {
			throw new RuntimeException("Cannot see already seen url");
		}
		cache.add(url);
	}
	public synchronized boolean add(String url) {
		if (!check(url)) {
			see(url);
			return true;
		}
		return false;
	}
	private Set<String> cache;
}
