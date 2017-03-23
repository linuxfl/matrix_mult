#/bin/bash

echo "begin to test!!!"
process_array=(4 6 8 10 12 14 16 18 20)
martix_size=(500 1000 1500 2000 2500 3000 3500 4000)

for((i=0;i<${#martix_size[@]};i++))
do
	for((j=0;j<${#process_array[@]};j++))
	do
		/home/fangling/mpi/bin/mpirun -f hostfile -np ${process_array[$j]} ./matrixmpi_scatter ${martix_size[$i]}
		echo finish martix size:${martix_size[$i]} process:${process_array[$j]} 
	done
done

echo "finish ... "
