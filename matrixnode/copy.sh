#/bin/bash

echo "begin copy the execute to node2 and master..."

scp matrixmpi_nodeapi fangling@master:/home/fangling/matrix_mult/matrixnode/
scp matrixmpi_nodeapi fangling@node2:/home/fangling/matrix_mult/matrixnode/

echo "finish..."
