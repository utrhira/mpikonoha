#!/bin/sh

mpicc -O3 ./src/parallel.c -o ./src/parallel
mpicc -O3 ./src/single.c -o ./src/single

qsub -pe openmpi1 1  -N mm_1x1  ./single.sh
qsub -pe openmpi1 2  -N mm_1x2  ./parallel.sh
qsub -pe openmpi1 4  -N mm_1x4  ./parallel.sh
qsub -pe openmpi1 6  -N mm_1x6  ./parallel.sh
qsub -pe openmpi1 8  -N mm_1x8  ./parallel.sh
qsub -pe openmpi1 12 -N mm_1x12 ./parallel.sh

qsub -pe openmpi2 2  -N mm_2x1  ./parallel.sh
qsub -pe openmpi2 4  -N mm_2x2  ./parallel.sh
qsub -pe openmpi2 8  -N mm_2x4  ./parallel.sh
qsub -pe openmpi2 12 -N mm_2x6  ./parallel.sh
qsub -pe openmpi2 16 -N mm_2x8  ./parallel.sh
qsub -pe openmpi2 24 -N mm_2x12 ./parallel.sh

qsub -pe openmpi3 3  -N mm_3x1  ./parallel.sh
qsub -pe openmpi3 6  -N mm_3x2  ./parallel.sh
qsub -pe openmpi3 12 -N mm_3x4  ./parallel.sh
qsub -pe openmpi3 18 -N mm_3x6  ./parallel.sh
qsub -pe openmpi3 24 -N mm_3x8  ./parallel.sh
qsub -pe openmpi3 36 -N mm_3x12 ./parallel.sh

qsub -pe openmpi4 4  -N mm_4x1  ./parallel.sh
qsub -pe openmpi4 8  -N mm_4x2  ./parallel.sh
qsub -pe openmpi4 16 -N mm_4x4  ./parallel.sh
qsub -pe openmpi4 24 -N mm_4x6  ./parallel.sh
qsub -pe openmpi4 32 -N mm_4x8  ./parallel.sh
qsub -pe openmpi4 48 -N mm_4x12 ./parallel.sh
