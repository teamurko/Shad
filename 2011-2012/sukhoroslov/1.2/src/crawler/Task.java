package crawler;

public class Task {
	Task(String url, int distance) {
		this.setUrl(url);
		this.setDistance(distance);
	}
	
	public void setUrl(String url) {
		this.url = url;
	}
	public String getUrl() {
		return url;
	}

	public void setDistance(int distance) {
		this.distance = distance;
	}

	public int getDistance() {
		return distance;
	}
	
	public void setManager(BFSTaskManager manager) {
		taskManager = manager;
	}
	
	public void done() {
		if (null == taskManager) {
			throw new RuntimeException("Task manager is not defined");
		}
		taskManager.decrement(distance);
	}
	
	public String toString() {
		return url + ", distance " + distance;
	}

	private String url;
	private int distance;
	private BFSTaskManager taskManager = null;
}
