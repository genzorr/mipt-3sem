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
		perror("# Socket");
		return FUN_ERROR;
	}

	return OK;
}


int socketSetup_client(int* sockfd, struct sockaddr_in* servaddr, struct sockaddr_in* cliaddr, const char* ip)
{
	if (MY_assert(sockfd))
		return ASSERT_FAIL;
	if (MY_assert(servaddr) || MY_assert(cliaddr))
		return ASSERT_FAIL;

	bzero(cliaddr, sizeof(*cliaddr));
	cliaddr->sin_family = AF_INET;
	cliaddr->sin_port = htons(0);
	cliaddr->sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(*sockfd, (struct sockaddr*)cliaddr, sizeof(*cliaddr)) < 0)
	{
		perror("# Bind");
		close(*sockfd);
		return FUN_ERROR;
	}

	bzero(servaddr, sizeof(*servaddr));
	servaddr->sin_family = AF_INET;
	servaddr->sin_port = htons(51000);
	if(inet_aton(ip, &(servaddr->sin_addr)) == 0)
	{
		printf("# Invalid IP address\n");
		close(*sockfd);
		return FUN_ERROR;
	}

	//	Connect to socket.
	if(connect(*sockfd, (struct sockaddr*)servaddr, sizeof(*servaddr)) < 0)
	{
		perror("# Connect");
		close(*sockfd);
		return FUN_ERROR;
	}

	return OK;
}
