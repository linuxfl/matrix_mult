#/bin/bash

echo "complie ..."

/home/fangling/mpi/bin/mpicc matrixmpi_scatter.c -fopenmp -o matrixmpi_scatter

echo "finish..."
