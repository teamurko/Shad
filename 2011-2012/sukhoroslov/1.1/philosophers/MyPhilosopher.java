package philosophers;

import java.util.ArrayList;
import java.util.List;

public class MyPhilosopher extends Philosopher implements Runnable {

    volatile boolean stopFlag = false;
    List<Fork> forksInOrder;

    public MyPhilosopher(int position, Fork left, Fork right) {
        super(position, left, right);
        forksInOrder = new ArrayList<Fork>();
        if (position % 2 == 0) {
        	forksInOrder.add(right);
        	forksInOrder.add(left);
        } else {
        	forksInOrder.add(left);
        	forksInOrder.add(right);
        }
    }

    public void run() {
        while (!stopFlag) {
            think();
            synchronized (forksInOrder.get(0)) {
                System.out.println(
                    "[Philosopher " + position + "] took the first fork");
                synchronized (forksInOrder.get(1)) {
                    System.out.println(
                        "[Philosopher " + position +
                        "] took the second fork");
                    eat();
                }
            }
        }
        System.out.println("[Philosopher " + position + "] stopped");
    }
}
