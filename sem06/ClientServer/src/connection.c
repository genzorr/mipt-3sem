#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

#include "tools.h"
#include "connection.h"


int msqCreate(char* path)
{
	int msqid = 0;
	key_t key;

	if((key = ftok(path,0)) < 0)
	{
		printf("# Can't generate key\n");
		exit(-1);
	}

	if((msqid = msgget(key, 0666 | IPC_CREAT)) < 0)
	{
		red; printf("# Can't get msqid\n"); reset_color;
		exit(-1);
	}

	return msqid;
}

