#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "tools.h"
#include "connection.h"


int socketCreate(int* sockfd)
{
	if (MY_assert(sockfd))
		return ASSERT_FAIL;

	if((*sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
	{
		red; perror("# Socket"); reset_color;
		return FUN_ERROR;
	}

	return OK;
}


int client_socketSetup(int* sockfd, struct sockaddr_in* servaddr, struct sockaddr_in* cliaddr, const char* ip)
{
	if (MY_assert(sockfd))
		return ASSERT_FAIL;
	if (MY_assert(servaddr) || MY_assert(cliaddr) || MY_assert(ip))
		return ASSERT_FAIL;

	bzero(cliaddr, sizeof(*cliaddr));
	cliaddr->sin_family = AF_INET;
	cliaddr->sin_port = htons(0);
	cliaddr->sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(*sockfd, (struct sockaddr*)cliaddr, sizeof(*cliaddr)) < 0)
	{
		red; perror("# Bind"); reset_color;
		close(*sockfd);
		return FUN_ERROR;
	}

	bzero(servaddr, sizeof(*servaddr));
	servaddr->sin_family = AF_INET;
	servaddr->sin_port = htons(51000);
	if(inet_aton(ip, &(servaddr->sin_addr)) == 0)
	{
		red; printf("# Invalid IP address\n"); reset_color;
		close(*sockfd);
		return FUN_ERROR;
	}

	//	Connect to socket.
	if(connect(*sockfd, (struct sockaddr*)servaddr, sizeof(*servaddr)) < 0)
	{
		red; perror("# Connect"); reset_color;
		close(*sockfd);
		return FUN_ERROR;
	}

	return OK;
}


int clientWrite(socket_params_t server_params, message_t message)
{
	int n = sendto(server_params.sockfd, (char*)&message, sizeof(message), 0, (struct sockaddr *)&(server_params.servaddr), sizeof(server_params.servaddr));
	if (n < 0)
	{
		red; perror("# Client write"); reset_color;
		close(server_params.sockfd);
		return FUN_ERROR;
	}

	return OK;
}

int clientRead(socket_params_t server_params, message_t* message)
{
	if (MY_assert(message))
		return ASSERT_FAIL;

	int n = recvfrom(server_params.sockfd, (char*)message, sizeof(*message), 0, (struct sockaddr*)NULL, NULL);
	if (n < 0)
	{
		red; perror("# Client read error"); reset_color;
		close(server_params.sockfd);
		return FUN_ERROR;
	}

	return OK;
}


int client_pickName(client_t* client)
{
	blue;
	printf("## Welcome to v1 server messenger\n");
	printf("## Pick a name to start (20 symbols max):\n");
	reset_color;

	int error = 0;
	for (;;)
	{
		message_t message = {
				.broadcast = BROADCAST_NONE,
				.response = -1
		};
		scanf("%s", message.payload.name);

		error = clientWrite(client->params, message);
		if (error != OK)
			return FUN_ERROR;

		error = clientRead(client->params, &message);
		if (error != OK)
			return FUN_ERROR;

		if (message.response != OK)
		{
			yellow; printf("This name is already in use, try again.\n"); reset_color;
		}
		else
		{
			memcpy(client->name, message.payload.name, NAME_LEN);
			green; printf("Your name: %s\n", client->name); reset_color;
			break;
		}
	}

	return OK;
}

