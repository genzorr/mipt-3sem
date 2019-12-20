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


void* Write(void* arg)
{
	client_t client = *((client_t*)arg);
	socket_params_t params = {
			.sockfd = client.params.sockfd,
			.servaddr = client.params.servaddr,
			.cliaddr = client.params.cliaddr
	};
	int sockfd = params.sockfd;

	int error = 0;
	message_t message = {};
	memcpy(message.payload.name, client.name, NAME_LEN);

	for(;;)
	{
		fflush(stdin);
		fgets(message.payload.message, MESSAGE_LEN, stdin);
//		if ((n = sendto(sockfd, sendline, strlen(sendline) + 1, 0, (struct sockaddr *) &servaddr, sizeof servaddr) < 0))
//		{
//			red; perror("Can't write\n"); reset_color;
//			close(sockfd);
//			exit(1);
//		}
		error = clientWrite(params, message);
		if (error != OK)
		{
			red; perror("Write error"); reset_color;
			close(sockfd);
			return NULL;
		}

		memset(message.payload.message, 0x00, MESSAGE_LEN);
	}

	return NULL;
}

void* Read(void* arg)
{
	client_t client = *((client_t*)arg);
	socket_params_t params = {
			.sockfd = client.params.sockfd,
			.servaddr = client.params.servaddr,
			.cliaddr = client.params.cliaddr
	};
	int sockfd = params.sockfd;

	int error = 0;
	message_t message = {};

	for (;;)
	{
		error = clientRead(params, &message);
		if (error != OK)
		{
			red; perror("Read error"); reset_color;
			close(sockfd);
			return NULL;
		}

		printf("%s", message.payload.message);
		memset(message.payload.message, 0x00, MESSAGE_LEN);
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
        yellow; printf("Usage: a.out <IP address>\n"); reset_color;
        exit(1);
    }

    //	Create tcp socket.
    error = socketCreate(&sockfd);
    if (error != OK)
    	return -1;

    //	Setup socket.
    error = client_socketSetup(&sockfd, &servaddr, &cliaddr, argv[1]);
    if (error != OK)
		return -1;

    client_t client = {
    		.name = {},
			.params.sockfd = sockfd,
			.params.servaddr = servaddr,
			.params.cliaddr = cliaddr
    };

    // Setup client in the net.
    error = client_pickName(&client);
    if (error == FUN_ERROR)
    	return -1;

    pthread_t thid1, thid2;
	pthread_create(&thid1, (pthread_attr_t*)NULL, Read,	(void*)&client);
	pthread_create(&thid2, (pthread_attr_t*)NULL, Write, (void*)&client);

	pthread_join(thid1,	(void**)NULL);
	pthread_join(thid2,	(void**)NULL);

    close(sockfd);

    return 0;
}
