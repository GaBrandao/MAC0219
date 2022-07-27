#! /bin/bash

set -o xtrace

MEASUREMENTS=15

# > results/mandelbrot_ompi
# for ((procs=9; procs<=65; procs+=8)); do
#     for ((i=0; i<15; i++)); do
#         mpirun --oversubscribe -np $procs ompi >> results/mandelbrot_ompi
#     done
# done

# rm output.ppm

for ((procs=4; procs<=64; procs*=2)); do
    threads=1
    while [ $(($procs * $threads)) -le 64 ]
    do
        for ((i=0; i<15; i++)); do
            mpirun --oversubscribe -np $(($procs + 1)) ompi_pth $threads >> results/mandelbrot_ompi_pth_1
        done
        ((threads*=2))
    done
done