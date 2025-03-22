// Project 1 OS Scheduler (FCFS and SJF)
// Group Members: Louie Jack Bearden IV
// March 21 2025
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define MAX_PROCESSES 100

// Structure to hold process information
typedef struct {
    int pid;            // Process ID
    int arrival_time;   // Time the process arrives
    int burst_time;     // Time the process needs to execute
    int waiting_time;   // Time the process waits before execution
    int turnaround_time;// Total time from arrival to completion
    int completion_time;// Time the process finishes execution
} Process;

// Function to read process data from a file
void readProcesses(Process processes[], int* n) {
    FILE* file = fopen("processes.txt", "r");  // Open file for reading

    if (!file) {
        printf("Error: Could not open processes.txt\n");
        exit(1);  // Exit if the file couldn't be opened
    }

    char line[100];
    fgets(line, sizeof(line), file); // Skip the header line in the file

    *n = 0;  // Initialize the number of processes

    // Read process data from the file
    while (fscanf(file, "%d %d %d", &processes[*n].pid, &processes[*n].arrival_time,
        &processes[*n].burst_time) == 3) {
        processes[*n].waiting_time = 0;
        processes[*n].turnaround_time = 0;
        processes[*n].completion_time = 0;
        (*n)++;
    }

    fclose(file);  // Close the file
}

// Function to reset process information (waiting, turnaround, completion times)
void resetProcesses(Process processes[], int n) {
    // Initialize process metrics to zero before each scheduling algorithm execution.
    for (int i = 0; i < n; i++) {
        processes[i].waiting_time = 0;
        processes[i].turnaround_time = 0;
        processes[i].completion_time = 0;
    }
}

// Function to sort processes by arrival time
void sortByArrivalTime(Process processes[], int n) {
    // Sort processes based on arrival time for FCFS scheduling.
    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            if (processes[i].arrival_time > processes[j].arrival_time) {
                Process temp = processes[i];
                processes[i] = processes[j];
                processes[j] = temp;
            }
        }
    }
}

// Function to sort processes for SJF (by burst time) with arrival time in mind
void sortByBurstTime(Process processes[], int n, int current_time) {
    // Sorts the processes that have arrived but are not completed based on burst time.
    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            if (processes[i].arrival_time <= current_time && processes[j].arrival_time <= current_time &&
                processes[i].completion_time == 0 && processes[j].completion_time == 0) {
                if (processes[i].burst_time > processes[j].burst_time) {
                    Process temp = processes[i];
                    processes[i] = processes[j];
                    processes[j] = temp;
                }
                else if (processes[i].burst_time == processes[j].burst_time &&
                    processes[i].arrival_time > processes[j].arrival_time) {
                    Process temp = processes[i];
                    processes[i] = processes[j];
                    processes[j] = temp;
                }
            }
        }
    }
}

// Function to implement the First-Come, First-Served (FCFS) scheduling algorithm
void FCFS(Process processes[], int n) {
    int current_time = 0;          // Keep track of the current time
    int total_waiting_time = 0;
    int total_turnaround_time = 0;

    // Structure to track Gantt chart entries
    typedef struct {
        int pid;
        int start;
        int end;
    } GanttEntry;
    GanttEntry gantt[MAX_PROCESSES];
    int gantt_count = 0;

    sortByArrivalTime(processes, n); // Sort processes by arrival time

    printf("\nFCFS Gantt Chart:\n|");

    // Simulate the execution of processes in FCFS order and build the Gantt chart
    for (int i = 0; i < n; i++) {
        // Store the start time and process ID for the Gantt chart
        gantt[gantt_count].start = current_time;
        gantt[gantt_count].pid = processes[i].pid;

        // Calculate waiting, turnaround, and completion times for the current process
        processes[i].waiting_time = current_time - processes[i].arrival_time;
        processes[i].turnaround_time = processes[i].waiting_time + processes[i].burst_time;
        processes[i].completion_time = current_time + processes[i].burst_time;

        // Store the end time for the Gantt chart
        gantt[gantt_count].end = processes[i].completion_time;
        gantt_count++;

        // Accumulate the total waiting and turnaround times for calculating the averages later
        total_waiting_time += processes[i].waiting_time;
        total_turnaround_time += processes[i].turnaround_time;

        // Update the current time to the completion time of the current process
        current_time = processes[i].completion_time;

        // Print the process ID in the Gantt chart format
        printf(" P%d |", processes[i].pid);
    }


    // Print timeline
    printf("\n%d", gantt[0].start);
    for (int i = 0; i < gantt_count; i++) {
        int space = 5;  // Base spacing between numbers
        if (i > 0) {
            space += (gantt[i].start >= 10) - (gantt[i - 1].end >= 10);
        }
        printf("%*d", space, gantt[i].end);
    }

    // Calculate and print averages
    float avg_waiting_time = (float)total_waiting_time / n;
    float avg_turnaround_time = (float)total_turnaround_time / n;

    printf("\n\nProcess | Waiting Time | Turnaround Time\n");
    for (int i = 0; i < n; i++) {
        printf("P%d      | %d            | %d\n",
            processes[i].pid,
            processes[i].waiting_time,
            processes[i].turnaround_time);
    }

    printf("\nAverage Waiting Time: %.2f\n", avg_waiting_time);
    printf("Average Turnaround Time: %.2f\n", avg_turnaround_time);
}

// Function to implement the Shortest Job First (SJF) scheduling algorithm + Gantt chart
void SJF(Process processes[], int n) {
    int current_time = 0;
    int total_waiting_time = 0;
    int total_turnaround_time = 0;
    int completed_processes = 0;

    typedef struct {
        int pid;
        int start;
        int end;
    } GanttEntry;

    GanttEntry gantt[MAX_PROCESSES];
    int gantt_count = 0;

    // Main loop to execute processes until all processes are completed
    while (completed_processes < n) {
        int shortest_job = -1;
        int shortest_burst = INT_MAX;

        // Find the process with the shortest burst time that has arrived and is not yet completed
        for (int i = 0; i < n; i++) {
            if (processes[i].arrival_time <= current_time &&
                processes[i].completion_time == 0) {
                if (processes[i].burst_time < shortest_burst) {
                    shortest_burst = processes[i].burst_time;
                    shortest_job = i;
                }
            }
        }

        if (shortest_job == -1) {
            // If no process meets the criteria, move the current time forward.
            current_time++;
        }
        else {
            // A process is found, so execute it:
            Process* p = &processes[shortest_job]; // Get a pointer to the shortest job.
            gantt[gantt_count].start = current_time; // Record start time for the Gantt chart
            gantt[gantt_count].pid = p->pid; //Record process id for gantt chart

            // Calculate completion, waiting, and turnaround times for the process.
            p->completion_time = current_time + p->burst_time;
            p->waiting_time = current_time - p->arrival_time;
            p->turnaround_time = p->completion_time - p->arrival_time;

            gantt[gantt_count].end = p->completion_time; //Record end time for gantt chart
            gantt_count++; //increment the gantt chart counter

            // Update totals
            total_waiting_time += p->waiting_time;
            total_turnaround_time += p->turnaround_time;

            // Update current time and increment completed processes.
            current_time = p->completion_time;
            completed_processes++;
        }

    }

    printf("\nSJF Gantt Chart:\n");

    // Print process IDs
    printf("|");
    for (int i = 0; i < gantt_count; i++) {
        printf(" P%d |", gantt[i].pid);
    }
    printf("\n");

    // Print timeline
    printf("%d", gantt[0].start);
    for (int i = 0; i < gantt_count; i++) {
        int space = 5;  // Default space
        if (i > 0) {
            space += (gantt[i].start >= 10) - (gantt[i - 1].end >= 10);
        }
        printf("%*d", space, gantt[i].end);
    }

    // Calculate averages and print results
    float avg_waiting_time = (float)total_waiting_time / n;
    float avg_turnaround_time = (float)total_turnaround_time / n;

    printf("\n\nProcess | Waiting Time | Turnaround Time\n");
    for (int i = 0; i < n; i++) {
        printf("P%d      | %d            | %d\n",
            processes[i].pid,
            processes[i].waiting_time,
            processes[i].turnaround_time);
    }

    printf("\nAverage Waiting Time: %.2f\n", avg_waiting_time);
    printf("Average Turnaround Time: %.2f\n", avg_turnaround_time);
}

// Main function of the program
int main() {
    Process processes[MAX_PROCESSES];
    int n;

    // Read processes from file
    readProcesses(processes, &n);

    int choice;
    int running = 1;

    // Main loop to present the menu and execute scheduling algorithms
    while (running) {
        resetProcesses(processes, n);  // Reset process information

        sortByArrivalTime(processes, n); // Sort processes by arrival time

        printf("\nSelect Scheduling Algorithm:\n");
        printf("1. FCFS (First-Come, First-Served)\n");
        printf("2. SJF (Shortest Job First)\n");
        printf("3. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
        case 1:
            FCFS(processes, n); // Execute FCFS scheduling
            break;
        case 2:
            SJF(processes, n);  // Execute SJF scheduling
            break;
        case 3:
            running = 0;        // Exit the loop
            break;
        default:
            printf("Invalid choice! Please enter 1, 2, or 3.\n");
        }
    }

    return 0;
}
