#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>


#define HIST_SIZE 25
#define ITERATION_NUMBER 50000

void printArray(int *hist) {
    printf("[");
    for (int i = 0; i < HIST_SIZE; i++) {
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

int main(void) {
    // initial 
    struct timeval start, end;
    int hist[HIST_SIZE] = {0};

    // main logic
    srand(time(NULL));
    gettimeofday(&start, NULL);

    for (int i = 0; i < ITERATION_NUMBER; i++) {
        int count = 0;
        for (int i = 0; i < 12; ++i) {
            int random_number = rand() % 100;
            if (random_number < 49)
                count--;
            else
                count++;
        }
        hist[count + 12]++;
    }

    gettimeofday(&end, NULL);

    // print duration
    double duration = (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_usec - start.tv_usec) / 1000.0;
    printf("Operation Number: %d| Execution Time: %.6f ms \n", ITERATION_NUMBER, duration);

    // print hist arr
    printArray(hist);

    // print histogram
    printHistogram(hist);
    return 0;
}
