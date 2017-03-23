#/bin/bash

echo "complie ..."

/home/fangling/mpi/bin/mpicc matrixmpi.c -fopenmp -o matrixmpi

echo "finish..."
