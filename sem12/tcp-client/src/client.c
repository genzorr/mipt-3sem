/* Простой пример TCP-клиента для сервиса echo */
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


void* clientRead(void* arg)
{
	int sockfd = *((int*)arg);
	char sendline[1000] = {};
	int n = 0;

	for(;;)
	{
		fflush(stdin);
		fgets(sendline, 1000, stdin);
		if((n = write(sockfd, sendline, strlen(sendline)+1)) < 0)
		{
			perror("Can't write\n");
			close(sockfd);
			exit(1);
		}
	}

	return NULL;
}

void* clientWrite(void* arg)
{
	int sockfd = *((int*)arg);
	int n = 0;
	char recvline[1000] = {};

	for (;;)
	{
		if ((n = read(sockfd,recvline, 999)) < 0)
		{
			perror("Can\'t read\n");
			close(sockfd);
			exit(1);
		}
		printf("%s", recvline);
	}

	return NULL;
}



int main(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_in servaddr;

    if(argc != 2){
        printf("Usage: a.out <IP address>\n");
        exit(1);
    }

    //	Create tcp socket.
    if((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror(NULL);
        exit(1);
    }

    //	Setup socket.
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(51000);
    if(inet_aton(argv[1], &servaddr.sin_addr) == 0)
    {
        printf("Invalid IP address\n");
        close(sockfd);
        exit(1);
    }

    //	Connect to socket.
    if(connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
    {
        perror(NULL);
        close(sockfd);
        exit(1);
    }

	pthread_t thid1, thid2;
	pthread_create(&thid1, (pthread_attr_t*)NULL, clientRead,	(void*)&sockfd);
	pthread_create(&thid2, (pthread_attr_t*)NULL, clientWrite, 	(void*)&sockfd);

	pthread_join(thid1,	(void**)NULL);
	pthread_join(thid2,	(void**)NULL);

    close(sockfd);

    return 0;
}
