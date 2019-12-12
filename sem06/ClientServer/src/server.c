#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "tools.h"
#include "connection.h"

#define PROCESS_COUNT 2

char pathname[] = "/home/michael/code/mipt-3sem/sem06/msqtest";


int main()
{
	//	Create message query.
	int msqid = msqCreate(pathname);
	printf("msq: %d\n", msqid);

	for (int i = 0; i < PROCESS_COUNT; i++)
	{
		pid_t pid = fork();
		if (pid == 0)
			continue;

		server_client_buf scbuf = {
			.mtype = 0,
			.payload = {}
		};
		client_server_buf csbuf = {
			.mtype = TO_SERVER,
			.payload = {}
		};

		int sclen = sizeof(scbuf.payload);
		int cslen = sizeof(csbuf.payload);

		for (;;)
		{
			//	Client to server.
			if (msgrcv(msqid, (struct msgbuf*)&csbuf, cslen, TO_SERVER, 0) < 0)
				perror("msgrcv");

			//	Server to client.
			scbuf.mtype = csbuf.payload.pid;
			scbuf.payload.ab = csbuf.payload.a * csbuf.payload.b;
			msgsnd(msqid, (struct msgbuf*)&scbuf, sclen, 0);

			printf("received: pid %d, payload %d %d, sent: %ld\n", csbuf.payload.pid,
								csbuf.payload.a, csbuf.payload.b, scbuf.payload.ab);
			sleep(10);
		}
	}

	return 0;
}
