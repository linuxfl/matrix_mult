#/bin/bash

echo "begin to complie..."

/home/fangling/mpi/bin/mpicc ../nodeAPI/MPI_node.c matrixmpi_nodeapi.c -fopenmp -o matrixmpi_nodeapi


