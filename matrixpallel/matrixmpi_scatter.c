#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"
#include <omp.h>
#define RESULT_SEND_AND_RECIEVE 3
#define COLUMN_DATA_SEND_AND_RECIEVE 1
//#define MAX 6

int main(int argc,char **argv)
{
	int MAX;
	if(argc < 1)
		fprintf(stderr,"please input the size of matrix!!!\n");
	else
		MAX = atoi(argv[1]);
	
	double startTime,endTime;
	double startTime1,endTime1;
	int i,j,k,line,namelen,temp;
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	/*float a[MAX*MAX],b[MAX*MAX],c[MAX*MAX];*/
	float *a,*b,*c;
	int rank,size;
	MPI_Status status;
	startTime1 = omp_get_wtime();

	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
//	MPI_Get_processor_name(processor_name,&namelen);
	//fprintf(stdout,"Process %d of %d on %s\n",rank,size,processor_name);
	/*如果进程数大于要处理的数组列数则每个进程处理一列
	  如果进程数小于要处理的数组列数则每一个进程要处理MAX/size列*/
	if(MAX/size == 0)
		line = 1;
	else line = MAX/size;
	//rank = 0 设为主进程负责分发数组、收集结果和剩余列的处理
	if(rank == 0) 
	{
		FILE *fp;
		fp = fopen("./scatter_test_result","a+");
		float *ans;
		a = (float *)malloc(sizeof(float)*(MAX*MAX+line*MAX));
		b = (float *)malloc(sizeof(float)*MAX*MAX);
		c = (float *)malloc(sizeof(float)*MAX*MAX);
		ans = (float *)malloc(sizeof(float)*line*MAX);
		memset(ans,0,sizeof(float)*line*MAX);
		/*初始化数组*/
	/*	for(i=0;i<MAX;i++)
			for(j=0;j<MAX;j++)
			{
				//a[i*MAX+j] = (float)(i+1)*(j+1);
				//b[i*MAX+j] = (float)(i+1)*(j+1);
				if(i*MAX+j >= line*MAX){
					a[i*MAX+j] = 1.0;
					b[i*MAX+j] = 1.0;
				}else{
					a[i*MAX+j] = 0.0;
					b[i*MAX+j] = 0.0;
				}
				c[i*MAX+j] = 0.0;
			}*/
		for(i = 0;i< MAX*MAX+line*MAX;i++)
			a[i] = 1.0;
		for(j = 0;j< MAX*MAX;j++)
		{		
			b[j] = 1.0;
			c[j] = 0.0;
		}

/*		printf("array A is:\n");
		for(i=0;i<MAX*MAX;i++)
		{	
			printf("%4f ",a[i]);
			if((i+1)%MAX == 0)
				printf("\n");
		}
		printf("array B is:\n");
		for(i=0;i<MAX*MAX;i++)
		{	
			printf("%4f ",b[i]);
			if((i+1)%MAX == 0)
				printf("\n");
		}*/
		/*开始记时*/
		startTime = MPI_Wtime();
		/*向从进程广播数组b*/
		MPI_Bcast(b,MAX*MAX,MPI_FLOAT,0,MPI_COMM_WORLD);

		/*向每一个从进程发送连续的MAX*LINE长度的数据*/
	/*	for(i=1;i<size;i++)
		{
			MPI_Send(a+(i-1)*line*MAX,MAX*line,MPI_FLOAT,i,COLUMN_DATA_SEND_AND_RECIEVE,MPI_COMM_WORLD);
		}*/
		MPI_Scatter(a,line*MAX,MPI_FLOAT,ans,line*MAX,MPI_FLOAT,0,MPI_COMM_WORLD);

		/*接收每一个从进程发送的结果*/
		for(i=1;i<size;i++)
		{	
			MPI_Recv(ans,MAX*line,MPI_FLOAT,i,RESULT_SEND_AND_RECIEVE,MPI_COMM_WORLD,&status);
			for(k=0;k<line;k++)
			{
				for(j=0;j<MAX;j++)
					c[((i-1)*line+k)*MAX+j]=ans[k*MAX+j];
			}
		}
		/*主进程没有处理完的由主进程来处理*/
		for(i=(size-1)*line;i<MAX;i++)
       	{
           	for(j=0;j<MAX;j++)
           	{
				temp = 0;
               	for(k=0;k<MAX;k++)
                   	temp+=b[j+k*MAX]*a[k+i*MAX];
               	c[i*MAX+j]=temp;
           	}
      	}
		/*结束记时并打印结果*/
		endTime = MPI_Wtime();
		endTime1 = omp_get_wtime();
/*		for(i=0;i<MAX*MAX;i++)
		{
			printf("%4.0f ",c[i]);
			if((i+1)%MAX == 0)
				printf("\n");
		}*/
		fprintf(fp,"test martix size %d processor %d------------------------------->\n",MAX,size);
		fprintf(fp,"the total compute time is %f \n",endTime-startTime);
		fprintf(fp,"the total time is %f         \n",endTime1-startTime1);
		if(ans != NULL)
		{
			free(ans);
			//ans = NULL;
		}
		if(a != NULL)
			free(a);
		if(b != NULL)
			free(b);
		if(c !=NULL)
			free(c);
	}else{
		
		int i,j,k,temp,l;
		float *buff,*ans;
		b = (float *)malloc(sizeof(float)*MAX*MAX);
		buff = (float *)malloc(sizeof(float)*line*MAX);
		ans = (float *)malloc(sizeof(float)*line*MAX);
		memset(buff,0,sizeof(float)*line*MAX);
		memset(ans,0,sizeof(float)*line*MAX);
		/*接收主进程的广播并接收主进程发送过来的列*/
		MPI_Bcast(b,MAX*MAX,MPI_FLOAT,0,MPI_COMM_WORLD);	
		
		//MPI_Recv(buff,line*MAX,MPI_FLOAT,0,COLUMN_DATA_SEND_AND_RECIEVE,MPI_COMM_WORLD,&status);
		MPI_Scatter(NULL,line*MAX,MPI_FLOAT,buff,line*MAX,MPI_FLOAT,0,MPI_COMM_WORLD);
		/*数组相乘*/
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
		/*把结果发送给主进程*/
		MPI_Send(ans,line*MAX,MPI_FLOAT,0,RESULT_SEND_AND_RECIEVE,MPI_COMM_WORLD);
		if(ans != NULL)
		{
			free(ans);
			ans = NULL;
		}
		if(buff != NULL){
			free(buff);
			buff = NULL;
		}		
		if(b !=NULL)
			free(b);
	}
	MPI_Finalize();
	return 0;
}
