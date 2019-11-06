#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define LAST_MESSAGE 255

typedef struct mymsgbuf1
{
	long mtype;
	int a;
} mybuf1;

typedef struct mymsgbuf2
{
	long mtype;
	long long a;
} mybuf2;


int main()
{
	int msqid;

	char pathname[] = "/home/michael/code/mipt-3sem/file";

	key_t key;
	int i,len;
	mybuf1 buf1;
	mybuf2 buf2;

	if((key = ftok(pathname,0)) < 0)
	{
		printf("Can\'t generate key\n");
		exit(-1);
	}

	if((msqid = msgget(key, 0666 | IPC_CREAT)) < 0)
	{
		printf("Can\'t get msqid\n");
		exit(-1);
	}

	//	Send.
	buf1.mtype = 1;
	buf1.a = 10;
	len = sizeof(buf1.a);

	if (msgsnd(msqid, (struct msgbuf *)&buf1, len, 0) < 0)
	{
		printf("Can\'t send message to queue\n");
		msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
		exit(-1);
	}

	//	Receive.
	if(( len = msgrcv(msqid, (struct msgbuf *) &buf2, sizeof(buf2.a), 0, 0) < 0))
	{
		printf("Can\'t receive message from queue\n");
		exit(-1);
	}
	printf("message type = %ld, info = %d\n", buf2.mtype, buf2.a);


	//	Last.
	buf1.mtype = LAST_MESSAGE;
	len = 0;

	if (msgsnd(msqid, (struct msgbuf *) &buf1, len, 0) < 0)
	{
		printf("Can\'t send message to queue\n");
		msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
		exit(-1);
	}

	return 0;
}
