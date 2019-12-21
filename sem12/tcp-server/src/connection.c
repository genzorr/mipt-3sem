#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "tools.h"
#include "connection.h"


void clearScreen()
{
}


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


int server_socketSetup(int* sockfd, struct sockaddr_in* servaddr)
{
	if (MY_assert(sockfd) || MY_assert(servaddr))
		return ASSERT_FAIL;

    bzero(servaddr, sizeof(*servaddr));
    servaddr->sin_family= AF_INET;
    servaddr->sin_port= htons(51000);
    servaddr->sin_addr.s_addr = htonl(INADDR_ANY);

    //	Setup socket's address.
    if(bind(*sockfd, (struct sockaddr *)servaddr, sizeof(*servaddr)) < 0)
    {
        red; perror("# Bind"); reset_color;
        close(*sockfd);
        return FUN_ERROR;
    }

    //	Move socket to listening mode.
    if(listen(*sockfd, 5) < 0)
    {
        red; perror("# Listen"); reset_color;
        close(*sockfd);
        return FUN_ERROR;
    }

//    setsockopt(*sockfd, )

    return OK;
}

int serverWrite(socket_params_t client_params, message_t message)
{
	int n = sendto(client_params.sockfd, (char*)&message, sizeof(message), 0, (struct sockaddr*)&(client_params.cliaddr), sizeof(client_params.cliaddr));
	if (n < 0)
	{
		red; perror("# Server write"); reset_color;
		return FUN_ERROR;
	}

	return OK;
}

int serverRead(socket_params_t client_params, message_t* message)
{
	if (MY_assert(message))
		return ASSERT_FAIL;

	int n = recvfrom(client_params.sockfd, (char*)message, sizeof(*message), 0, (struct sockaddr*)NULL, NULL);
	if (n < 0)
	{
		red; perror("# Server read error"); reset_color;
		close(client_params.sockfd);
		return FUN_ERROR;
	}
	else if (n == 0)
	{
		close(client_params.sockfd);
		return FUN_ERROR;
	}

	return OK;
}

int server_checkClientName(client_t* clients, client_t* client)
{
	if (MY_assert(clients) || MY_assert(client))
		return ASSERT_FAIL;

	int error = 0;
	message_t message = {};
	for (;;)
	{
		error = serverRead(client->params, &message);
		if (error != OK)
			return FUN_ERROR;

		int res = 1;
		for (int i = 0; i < MAX_CLIENT_COUNT; i++)
		{
			if (clients[i].status == OK)
			{
				res = strcmp(message.payload.name, clients[i].name);
				if (res == 0)
					break;
			}
		}

		if (res == 0)
			message.response = NON_UNIQUE;
		else
		{
			message.response = OK;
			memcpy(client->name, message.payload.name, NAME_LEN);
			client->status = OK;
			green; printf("# Client %s added.", client->name); reset_color; printf("\n");
			break;
		}

		error = serverWrite(client->params, message);
		if (error != OK)
			return FUN_ERROR;
	}

	error = serverWrite(client->params, message);
	if (error != OK)
		return FUN_ERROR;

	return OK;
}


int server_searchFree(client_t* clients, int* num)
{
	if (MY_assert(clients) || MY_assert(num))
		return ASSERT_FAIL;

	*num = -1;
	for (int i = 0; i < MAX_CLIENT_COUNT; i++)
	{
		if (clients[i].status != OK)
		{
			*num = i;
			break;
		}
	}

	return OK;
}


int server_searchReceiver(client_t* clients, char* name, int* num)
{
	if (MY_assert(clients) || MY_assert(num))
		return ASSERT_FAIL;

	*num = -1;
	for (int i = 0; i < MAX_CLIENT_COUNT; i++)
	{
		if (clients[i].status == OK)
		{
			if (strcmp(name, clients[i].name) == 0)
			{
				*num = i;
				break;
			}
		}
	}

	return OK;
}
