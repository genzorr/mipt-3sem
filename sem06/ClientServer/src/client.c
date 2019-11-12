#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "tools.h"
#include "connection.h"

#define CLIENT_COUNT 10

char pathname[] = "/home/michael/code/mipt-3sem/sem06/msqtest";


int main()
{
	//	Create message query.
	int msqid = msqCreate(pathname);
	printf("msq: %d\n", msqid);

	for (int i = 0; i < CLIENT_COUNT; i++)
	{
		pid_t pid = fork();
		if (pid == 0)
			continue;

		server_client_buf scbuf = {
			.mtype = TO_CLIENT,
			.payload = {}
		};
		client_server_buf csbuf = {
			.mtype = TO_SERVER,
			.payload = {}
		};

		int sclen = sizeof(scbuf.payload);
		int cslen = sizeof(csbuf.payload);

		pid = getpid();
		srand(pid);
		for (;;)
		{
			//	Client to server.
			csbuf.mtype = TO_SERVER;
			csbuf.payload.a = rand()%20;
			csbuf.payload.b = rand()%20;
			csbuf.payload.pid = pid;
			msgsnd(msqid, (struct msgbuf*)&csbuf, cslen, 0);

			//	Server to client.
			msgrcv(msqid, (struct msgbuf *)&scbuf, sclen, pid, 0);

			printf("sent: pid %d, payload %d %d, received: %ld\n", csbuf.payload.pid,
											csbuf.payload.a, csbuf.payload.b, scbuf.payload.ab );
			sleep(2);
		}
	}

	return 0;
}
