#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

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
	int len;
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

	//	Receive.
//	if(( len = msgrcv(msqid, (struct msgbuf *) &buf1, sizeof(buf1.a), 0, 0) < 0))
//	{
//		printf("Can\'t receive message from queue\n");
//		exit(-1);
//	}
	while(msgrcv(msqid, (struct msgbuf *) &buf1, sizeof(buf1.a), 0, 0) < 0);
	printf("message type = %ld, info = %d\n", buf1.mtype, buf1.a);


	//	Send.
	buf2.mtype = 2;
//	buf2.a = 11;
	buf2.a = 1;
	for (int i = buf1.a; i > 1; i--)
		buf2.a *= i;
	len = sizeof(buf2.a);

	if (msgsnd(msqid, (struct msgbuf *)&buf2, len, 0) < 0)
	{
		printf("Can\'t send message to queue\n");
		msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
		exit(-1);
	}


	//	Receive.
	if(( len = msgrcv(msqid, (struct msgbuf *) &buf1, sizeof(buf1.a), 0, 0) < 0))
	{
		printf("Can\'t receive message from queue\n");
		exit(-1);
	}
	if (buf1.mtype == LAST_MESSAGE)
	{
		msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
		exit(0);
	}

	return 0; /* Исключительно для отсутствия warning'ов при компиляции. */
}
