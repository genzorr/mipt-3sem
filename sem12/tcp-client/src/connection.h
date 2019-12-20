#ifndef CONNECTION_H_
#define CONNECTION_H_

#include <netinet/in.h>

#define NAME_LEN		21

#define BROADCAST_ALL	-1
#define BROADCAST_ONE	1
#define BROADCAST_NONE	0

#define MESSAGE_LEN		101

typedef struct socket_params
{
	int sockfd;
	struct sockaddr_in servaddr;
	struct sockaddr_in cliaddr;
} socket_params_t;


typedef struct message
{
	int broadcast;
	char receiver[NAME_LEN];
	struct payload_t
	{
		char name[NAME_LEN];
		char message[MESSAGE_LEN];
	} payload;
	int response;
} message_t;


typedef struct client
{
	socket_params_t params;
	char name[NAME_LEN];
} client_t;


int socketCreate(int* sockfd);
int client_socketSetup(int* sockfd, struct sockaddr_in* servaddr, struct sockaddr_in* cliaddr, const char* ip);

int clientWrite(socket_params_t server_params, message_t message);
int clientRead(socket_params_t server_params, message_t* message);

int client_pickName(client_t* client);

#endif /* CONNECTION_H_ */
