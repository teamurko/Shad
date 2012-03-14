package crawler;

public class Crawl {

	public static void main(String[] args) {
		String startUrl = args[0];
		int depth = Integer.parseInt(args[1]);
		int crawlersNumber = Integer.parseInt(args[2]);
		String saveDataFilename = args[3];
		boolean save = Boolean.parseBoolean(args[4]);
		long startTime = System.currentTimeMillis();
		if (depth < 0) {
			throw new RuntimeException("Depth should be >= 0");
		}
		BFSTaskManager taskManager = new BFSTaskManager(depth);
		taskManager.add(new Task(startUrl, 0));
		UrlSeenChecker urlChecker = new UrlSeenChecker();
		PageDownloader pageDownloader = new PageDownloader(saveDataFilename, save);
		Crawler[] crawlers = new Crawler[crawlersNumber];
		Thread[] threads = new Thread[crawlersNumber];
		for (int i = 0; i < crawlersNumber; ++i) {
			crawlers[i] = new Crawler(taskManager, pageDownloader, urlChecker, i);
			threads[i] = new Thread(crawlers[i]);
			threads[i].start();
		}
		for (Thread thread : threads) {
			try {
				thread.join();
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}
		System.err.println("Time passed : " + (System.currentTimeMillis() - startTime));
	}

}
