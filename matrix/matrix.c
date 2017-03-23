#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#define M 4000

int main(int argc,char **argv)
{
	double startime,endtime,duration;

	float *a,*b,*c;
	int i,j,k;
	a = (float *)malloc(sizeof(float)*M*M);
	b = (float *)malloc(sizeof(float)*M*M);
	c = (float *)malloc(sizeof(float)*M);

	for(i=0;i<M;i++){
		for(j=0;j<M;j++)
		{
			a[i*M+j] = 1.0;
			b[i*M+j] = 1.0;
		}
	}
	startime = omp_get_wtime();
	for(i=0;i<M;i++)
	{
		for(j=0;j<M;j++)
		{
			for(k=0;k<M;k++)
					c[i] += a[i*M+k] * b[k+j*M];
		}
	}
	endtime = omp_get_wtime();
	duration = endtime - startime;
	printf("duration = %fs\n",duration);
	free(a);
	free(b);
	free(c);
}
