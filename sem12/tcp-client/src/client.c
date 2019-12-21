#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>

#include "tools.h"
#include "connection.h"

pthread_t thid1, thid2;

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
	message_t message = {
			.broadcast = BROADCAST_ALL
	};

	for (;;)
	{
		message.broadcast = BROADCAST_ALL;

		char dummy[MESSAGE_LEN] = {};
		fflush(stdin);
		fgets(dummy, MESSAGE_LEN, stdin);
		char* n = strchr(dummy, '\n');
		*n = '\0';

		if (dummy[0] == '#')
		{
			memcpy(message.receiver, dummy+1, MESSAGE_LEN);
			fgets(message.payload.message, MESSAGE_LEN, stdin);
			char* n = strchr(message.payload.message, '\n');
			*n = '\0';
			message.broadcast = BROADCAST_ONE;
		}
		else
			memcpy(message.payload.message, dummy, MESSAGE_LEN);

		memcpy(message.payload.name, client.name, NAME_LEN);
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
			close(sockfd);
			break;
		}

		if (message.response == NO_USER)
		{
			yellow; printf("No user with name %s.", message.receiver); reset_color;
			printf("\n");
			continue;
		}

		if (message.broadcast == BROADCAST_ONE)
		{
			yellow; printf("(you)%s: ", message.payload.name); reset_color;
			printf("%s\n", message.payload.message);
		}
		else if (message.broadcast == BROADCAST_ALL)
		{
			blue; printf("(all)%s: ", message.payload.name); reset_color;
			printf("%s\n", message.payload.message);
			memset(message.payload.message, 0x00, MESSAGE_LEN);
		}
	}

	pthread_kill(thid2, SIGINT);
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

    system("clear");

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
    {
    	close(client.params.sockfd);
    	return -1;
    }

	pthread_create(&thid1, (pthread_attr_t*)NULL, Read,	(void*)&client);
	pthread_create(&thid2, (pthread_attr_t*)NULL, Write, (void*)&client);

	pthread_join(thid1,	(void**)NULL);
	pthread_join(thid2,	(void**)NULL);

    close(sockfd);

    return 0;
}
