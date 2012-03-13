package philosophers;

import java.util.Random;

public class Philosopher {

    int position;
    Fork left;
    Fork right;
    int eatCount = 0;
    long waitTime = 0;
    long startWait;
    Random rnd = new Random();

    public Philosopher(int position, Fork left, Fork right) {
        this.position = position;
        this.left = left;
        this.right = right;
    }

    public void eat() {
        waitTime += System.currentTimeMillis() - startWait;
        System.out.println("[Philosopher " + position + "] is eating");
        try {
            Thread.sleep(rnd.nextInt(100));
        } catch (InterruptedException e) { e.printStackTrace(); }
        eatCount++;
        System.out.println("[Philosopher " + position + "] finished eating");
    }

    public void think() {
        System.out.println("[Philosopher " + position + "] is thinking");
        try {
            Thread.sleep(rnd.nextInt(100));
        } catch (InterruptedException e) { e.printStackTrace(); }
        System.out.println("[Philosopher " + position + "] is hungry");
        startWait = System.currentTimeMillis();
    }

}
