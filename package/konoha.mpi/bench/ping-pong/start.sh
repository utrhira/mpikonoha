#!/bin/sh

if [ ! -e ./src/int ]; then
    echo "--- build src/int"
    mpicc -O3 ./src/int.c -o ./src/int
fi
if [ ! -e ./src/float ]; then
    echo "--- build src/float"
    mpicc -O3 ./src/float.c -o ./src/float
fi

qsub -N ppInt ./int.sh
qsub -N ppFloat ./float.sh

rm ./src/int ./src/float
