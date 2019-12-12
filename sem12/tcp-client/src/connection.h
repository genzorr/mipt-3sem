#ifndef CONNECTION_H_
#define CONNECTION_H_

#include <netinet/in.h>

#define TO_CLIENT		1
#define TO_SERVER		2
#define LAST_MESSAGE	255

typedef struct socket_params
{
	int sockfd;
	struct sockaddr_in servaddr;
	struct sockaddr_in cliaddr;
} socket_params_t;


typedef struct client_server_buf_msg
{
	long mtype;
	struct payload1_t
	{
		pid_t pid;
		int a;
		int b;
	}payload;
} client_server_buf;


typedef struct server_client_buf_msg
{
	long mtype;
	struct payload2_t
	{
		long ab;
	}payload;
} server_client_buf;

int socketCreate(int* sockfd);
int socketSetup_client(int* sockfd, struct sockaddr_in* servaddr, struct sockaddr_in* cliaddr, const char* ip);

#endif /* CONNECTION_H_ */
