package crawler;

import java.util.HashSet;
import java.util.Set;

public class UrlSeenChecker {
	UrlSeenChecker() {
		this.cache = new HashSet<String>();
	}
	
	public synchronized boolean add(String url) {
		if (!cache.contains(url)) {
			cache.add(url);
			return true;
		}
		return false;
	}
	private Set<String> cache;
}
