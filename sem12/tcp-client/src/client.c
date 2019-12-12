#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#include "tools.h"
#include "connection.h"


void* clientRead(void* arg)
{
	socket_params_t params = *((socket_params_t*)arg);
	int sockfd = params.sockfd;
	struct sockaddr_in servaddr = params.servaddr;
	struct sockaddr_in cliaddr	= params.cliaddr;

	char sendline[1000] = {};
	int n = 0;

	for(;;)
	{
		fflush(stdin);
		fgets(sendline, 1000, stdin);
		if ((n = sendto(sockfd, sendline, strlen(sendline) + 1, 0, (struct sockaddr *) &servaddr, sizeof servaddr) < 0))
		{
			perror("Can't write\n");
			close(sockfd);
			exit(1);
		}
	}

	return NULL;
}

void* clientWrite(void* arg)
{
	socket_params_t params = *((socket_params_t*)arg);
	int sockfd = params.sockfd;
	struct sockaddr_in servaddr = params.servaddr;
	struct sockaddr_in cliaddr	= params.cliaddr;

	int n = 0;
	char recvline[1000] = {};

	for (;;)
	{
		if ((n = read(sockfd,recvline, 999)) < 0)
		{
			perror("Can\'t read\n");
			close(sockfd);
			exit(1);
		}
		printf("%s", recvline);
	}

	return NULL;
}



int main(int argc, char **argv)
{
	int error = 0;

	//	Parameters to operate with socket.
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;

    //	Check for valid usage.
    if(argc != 2){
        printf("Usage: a.out <IP address>\n");
        exit(1);
    }

    //	Create tcp socket.
    error = socketCreate(&sockfd);
    if (error == FUN_ERROR)
    	return -1;

    //	Setup socket.
    error = socketSetup_client(&sockfd, &servaddr, &cliaddr, argv[1]);
    if (error == FUN_ERROR)
		return -1;

    socket_params_t params = {
    		.sockfd = sockfd,
    		.servaddr = servaddr,
			.cliaddr = cliaddr
    };

	pthread_t thid1, thid2;
	pthread_create(&thid1, (pthread_attr_t*)NULL, clientRead,	(void*)&params);
	pthread_create(&thid2, (pthread_attr_t*)NULL, clientWrite, 	(void*)&params);

	pthread_join(thid1,	(void**)NULL);
	pthread_join(thid2,	(void**)NULL);

    close(sockfd);

    return 0;
}
