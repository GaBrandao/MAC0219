#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

pthread_t threads[4];
int A_4[4], A_8[8], A_16[16], A_32[32], A_64[64], A_128[128];

struct argument {
    int *A;
    int idx;
};

void *increment(void *arguments) {
    struct argument *args = (struct argument *)arguments;

    for (int i = 0; i < 100000000; ++i)
        (args->A)[args->idx]++;
}

int main() {
    clock_t start, end;
    double cpu_time_used;

    start = clock();
    for(int i = 0; i < 4; ++i) {
        struct argument args;
        args.A = A_4;
        args.idx = i;
        pthread_create(&(threads[i]), NULL, increment, (void *)&args);
    }

    for(int i = 0; i < 4; ++i) {
        pthread_join(threads[i], NULL);
    }
    end= clock();

    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time: %lf\n", cpu_time_used);


    start = clock();
    for(int i = 0; i < 4; ++i) {
        struct argument args;
        args.A = A_8;
        args.idx = i<<1;
        pthread_create(&(threads[i]), NULL, increment, (void *)&args);
    }

    for(int i = 0; i < 4; ++i) {
        pthread_join(threads[i], NULL);
    }
    end= clock();

    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time: %lf\n", cpu_time_used);


    start = clock();
    for(int i = 0; i < 4; ++i) {
        struct argument args;
        args.A = A_16;
        args.idx = i<<2;
        pthread_create(&(threads[i]), NULL, increment, (void *)&args);
    }

    for(int i = 0; i < 4; ++i) {
        pthread_join(threads[i], NULL);
    }
    end= clock();

    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time: %lf\n", cpu_time_used);


    start = clock();
    for(int i = 0; i < 4; ++i) {
        struct argument args;
        args.A = A_32;
        args.idx = i<<3;
        pthread_create(&(threads[i]), NULL, increment, (void *)&args);
    }

    for(int i = 0; i < 4; ++i) {
        pthread_join(threads[i], NULL);
    }
    end= clock();

    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time: %lf\n", cpu_time_used);

    start = clock();
    for(int i = 0; i < 4; ++i) {
        struct argument args;
        args.A = A_64;
        args.idx = i<<4;
        pthread_create(&(threads[i]), NULL, increment, (void *)&args);
    }

    for(int i = 0; i < 4; ++i) {
        pthread_join(threads[i], NULL);
    }
    end= clock();

    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time: %lf\n", cpu_time_used);

    start = clock();
    for(int i = 0; i < 4; ++i) {
        struct argument args;
        args.A = A_128;
        args.idx = i<<5;
        pthread_create(&(threads[i]), NULL, increment, (void *)&args);
    }

    for(int i = 0; i < 4; ++i) {
        pthread_join(threads[i], NULL);
    }
    end= clock();

    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time: %lf\n", cpu_time_used);
}


