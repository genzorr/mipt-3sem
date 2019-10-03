#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "tools.h"

#define MAX_SIZE	100
#define END			10

const char* filepath = "../input.txt";

/*
 * Parents: from 1 to 2
 * Childs:	from 2 to 1
 */

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
		red; printf("Can't open FIFO for writing\n"); reset_color;
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
		red; printf("Can't open FIFO for reading\n"); reset_color;
		return FUN_ERROR;
	}

	return OK;
}

int parentWrite(Fifo* fifo, char* str, pid_t child)
{
	if (MY_assert(fifo) || MY_assert(str))
		return ASSERT_FAIL;

	fgets(str, MAX_SIZE, stdin);
	write(fifo->fd, str, MAX_SIZE);

	//	Transfer '`' symbol to close session
	if (str[0] == '`')
	{
		kill(child, SIGKILL);
		close(fifo->fd);

		return END;
	}

	return OK;
}

int childRead(Fifo* fifo, char* str)
{
	if (MY_assert(fifo) || MY_assert(str))
		return ASSERT_FAIL;

	read(fifo->fd, str, MAX_SIZE);

	//	Transfer '`' symbol to close session
	if (str[0] == '`')
	{
		memset(str, 0, MAX_SIZE);
		exit(0);
	}

	printf("%s", str);
	fflush(stdout);
	memset(str, 0, MAX_SIZE);

	return OK;
}


int main(int argc, char **argv, char** envp)
{
	int arg = 0, error = 0;

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

	//	Create fifoss
	error |= mknod(fifo1.name, S_IFIFO | 0666, 0);
	error |= mknod(fifo2.name, S_IFIFO | 0666, 0);

	if (error == EEXIST)
	{
		red; printf("# Can't create or open fifo\n"); reset_color;
		return OPEN_FAIL;
	}

	Fifo fifos[2] = {fifo1, fifo2};

	pid_t pid = fork();

	if (pid < 0)
	{
		red; printf("# Can't fork process\n"); reset_color;
		return -1;
	}


	//	Open fifos on writing/reading
	if (pid > 0)
	{
		//	Parents
		if (openFifo_write(&(fifos[arg])))
			return FUN_ERROR;
	}
	else
	{
		//	Childs
		if (openFifo_read(&(fifos[1-arg])))
			return FUN_ERROR;
	}

	error = 0;
	char* str = (char*)calloc(MAX_SIZE+1, sizeof(*str));

	//	In parent processes: read from stdin, write to fifo
	//	in childs: read from fifo, write to stdout
	for (;;)
	{
		if (pid > 0)
		{
			error = parentWrite(&(fifos[arg]), str, pid);
			if (error == END)
				break;
		}
		else
		{
			error = childRead(&(fifos[1-arg]), str);
			if (error)
				return error;
		}
	}

	free(str);

	return 0;
}
