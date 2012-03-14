package crawler;

import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.TimeUnit;

/*
 * First task should be added with distance 0.
 */
public class BFSTaskManager {
	BFSTaskManager(int maxDistance) {
		queue = new LinkedBlockingQueue<Task>();
		distanceAudit = new HashMap<Integer, Integer>();
		this.maxDistance =  maxDistance;
	}
	
	public void add(Task task) {
		if (task.getDistance() > maxDistance) {
			return;
		}
		queue.add(task);
		task.setManager(this);
		synchronized (distanceAudit) {
			if (distanceAudit.containsKey(task.getDistance())) {
				Integer value = distanceAudit.get(task.getDistance());
			distanceAudit.put(task.getDistance(), value.intValue() + 1);
			} else {
				distanceAudit.put(task.getDistance(), 1);
			}
		}
		System.err.println("Task " + task + " is added to task manager");
	}
	
	public Task next() {
		try {
			while (true) {
				synchronized (distance) {
					if (distance > maxDistance) {
						return null;
					}
				}
				Task task = queue.poll(100, TimeUnit.MILLISECONDS);
				if (task == null) {
					continue;
				}
				while (true) {
					synchronized (distance) {
						if (task.getDistance() == distance.intValue()) {
							return task;
						}
					}
					Thread.sleep(100);
				}
			}
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		return null;
	}
	
	public synchronized void decrement(int key) {
		synchronized (distanceAudit) {
			if (distanceAudit.containsKey(key)) {
				distanceAudit.put(key, distanceAudit.get(key) - 1);
				if (distanceAudit.get(key).intValue() == 0) {
					++distance;
				}
			} else {
				throw new RuntimeException("Unknown key");
			}
		}
	}
	
	private BlockingQueue<Task> queue;
	private Map<Integer, Integer> distanceAudit;
	private Integer distance = 0;
	private int maxDistance;
}
