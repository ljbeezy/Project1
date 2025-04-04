// Project 2: Threads by Louie Jack Bearden IV 
// CSC 4320
// 04 / 04 / 2025
import java.io.*;  // Import classes for input and output, e.g., BufferedReader for reading file
import java.util.*;  // Import classes for collections like List, ArrayList
import java.util.concurrent.Semaphore;  // Import Semaphore for synchronization

// Thread class representing a process in the simulation
class ProcessThread extends Thread {
    int pid, burstTime;  // Process ID and CPU burst time

    // Constructor to initialize the process ID and burst time
    public ProcessThread(int pid, int burstTime) {
        this.pid = pid;  // Set the process ID
        this.burstTime = burstTime;  // Set the burst time (how long the process will run)
    }

    // Overriding the run() method for the thread's execution
    public void run() {
        System.out.println("Process " + pid + " started.");  // Print when the process starts
        try {
            Thread.sleep(burstTime * 1000);  // Simulate the process's execution by sleeping for burstTime seconds
        } catch (InterruptedException e) {
            // Handle interruption (though it's not expected to happen in this case)
        }
        System.out.println("Process " + pid + " finished.");  // Print when the process finishes
    }
}

// Thread class representing a philosopher in the Dining Philosophers problem
class Philosopher extends Thread {
    private int id;  // Philosopher ID (0-based index)
    private int arrivalTime;  // Time when the philosopher arrives to start eating
    private int burstTime;  // How long the philosopher will eat
    private Semaphore leftFork, rightFork;  // Semaphores for the left and right forks (shared resources)

    // Constructor to initialize the philosopher with an ID, arrival time, burst time, and semaphores for the forks
    public Philosopher(int id, int arrivalTime, int burstTime, Semaphore leftFork, Semaphore rightFork) {
        this.id = id;  // Philosopher ID
        this.arrivalTime = arrivalTime;  // Arrival time of the philosopher
        this.burstTime = burstTime;  // Burst time for the philosopher's eating duration
        this.leftFork = leftFork;  // Semaphore for the left fork
        this.rightFork = rightFork;  // Semaphore for the right fork
    }

    // Method for the philosopher to think (random duration)
    private void think() throws InterruptedException {
        System.out.println("[Philosopher " + id + "] Thinking...");  // Print thinking message
        Thread.sleep((int) (Math.random() * 2000));  // Simulate thinking for a random time (up to 2 seconds)
    }

    // Method for the philosopher to eat (specified burst time)
    private void eat() throws InterruptedException {
        System.out.println("[Philosopher " + id + "] Eating for " + burstTime + " seconds...");  // Print eating message
        Thread.sleep(burstTime * 1000);  // Simulate eating for the burstTime seconds
    }

    // Overriding the run() method to define the philosopher's behavior
    public void run() {
        try {
            // Wait for the philosopher's arrival time before starting
            Thread.sleep(arrivalTime * 1000);
            System.out.println("[Philosopher " + id + "] Arrived.");  // Print when the philosopher arrives

            while (true) {
                // Philosopher starts by thinking
                think();

                System.out.println("[Philosopher " + id + "] Waiting for forks...");  // Print waiting message

                // Simulate the philosopher picking up the forks (deadlock-avoidance strategy)
                if (id % 2 == 0) {  // Even philosophers pick the left fork first
                    leftFork.acquire();  // Try to acquire the left fork (semaphore)
                    System.out.println("[Philosopher " + id + "] Picked up left fork");
                    rightFork.acquire();  // Try to acquire the right fork (semaphore)
                    System.out.println("[Philosopher " + id + "] Picked up right fork");
                } else {  // Odd philosophers pick the right fork first
                    rightFork.acquire();  // Try to acquire the right fork
                    System.out.println("[Philosopher " + id + "] Picked up right fork");
                    leftFork.acquire();  // Try to acquire the left fork
                    System.out.println("[Philosopher " + id + "] Picked up left fork");
                }

                // Philosopher starts eating after picking up both forks
                eat();

                // After eating, philosopher releases both forks
                leftFork.release();  // Release the left fork
                rightFork.release();  // Release the right fork
                System.out.println("[Philosopher " + id + "] Released forks");

                break;  // After one cycle (think, pick forks, eat, release), philosopher stops
            }
        } catch (InterruptedException e) {
            System.out.println("[Philosopher " + id + "] Interrupted.");  // Print if the thread is interrupted
        }
    }
}

// Main class for simulating the Dining Philosophers problem
public class DiningPhilosophers {
    public static void main(String[] args) {
        List<Philosopher> philosophersList = new ArrayList<>();  // List to hold philosopher threads
        Semaphore[] forks;  // Array of semaphores (forks)

        try {
            BufferedReader br = new BufferedReader(new FileReader("processes.txt"));  // Reading philosopher data from a file
            String line;
            List<int[]> processes = new ArrayList<>();  // List to store process data (ID, arrival time, burst time)

            // Reading and parsing the data from the file
            while ((line = br.readLine()) != null) {
                if (line.startsWith("PID")) continue;  // Skip the header line in the file
                String[] parts = line.split("\\s+");  // Split the line into parts (PID, arrival time, burst time)
                int pid = Integer.parseInt(parts[0]);  // Process ID
                int arrivalTime = Integer.parseInt(parts[1]);  // Arrival time
                int burstTime = Integer.parseInt(parts[2]);  // Burst time
                processes.add(new int[]{pid, arrivalTime, burstTime});  // Add the parsed data to the list
            }
            br.close();  // Close the file reader

            // Determine the number of philosophers based on the number of processes
            int numPhilosophers = processes.size();
            forks = new Semaphore[numPhilosophers];  // Initialize semaphores for each fork

            // Initialize semaphores for each fork (each fork is a semaphore with 1 permit)
            for (int i = 0; i < numPhilosophers; i++) {
                forks[i] = new Semaphore(1);  // One philosopher can hold one fork at a time
            }

            // Create philosopher threads based on the process data
            for (int i = 0; i < numPhilosophers; i++) {
                int[] process = processes.get(i);  // Get the process data for this philosopher
                // Create a philosopher thread, passing their ID, arrival time, burst time, and fork semaphores
                Philosopher philosopher = new Philosopher(process[0], process[1], process[2], forks[i], forks[(i + 1) % numPhilosophers]);
                philosophersList.add(philosopher);  // Add the philosopher to the list
                philosopher.start();  // Start the philosopher's thread
            }

        } catch (IOException e) {
            System.out.println("Error reading file: " + e.getMessage());  // Handle any file reading errors
        }
    }
}
