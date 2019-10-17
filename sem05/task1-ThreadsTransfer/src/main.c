#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>

#include "tools.h"

#define MAX_SIZE	100

typedef struct
{
	int fd;
	char name[6];
} Fifo;


//	Open fifo on writing.
int openFifo_write(Fifo* fifo)
{
	if (MY_assert(fifo))
		return ASSERT_FAIL;

	if((fifo->fd = open(fifo->name, O_WRONLY)) < 0)
	{
		message(red, "Can't open FIFO for writing");
		return FUN_ERROR;
	}

	return OK;
}


//	Open fifo on reading.
int openFifo_read(Fifo* fifo)
{
	if (MY_assert(fifo))
		return ASSERT_FAIL;

	if((fifo->fd = open(fifo->name, O_RDONLY)) < 0)
	{
		message(red, "Can't open FIFO for reading");
		return FUN_ERROR;
	}

	return OK;
}


void* threadFifoWrite(void* arg)
{
	Fifo* fifo = (Fifo*)arg;
	char str[MAX_SIZE] = {};

	for (;;)
	{
		fgets(str, MAX_SIZE, stdin);
		fifo->fd = open(fifo->name, O_WRONLY);
		write(fifo->fd, str, MAX_SIZE);
		close(fifo->fd);
	}

	return NULL;
}


void* threadFifoRead(void* arg)
{
	Fifo* fifo = (Fifo*)arg;
	char str[MAX_SIZE] = {};

	for (;;)
	{
		fifo->fd = open(fifo->name, O_RDONLY);
		read(fifo->fd, str, MAX_SIZE);
		close(fifo->fd);
		printf("%s", str);
	}

	return NULL;
}



int main(int argc, char **argv, char** envp)
{
	int arg = -1, error = 0, result = -1;

	//	Check if there is one parameter in argv
	if (argc != 2)
	{
		red; printf("# Pass one argument: 0 or 1\n"); reset_color;
		return -1;
	}
	else
		arg = atoi(argv[1]);

	//	Arg can be 0 or 1 only
	if ((arg != 0) && (arg != 1))
	{
		red; printf("# Pass one argument: 0 or 1\n"); reset_color;
		return -1;
	}

	(void)umask(0);

	Fifo fifo1 = {-1, "fifo1"};
	Fifo fifo2 = {-1, "fifo2"};
	Fifo fifos[2] = {fifo1, fifo2};

	//	Create fifos
	error |= mknod(fifo1.name, S_IFIFO | 0666, 0);
	error |= mknod(fifo2.name, S_IFIFO | 0666, 0);

	if (error == EEXIST)
	{
		red; printf("# Can't create or open fifo\n"); reset_color;
		return OPEN_FAIL;
	}

	pthread_t thid1, thid2;
	if (arg == 0)
	{
		result |= pthread_create(&thid1, (pthread_attr_t*)NULL, threadFifoWrite,	(void*)(&fifos[arg]));
		result |= pthread_create(&thid2, (pthread_attr_t*)NULL, threadFifoRead, 	(void*)(&fifos[1-arg]));
	}
	else
	{
		result |= pthread_create(&thid2, (pthread_attr_t*)NULL, threadFifoRead, 	(void*)(&fifos[1-arg]));
		result |= pthread_create(&thid1, (pthread_attr_t*)NULL, threadFifoWrite,	(void*)(&fifos[arg]));
	}

	pthread_join(thid1,	(void**)NULL);
	pthread_join(thid2,	(void**)NULL);

	close(fifo1.fd);
	close(fifo2.fd);

	return 0;
}
