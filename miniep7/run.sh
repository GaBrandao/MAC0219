#!/usr/bin/env bash

FILE="results.txt"

> $FILE
echo size,best_avg,avg_avg,worst_avg >> $FILE
for ((i = 2; i <= 8; i+=2)); do
    # echo "Results for $i procs" >> $FILE
    mpirun --oversubscribe -np $i mpi_bandwidth >> $FILE
done

