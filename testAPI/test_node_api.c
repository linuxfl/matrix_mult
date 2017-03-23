#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"
#include "MPI_node.h"

int main(int argc,char **argv)
{
	double startTime,endTime;
	int i,j,k,line,namelen,temp;
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	int rank,size;
	MPI_Status status;
	
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	MPI_Get_processor_name(processor_name,&namelen);
	MPIN_init("hostfile",size);
	
	if(rank == 0){
	printf("MPIN_get_node_size: %d\n",MPIN_get_node_size());
	printf("MPIN_get_node_process_size: %d\n",MPIN_get_node_process_size(1));
	printf("MPIN_get_master_rank: %d\n",MPIN_get_master_rank(1));
	printf("MPIN_get_node_process_rank: %d\n",MPIN_get_node_process_rank(0,2));
	printf("MPIN_get_next_rank: %d\n",MPIN_get_next_rank(rank));
	}
	MPI_Finalize();
	return 0;
}
