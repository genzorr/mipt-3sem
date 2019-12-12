/* Пример простого TCP-сервера для сервиса echo */
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

#define MAX_CLIENT_COUNT	10

int client_count;
int* sockfds;


void* server(void* arg)
{
	socket_params_t params = *((socket_params_t*)arg);
	int newsockfd = params.sockfd;
	struct sockaddr_in servaddr = params.servaddr;
	struct sockaddr_in cliaddr	= params.cliaddr;

	char line[1000] = {};
	int n = 0;

	for (;;)
	{
		//	Read information from connected socket.
		while((n = recvfrom(newsockfd, line, 1000, 0, (struct sockaddr*)NULL, NULL)) > 0)
		{
			for (int i = 0; i < client_count; i++)
			{
				if (sockfds[i] == newsockfd)
					continue;

				if ((n = sendto(sockfds[i], line, 1000, 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr))) < 0)
				{
					perror("# Sendto");
					close(newsockfd);
					exit(1);
				}
			}
		}

		//	Exit if error occurred.
		if(n < 0)
		{
			perror("Error when send");
			close(newsockfd);
			exit(1);
		}
	}

	return NULL;
}


int main()
{
	int error = 0;

    int sockfd, newsockfd;
    socklen_t clilen;
    struct sockaddr_in servaddr, cliaddr;

    //	Create tcp socket.
    error = socketCreate(&sockfd);
    if (error == FUN_ERROR)
    	return -1;

    //	Setup socket.
    error = socketSetup_server(&sockfd, &servaddr);
    if (error == FUN_ERROR)
		return -1;

    socket_params_t params = {
    		.sockfd = 0,
    		.servaddr = servaddr,
			.cliaddr = cliaddr
    };

    client_count = 0;
    sockfds = (int*)calloc(MAX_CLIENT_COUNT, sizeof(*sockfds));

    while(1)
    {
    	//	Client address.
        clilen = sizeof(cliaddr);

        //	Accept socket connection.
        if((newsockfd = accept(sockfd, (struct sockaddr*)&cliaddr, &clilen)) < 0)
        {
            perror("# Accept");
            close(sockfd);
            return -1;
        }
        sockfds[client_count++] = newsockfd;
        params.sockfd = newsockfd;

        pthread_t thid;
        pthread_create(&thid, (pthread_attr_t*)NULL, server, (void*)&params);
    }

    return 0;
}
