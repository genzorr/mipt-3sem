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

client_t* clients;

void* server(void* arg)
{
	client_t client = {
			.params.sockfd = ((client_t*)arg)->params.sockfd,
			.params.servaddr = ((client_t*)arg)->params.servaddr,
			.params.cliaddr = ((client_t*)arg)->params.cliaddr,
			.name = {},
			.status = -1,
			.number = -1
	};
//	client_t client = *((client_t*)arg);

	int error = 0;

    error = server_checkClientName(clients, &client);
    if (error != OK)
    {
    	return NULL;
    }

    for (int i = 0; i < MAX_CLIENT_COUNT; i++)
    {
    	if (clients[i].status != OK)
    	{
    		client.number = i;
    		memcpy(&(clients[i]), &client, sizeof(client));
    		break;
    	}
    }

	for (;;)
	{
		//	Read information from connected socket.
//		while((n = recvfrom(newsockfd, line, 1000, 0, (struct sockaddr*)NULL, NULL)) > 0)
//		{
//			for (int i = 0; i < client_count; i++)
//			{
//				if (clients[i].params.sockfd == newsockfd)
//					continue;
//
//				if ((n = sendto(clients[i].params.sockfd, line, 1000, 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr))) < 0)
//				{
//					red; perror("# Sendto"); reset_color;
//					close(newsockfd);
//					exit(1);
//				}
//			}
//		}

		message_t message = {};
		error = serverRead(client.params, &message);
		if (error != OK)
		{
//			close(clients[client.number].params.sockfd);
			clients[client.number].status = -1;
			return NULL;
		}

		error = serverWrite(client.params, message);
		if (error != OK)
		{
			close(clients[client.number].params.sockfd);
			clients[client.number].status = -1;
			return NULL;
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
    if (error != OK)
    	return -1;

    //	Setup socket.
    error = server_socketSetup(&sockfd, &servaddr);
    if (error != OK)
		return -1;

    client_t client_dummy = {};
    clients = (client_t*)calloc(MAX_CLIENT_COUNT, sizeof(*clients));
    for (int i = 0; i < MAX_CLIENT_COUNT; i++)
    {
    	memcpy(&clients[i], &client_dummy, sizeof(client_dummy));
    	clients[i].status = -1;
    }

    while(1)
    {
    	while(1)
    	{
			for (int i = 0; i < MAX_CLIENT_COUNT; i++)
			{
				if (clients[i].status != OK)
					break;
			}
    	}

    	//	Client address.
        clilen = sizeof(cliaddr);

        //	Accept socket connection.
        if((newsockfd = accept(sockfd, (struct sockaddr*)&cliaddr, &clilen)) < 0)
        {
            red; perror("# Accept"); reset_color;
            close(sockfd);
            return -1;
        }

        client_t new_client = {
        		.params.sockfd = newsockfd
        };
        memcpy(&(new_client.params.servaddr), &servaddr, sizeof(servaddr));
        memcpy(&(new_client.params.cliaddr), &cliaddr, sizeof(cliaddr));

        pthread_t thid;
        pthread_create(&thid, (pthread_attr_t*)NULL, server, (void*)&new_client);
    }

    free(clients);

    return 0;
}
