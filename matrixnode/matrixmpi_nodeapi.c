#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include "mpi.h"
//#define MAX 20

int main(int argc,char **argv)
{
	int MAX;
	if(argc < 2){
		printf("args error!\n");
		return ;
	}else{
		MAX = atoi(argv[1]);
	}
	double startTime,endTime;
	double startTime1,endTime1;
	int i,j,k,line,temp;
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	/*float a[MAX*MAX],b[MAX*MAX],c[MAX*MAX];*/
	float *a,*b;
	int rank,size,node_size;
	MPI_Status status;
	startTime1 = omp_get_wtime();

	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	MPIN_init("hostfile",size);
	if(MAX/(size - 1) == 0)
		line = 1;
	else line = MAX/(size - 1);
	if(rank == 0) 
	{
	//	printf("===============start=================\n");
		FILE *fp;
		fp = fopen("./nodeapi_test_result","a+");
		int master_rank,process_size,data_size = 0,child_rank,node_id;
		a = (float *)malloc(sizeof(float)*MAX*MAX);
		b = (float *)malloc(sizeof(float)*MAX*MAX);
		float *ans;
		ans = (float *)malloc(sizeof(float)*MAX*MAX);
		memset(ans,0,sizeof(float)*MAX*MAX);
	//	printf("===================1=========================\n");
		for(i=0;i<MAX;i++){
			for(j=0;j<MAX;j++)
			{
				a[i*MAX+j] = 1.0;
				b[i*MAX+j] = 1.0;
				//c[i*MAX+j] = 0.0;
			}
		}
		startTime = MPI_Wtime();
		/*向从进程广播数组b*/
	//	printf("==================2===========================\n");
		MPI_Bcast(b,MAX*MAX,MPI_FLOAT,0,MPI_COMM_WORLD);
	//	printf("==================3===========================\n");
		/*向每一个从进程发送连续的MAX*LINE长度的数据*/
		node_size = MPIN_get_node_size();
	//	printf("=======================>rank %d,node_size %d\n",rank,node_size);
		//将数据发送到每个节点的master进程
		for(i = 1;i < node_size;i++)
		{
			master_rank = MPIN_get_master_rank(i);
			process_size = MPIN_get_node_process_size(i);
			MPI_Send(a+data_size,process_size * line*MAX,MPI_FLOAT,master_rank,0,MPI_COMM_WORLD);
			data_size += process_size * line*MAX;
		}
	//	把本进程的数据发给节点内其他进程
		int mastersize = MPIN_get_node_process_size(0);
	//	printf("=======================>rank %d mastersize %d\n",rank,mastersize);
		for(i = 1;i < mastersize;i++)
		{
			child_rank = MPIN_get_node_process_rank(0,i);
	//		printf("=======================>rank %d mastersize %d childrank %d\n",rank,mastersize,child_rank);
			MPI_Send(a+data_size+(i-1)*line*MAX,line*MAX,MPI_FLOAT,child_rank,0,MPI_COMM_WORLD);
		}
		for(i = (size-1)*line;i<MAX;i++){
			for(j=0;j<MAX;j++)
			{
				temp = 0;
				for(k=0;k<MAX;k++)
					temp+=b[j+k*MAX]*a[k+j*MAX];
				ans[i*MAX+j] = temp;
			}
		}
		data_size = 0;
	//	printf("=======================>rank %d -----4\n",rank);
		for(i = 1;i < node_size;i++)
		{
			master_rank = MPIN_get_master_rank(i);
			process_size = MPIN_get_node_process_size(i);
			MPI_Recv(ans+data_size,process_size*line*MAX,MPI_FLOAT,master_rank,0,MPI_COMM_WORLD,&status);
			data_size += process_size*line*MAX;
		}
	//	printf("=======================>rank %d------5\n",rank);
		for(i = 1;i < mastersize;i++)
		{
			child_rank = MPIN_get_node_process_rank(0,i);
			MPI_Recv(ans+data_size+(i-1)*line*MAX,line*MAX,MPI_FLOAT,child_rank,0,MPI_COMM_WORLD,&status);
		}
	//	printf("=======================>rank %d-------6\n",rank);
		endTime = MPI_Wtime();
		endTime1 = omp_get_wtime();
	/*	for(i=0;i<MAX*MAX;i++)
		{
			printf("%4.0f ",ans[i]);
			if((i+1)%MAX == 0)
				printf("\n");
		}*/
		fprintf(fp,"matrix size %d,processor %d\n",MAX,size);
		fprintf(fp,"the total compute time is %f\n",endTime-startTime);
		fprintf(fp,"the total time is %f\n",endTime1-startTime1);
	}else{
		int k,temp,i,j;
		int child_rank;
		b = (float *)malloc(sizeof(float)*MAX*MAX);
		MPI_Bcast(b,MAX*MAX,MPI_FLOAT,0,MPI_COMM_WORLD);
		int node_id = MPIN_get_node_by_rank(rank);
		int master_rank = MPIN_get_master_rank(node_id);
		if(rank == master_rank){
	//		printf("================master_rank %d=======>rank %d---else----1\n",master_rank,rank);
			int psize = MPIN_get_node_process_size(node_id);
			int data_size = line * psize *MAX;
			float *buff = (float *)malloc(data_size*sizeof(float));
			float *myans = (float *)malloc(line*MAX*sizeof(float));
			float *ans = (float *)malloc(data_size*sizeof(float));
			memset(buff,0,sizeof(float)*data_size);
			memset(ans,0,sizeof(float)*data_size);
			memset(myans,0,sizeof(float)*line*MAX);
			
			MPI_Recv(buff,data_size,MPI_FLOAT,0,0,MPI_COMM_WORLD,&status);
	//		printf("=======================>rank %d---else----2\n",rank);
			//把数据块分块分给子进程
			for(i = 1;i < psize;i++){
				child_rank = MPIN_get_node_process_rank(node_id,i);
				MPI_Send(buff+i*line*MAX,line*MAX,MPI_FLOAT,child_rank,0,MPI_COMM_WORLD);
			}
	//		printf("=======================>rank %d---else----3\n",rank);
			//计算自已的部分
			for(k=0;k<line;k++)
			{
				for(i=0;i<MAX;i++)
				{
					temp = 0;
					for(j=0;j<MAX;j++)
						temp += b[i+j*MAX]*buff[j+k*MAX];
					myans[k*MAX+i] = temp;
				}
			}
			for(i = 0;i < line*MAX;i++)
				ans[i] = myans[i];
			//接收子进程的结果
			for(i = 1;i < psize;i++){
				child_rank = MPIN_get_node_process_rank(node_id,i);
				MPI_Recv(myans,line*MAX,MPI_FLOAT,child_rank,0,MPI_COMM_WORLD,&status);
				for(k=0;k<line;k++)
				{
					for(j=0;j<MAX;j++)
						ans[((i)*line+k)*MAX+j]=myans[k*MAX+j];
				}
			}
	//		printf("=======================>rank %d---else----4\n",rank);
			//发送结果给主进程
			MPI_Send(ans,data_size,MPI_FLOAT,0,0,MPI_COMM_WORLD);
	//		printf("=======================>rank %d---else----5\n",rank);
		}else{
			float *buff = (float *)malloc(line*MAX*sizeof(float));
			float *ans = (float *)malloc(line*MAX*sizeof(float));
			//接收master进程的数据
	//		printf("==================masterrank %d=====>rank %d---else----6\n",master_rank,rank);
			MPI_Recv(buff,line*MAX,MPI_FLOAT,master_rank,0,MPI_COMM_WORLD,&status);
			//进行计算
			for(k=0;k<line;k++)
			{
				for(i=0;i<MAX;i++)
				{
					temp = 0;
					for(j=0;j<MAX;j++)
						temp += b[i+j*MAX]*buff[j+k*MAX];
					ans[k*MAX+i] = temp;
				}
			}
			//把计算结果发给master
	//		printf("=======================>rank %d---else----7\n",rank);
			MPI_Send(ans,line*MAX,MPI_FLOAT,master_rank,0,MPI_COMM_WORLD);
		}
	}
	MPI_Finalize();
	return 0;
}
