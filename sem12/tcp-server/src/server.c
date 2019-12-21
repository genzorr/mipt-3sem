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

	int error = 0;
    int free_socket = -1;
    server_searchFree(clients, &free_socket);

    if (free_socket == -1)
    {
    	message_t message = {
    			.response = CLIENT_LIMIT
    	};
    	serverWrite(client.params, message);
    	close(client.params.sockfd);
    	return NULL;
    }
    else
    {
    	message_t message = {
			.response = OK
		};
		serverWrite(client.params, message);
    }

    error = server_checkClientName(clients, &client);
    if (error != OK)
    	return NULL;

    client.number = free_socket;
    memcpy(&(clients[free_socket]), &client, sizeof(client));

	for (;;)
	{
		message_t message = {};
		error = serverRead(client.params, &message);
		if (error != OK)
		{
			close(clients[client.number].params.sockfd);
			clients[client.number].status = -1;
			yellow; printf("# Client %s disconnected.", client.name); reset_color; printf("\n");
			return NULL;
		}

		if (message.broadcast == BROADCAST_ALL)
		{
			for (int i = 0; i < MAX_CLIENT_COUNT; i++)
			{
				if ((clients[i].status == OK) && (i != client.number))
					error = serverWrite(clients[i].params, message);

				if (error != OK)
				{
					close(clients[client.number].params.sockfd);
					clients[client.number].status = -1;
					yellow; printf("# Client %s disconnected.", client.name); reset_color; printf("\n");
					return NULL;
				}
			}
		}
		else if (message.broadcast == BROADCAST_ONE)
		{
			int num = -1;
			error = server_searchReceiver(clients, message.receiver, &num);
			if (error != OK)
			{
				close(client.params.sockfd);
				return NULL;
			}

			if (num != -1)
			{
				error = serverWrite(clients[num].params, message);
				if (error != OK)
				{
					close(clients[client.number].params.sockfd);
					clients[client.number].status = -1;
					yellow; printf("# Client %s disconnected.", client.name); reset_color; printf("\n");
					return NULL;
				}
			}
			else
			{
				message.response = NO_USER;
				error = serverWrite(client.params, message);
				if (error != OK)
				{
					close(clients[client.number].params.sockfd);
					clients[client.number].status = -1;
					yellow; printf("# Client %s disconnected.", client.name); reset_color; printf("\n");
					return NULL;
				}

			}
		}
	}

	return NULL;
}


int main()
{
	int error = 0;

    int sockfd, newsockfd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t clilen = sizeof(cliaddr);

    system("clear");

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
