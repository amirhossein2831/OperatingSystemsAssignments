#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <stdatomic.h>

#define HIST_SIZE 25
#define NUM_PROCESSES 5

void printArray(int *hist) {
    printf("[");
    for (int i = 0; i < 25; i++) {
        printf(" %d:%d ", i -12, hist[i]);
        if (i  != 24)
            printf(",");

    }
    printf("]\n");
}

void printHistogram(int* hist) {
    for (int i = 0; i < HIST_SIZE; i++) {
        printf("%2d: ", i - 12);
        for (int j = 0; j < hist[i]; j++) {
            printf("*");
        }
        printf("\n");
    }
}

void performTask(_Atomic int* shared_hist, int iterations) {
    for (int i = 0; i < iterations; i++) {
        int count = 0;
        for (int j = 0; j < 12; ++j) {
            int random_number = rand() % 100;
            if (random_number < 49)
                count--;
            else
                count++;
        }

        atomic_fetch_add(&shared_hist[count + 12], 1);
    }
}

int main(void) {
    // create shared memory ,make it atomic, and initialize to 0
    int shm_id = shmget(IPC_PRIVATE, HIST_SIZE * sizeof(_Atomic int), IPC_CREAT | 0666);
    if (shm_id < 0) {
        perror("shmget failed");
        exit(EXIT_FAILURE);
    }

    _Atomic int* shared_hist = (_Atomic int*)shmat(shm_id, NULL, 0);
    if (shared_hist == (_Atomic int*)-1) {
        perror("shmat failed");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < HIST_SIZE; i++) {
        shared_hist[i] = 0;
    }

    // start main process
    srand(time(NULL));
    clock_t start = clock();

    for (int process_id = 0; process_id < NUM_PROCESSES; process_id++) {
        pid_t pid = fork();

        if (pid < 0) {
            perror("Fork failed");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            performTask(shared_hist, 100000);
            exit(EXIT_SUCCESS);
        }
    }

    // wait for finish child process
    for (int i = 0; i < NUM_PROCESSES; i++) {
        wait(NULL);
    }

    clock_t end = clock();

    // print duration
    double duration = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Execution Time: %.6f ms\n", duration * 1000);

    // print hist arr
    for (int i = 0; i < 25; ++i) {
        printf("%d ", shared_hist[i]);
    }
    printf("\n");

    // print histogram
    printHistogram((int*)shared_hist);

    // free memory
    shmdt(shared_hist);
    shmctl(shm_id, IPC_RMID, NULL);

    return 0;
}
