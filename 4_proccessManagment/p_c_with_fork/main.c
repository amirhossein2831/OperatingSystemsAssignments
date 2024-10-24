#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <time.h>

// Shared memory to store 1 integer and 1 flaG
#define SHM_SIZE (2*sizeof(int))

int consumer(int *number, int *flag) {
    int sum = 0;
    for (int i = 0; i < 100; i++) {

        // Wait for the producer to write a number with a Short sleep to avoid busy waiting
        while (*flag == 0) {
            usleep(100);
        }

        // Mark the number as consumed
        *flag = 0;
        printf("consumed\n");
        sum += *number;
    }

    return sum;
}

void produce(int *number, int *flag) {
    srand(time(NULL));
    for (int i = 0; i < 100; i++) {

        // Wait until the consumer has consumed the number with a  Short sleep to avoid busy waiting
        while (*flag == 1) {
            usleep(100);
        }

        // Generate a random number between 0 and 99 and Mark the number as ready for consumption
        *number = rand() % 100;
        *flag = 1;
        printf("Produced: %d\n", *number);
    }

}

int main() {
    // Create shared memory segment
    int shared_memory_id = shmget(IPC_PRIVATE, SHM_SIZE, IPC_CREAT | 0666);
    if (shared_memory_id == -1) {
        perror("Failed to allocate shared memory");
        exit(1);
    }

    // Attach shared memory
    int *shared_memory = (int *) shmat(shared_memory_id, NULL, 0);
    if (shared_memory == (int *) -1) {
        perror("Failed to attach shared memory to process");
        exit(1);
    }

    // Pointer to shared memory for the number and flags (1: ready, 0: not ready)
    int *number = &shared_memory[0];
    int *flag = &shared_memory[1];

    // Initially, no number is written
    *flag = 0;

    // Fork a new process
    pid_t pid = fork();
    if (pid < 0) {
        perror("Failed to fork new process");
        exit(1);
    }

    if (pid == 0) {

        // Child process: Consumer
        int sum = consumer(number, flag);

        // Detach shared memory
        shmdt(shared_memory);

        // print the result
        printf("Total Sum: %d\n", sum);

        // finish the child process
        exit(0);
    } else {

        // Parent process: Producer
        produce(number, flag);

        // Wait for the consumer to finish
        wait(NULL);

        // Detach shared memory
        shmdt(shared_memory);

        // Remove shared memory segment
        shmctl(shared_memory_id, IPC_RMID, NULL);
    }

    return 0;
}
