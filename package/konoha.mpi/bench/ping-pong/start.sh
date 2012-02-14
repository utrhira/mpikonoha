#!/bin/sh

mpicc -O3 ./src/int.c -o ./src/int
mpicc -O3 ./src/float.c -o ./src/float

qsub -N ppInt ./int.sh
qsub -N ppFloat ./float.sh
