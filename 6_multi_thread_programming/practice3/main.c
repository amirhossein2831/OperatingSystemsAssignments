#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define SIZE 10

typedef struct {
    int *input;
    int *output;
    int start;
    int end;
    int offset;
} SummationThreadArgs;

void* cal(void* args) {
    SummationThreadArgs* threadArgs = (SummationThreadArgs*)args;

    threadArgs->output = malloc(sizeof(int) * (threadArgs->end - threadArgs->start));
    if (!threadArgs->output) {
        perror("malloc failed");
        pthread_exit(NULL);
    }

    threadArgs->output[0] = threadArgs->input[threadArgs->start] + threadArgs->offset;
    for (int i = threadArgs->start + 1; i < threadArgs->end; i++) {
        threadArgs->output[i - threadArgs->start] = threadArgs->output[i - threadArgs->start - 1] + threadArgs->input[i];
    }

    pthread_exit(NULL);
}

int main() {
    int input[SIZE] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int mid = SIZE / 2;

    SummationThreadArgs args1 = {input, NULL, 0, mid, 0};
    SummationThreadArgs args2 = {input, NULL, mid, SIZE, 0};
    pthread_t thread1, thread2;

    if (pthread_create(&thread1, NULL, cal, &args1) != 0) {
        perror("Failed to create thread1");
        return 1;
    }
    if (pthread_join(thread1, NULL) != 0) {
        perror("Failed to join thread1");
        return 1;
    }

    args2.offset = args1.output[mid - 1];

    if (pthread_create(&thread2, NULL, cal, &args2) != 0) {
        perror("Failed to create thread2");
        return 1;
    }
    if (pthread_join(thread2, NULL) != 0) {
        perror("Failed to join thread2");
        return 1;
    }

    printf("Thread 1 results: ");
    for (int i = 0; i < mid; i++) {
        printf("%d ", args1.output[i]);
    }
    printf("\n");

    printf("Thread 2 results: ");
    for (int i = 0; i < SIZE - mid; i++) {
        printf("%d ", args2.output[i]);
    }

    free(args1.output);
    free(args2.output);
    return 0;
}
