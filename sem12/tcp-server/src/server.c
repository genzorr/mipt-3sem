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

#define MAX_CLIENT_COUNT 1000

int client_count;
int* sockfds;


void* server(void* arg)
{
	int newsockfd = *((int*)arg);

	char line[1000] = {};
	int n = 0;

	for (;;)
	{
		//	Read information from connected socket.
		while((n = read(newsockfd, line, 999)) > 0)
		{
			for (int i = 0; i < client_count; i++)
			{
				if (sockfds[i] == newsockfd)
					continue;

				if((n = write(sockfds[i], line, strlen(line)+1)) < 0)
				{
					perror(NULL);
					close(newsockfd);
					exit(1);
				}
			}
		}

		//	Exit if error occurred.
		if(n < 0)
		{
			perror(NULL);
			close(newsockfd);
			exit(1);
		}
	}

	return NULL;
}


int main()
{
    int sockfd, newsockfd;
    int clilen;
    struct sockaddr_in servaddr, cliaddr;

    //	Create tcp socket.
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror(NULL);
        exit(1);
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family= AF_INET;
    servaddr.sin_port= htons(51000);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    //	Setup socket's address.
    if(bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
    {
        perror(NULL);
        close(sockfd);
        exit(1);
    }

    //	Move socket to listening mode.
    if(listen(sockfd, 5) < 0)
    {
        perror(NULL);
        close(sockfd);
        exit(1);
    }

    client_count = 0;
    sockfds = (int*)calloc(MAX_CLIENT_COUNT, sizeof(*sockfds));

    while(1)
    {
    	//	Client address.
        clilen = sizeof(cliaddr);

        //	Accept socket connection.
        if((newsockfd = accept(sockfd, (struct sockaddr *) &cliaddr, &clilen)) < 0)
        {
            perror(NULL);
            close(sockfd);
            exit(1);
        }
        sockfds[client_count++] = newsockfd;

        pthread_t thid;
        pthread_create(&thid, (pthread_attr_t*)NULL, server, (void*)&newsockfd);
    }

    return 0;
}
