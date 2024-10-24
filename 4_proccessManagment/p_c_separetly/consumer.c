#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>

// Shared memory size for 1 integer and 1 flag
#define SHM_SIZE (2 * sizeof(int))
// Shared memory key
#define SHM_KEY 1234

int consumer(int *number, int *flag) {
    int sum = 0;
    for (int i = 0; i < 100; i++) {

        // Wait for the producer to write a number
        while (*flag == 0) {
            usleep(100);
        }

        // Consume the number
        sum += *number;
        printf("consumed\n");
        *flag = 0;

        usleep(1000000);

    }

    return sum;
}

int main() {
    // Attach to the existing shared memory segment using the fixed key
    int shm_id = shmget(SHM_KEY, SHM_SIZE, 0666);
    if (shm_id == -1) {
        perror("Failed to attach to shared memory");
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

    // Consume 100 numbers
    int sum = consumer(number, flag);

    // Print the result
    printf("Total Sum: %d\n", sum);

    // Detach shared memory
    shmdt(shared_memory);

    // Remove the shared memory segment (cleanup happens here)
    shmctl(shm_id, IPC_RMID, NULL);

    return 0;
}
