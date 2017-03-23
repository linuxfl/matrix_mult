#/bin/bash

echo "now copy the execute to the node1 and master..."

scp matrixmpi_scatter fangling@node2:/home/fangling/matrix_mult/matrixpallel
scp matrixmpi_scatter fangling@master:/home/fangling/matrix_mult/matrixpallel

echo "finish ... "
