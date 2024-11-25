#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <time.h>

// Shared memory size for 1 integer and 1 flag
#define SHM_SIZE (2 * sizeof(int))
// Shared memory key
#define SHM_KEY 1234

void produce(int *number, int *flag) {
    srand(time(NULL));
    for (int i = 0; i < 100; i++) {

        // Wait until the consumer has consumed the previous number
        while (*flag == 1) {
            usleep(100);
        }

        // Generate a random number between 0 and 99
        *number = rand() % 100;
        *flag = 1;
        printf("Produced: %d\n", *number);
        usleep(1000000);
    }
}

int main() {
    // Create or attach to the shared memory segment using a fixed key
    int shm_id = shmget(SHM_KEY, SHM_SIZE, IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("Failed to allocate shared memory");
        exit(1);
    }

    // Attach shared memory
    int *shared_memory = (int *) shmat(shm_id, NULL, 0);
    if (shared_memory == (int *) -1) {
        perror("Failed to attach shared memory");
        exit(1);
    }

    // Pointer to shared memory for the number and flags (1: ready, 0: not ready)
    int *number = &shared_memory[0];
    int *flag = &shared_memory[1];

    // Initially, no number is written
    *flag = 0;

    // Produce 100 numbers
    produce(number, flag);

    // Detach shared memory
    shmdt(shared_memory);

    return 0;
}
