#!/bin/sh

#$ -S /bin/sh
#$ -cwd
#$ -V
#$ -pe openmpi1 2

echo "---- c ------------------------------"
mpirun -np $NSLOTS ./src/float

echo "---- konoha -------------------------"
mpirun -np $NSLOTS mpikonoha ./src/float.k 2> /dev/null

echo "---- llvm ---------------------------"
export LLVM_OS_ARCH="so"
export LLVM_ENABLE_OPT=true
export LLVM_ENABLE_OPT_INTRINSIC_METHOD=true
export LLVM_ENABLE_OPT_SAFEPOINT=true
export LLVM_ENABLE_OPT_ARRAY_ACCESS=true
export LLVM_ENABLE_OPT_OBJECT_NEW=true
export LLVM_ENABLE_OPT_FIELD_ACCESS=true
export LLVM_ENABLE_OPT_REMOVE_SFP=true
mpirun -np $NSLOTS konohac --emit-llvm --run ./src/float.k 2> /dev/null

echo "---- python -------------------------"
mpirun -np $NSLOTS python ./src/float.py

echo "---- ruby ---------------------------"
mpirun -np $NSLOTS mpi_ruby ./src/float.rb
