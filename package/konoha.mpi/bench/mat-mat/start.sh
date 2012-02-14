#!/bin/sh

mpicc -O3 ./src/parallel.c -o ./src/parallel
mpicc -O3 ./src/single.c -o ./src/single

single=./single.sh
parallel=./parallel.sh
for numpe in 1 2 4 6 8 12
do
    pe_name=openmpi${numpe}
    for numnodes in 1 2 3 4
    do
        numprocs=`expr ${numpe} \* ${numnodes}`
        job_name=mm_${numpe}x${numnodes}
        if [ ${numprocs} = 1 ]; then
            qsub -pe ${pe_name} ${numprocs} -N ${job_name} ${single}
        elif [ `expr ${numprocs} \% 2` = 0 ]; then
            qsub -pe ${pe_name} ${numprocs} -N ${job_name} ${parallel}
        fi
    done
done
