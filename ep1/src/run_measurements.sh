#! /bin/bash

set -o xtrace

MEASUREMENTS=10

NAMES=('mandelbrot_seq' 'mandelbrot_pth' 'mandelbrot_omp')

make
mkdir results

for NAME in ${NAMES[@]}; do
    > results/$NAME
    for ((size=16; size<=8192; size*=2)); do
        for ((threads=1; threads<=32; threads*=2)); do
            for ((i=0; i<10; i++)); do
                echo -n '1,' >> results/$NAME
                ./$NAME -2.5 1.5 -2.0 2.0 $size $threads >> results/$NAME 
                echo -n '2,' >> results/$NAME
                ./$NAME -0.8 -0.7 0.05 0.15 $size $threads >> results/$NAME
                echo -n '3,' >> results/$NAME
                ./$NAME 0.175 0.375 -0.1 0.1 $size $threads >> results/$NAME 
                echo -n '4,' >> results/$NAME
                ./$NAME -0.188 -0.012 0.554 0.754 $size $threads >> results/$NAME 
            done

            if [[ "$NAME" == "mandelbrot_seq" ]]; then
                break
            fi 
        done
    done
done

rm output.ppm