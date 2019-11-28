#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>

#include "tools.h"
#include "connection.h"


void* serverRead(void* arg)
{
	int sockfd = *((int*)arg);
	struct sockaddr_in cliaddr;

	int clilen = 0, n = 0;
	char line[1000] = {};

	for (;;)
	{
		clilen = sizeof(cliaddr);
		if((n = recvfrom(sockfd, line, 999, 0, (struct sockaddr *) &cliaddr, (socklen_t*)&clilen)) < 0)
		{
			perror(NULL);
			close(sockfd);
			exit(1);
		}

		printf("%s", line);
	}

	return NULL;
}

void* serverWrite(void* arg)
{
	int sockfd = *((int*)arg);

	struct sockaddr_in cliaddr;

	int clilen = 0;
	char line[1000] = {};

	for (;;)
	{
		if(sendto(sockfd, line, strlen(line), 0, (struct sockaddr *) &cliaddr, clilen) < 0)
		{
			perror(NULL);
			close(sockfd);
			exit(1);
		}
	}

	return NULL;
}


int main(int argc, char **argv)
{
	//	Create socket.
	int sockfd = socketCreate();

	//	Server structure.
	struct sockaddr_in servaddr;
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(51000);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	//	Setup socket.
	if((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror(NULL);
		exit(1);
	}

	//	Bind socket address.
	if(bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
	{
		perror(NULL);
		close(sockfd);
		exit(1);
	}

	pthread_t thid1, thid2;
	pthread_create(&thid1, (pthread_attr_t*)NULL, serverRead,	(void*)&sockfd);
	pthread_create(&thid2, (pthread_attr_t*)NULL, serverWrite, 	(void*)&sockfd);

	pthread_join(thid1,	(void**)NULL);
	pthread_join(thid2,	(void**)NULL);

	close(sockfd);

	return 0;
}
