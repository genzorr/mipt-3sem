#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <pthread.h>

#include "tools.h"


void* ThreadGetAverage(void* arg)
{
	int count = *(int*)arg;

	double* avg = (double*)calloc(1, sizeof(*avg));
	for (int i = 0; i < count; i++)
		*avg += rand();

	return (void*)avg;
}


int main(int argc, char **argv)
{
	srand(time(0));

	int n = 0;
	//	Check if there is one parameter in argv.
	if (argc != 2)
	{
		message(red, "# Enter number of threads");
		return -1;
	}
	else
	{
		n = atoi(argv[1]);
		if (n < 1)
		{
			message(red, "# Pass valid number of threads");
			return -1;
		}
	}

	//	Number of elements.
	size_t size = 10E+6;

	//	Allocate array for thread ids and datas.
	pthread_t* thid = (pthread_t*)calloc(n, sizeof(*thid));
	int* data 		= (int*)calloc(n, sizeof(*data));

	struct timeval start, end;
	gettimeofday(&start, NULL);

	int dummy_cnt = 0;
	for (int i = 0; i < n; i++)
	{
		data[i] = (i != (n-1)) ? (size / n) : (size - dummy_cnt);
		dummy_cnt += data[i];

		pthread_create(&thid[i], (pthread_attr_t*)NULL, ThreadGetAverage, (void*)(&data[i]));
	}


	//	Get average values from threads return.
	double avg = 0;
	for (int i = 0; i < n; i++)
	{
		double* dummy;
		pthread_join(thid[i], (void**)&dummy);
		avg += *dummy;
		free(dummy);
	}

	gettimeofday(&end, NULL);

	green; printf("# Average: %.2lf\n", avg / size); reset_color;
	printf("# Time: %fs\n", (end.tv_sec - start.tv_sec)+((float)(end.tv_usec - start.tv_usec))/1000000);

	free(data);
	free(thid);

	return 0;
}
