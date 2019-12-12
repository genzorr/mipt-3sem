#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
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


int socketSetup_server(int* sockfd, struct sockaddr_in* servaddr)
{
    bzero(servaddr, sizeof(*servaddr));
    servaddr->sin_family= AF_INET;
    servaddr->sin_port= htons(51000);
    servaddr->sin_addr.s_addr = htonl(INADDR_ANY);

    //	Setup socket's address.
    if(bind(*sockfd, (struct sockaddr *)servaddr, sizeof(*servaddr)) < 0)
    {
        perror("# Bind");
        close(*sockfd);
        return FUN_ERROR;
    }

    //	Move socket to listening mode.
    if(listen(*sockfd, 5) < 0)
    {
        perror("# Listen");
        close(*sockfd);
        return FUN_ERROR;
    }

    return OK;
}
