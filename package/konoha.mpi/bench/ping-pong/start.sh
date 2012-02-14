#!/bin/sh

mpicc -O3 ./src/int.c -o ./src/int
mpicc -O3 ./src/float.c -o ./src/float

qsub -N pp_int ./int.sh
qsub -N pp_float ./float.sh
