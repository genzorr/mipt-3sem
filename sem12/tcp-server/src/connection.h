#ifndef CONNECTION_H_
#define CONNECTION_H_

#define NAME_LEN		21

#define BROADCAST_ALL	-1
#define BROADCAST_ONE	1
#define BROADCAST_NONE	0

#define MESSAGE_LEN		101

#define MAX_CLIENT_COUNT	3


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
	int status;
	int number;
} client_t;


int socketCreate(int* sockfd);
int server_socketSetup(int* sockfd, struct sockaddr_in* servaddr);

int serverWrite(socket_params_t client_params, message_t message);
int serverRead(socket_params_t client_params, message_t* message);

int server_checkClientName(client_t* clients, client_t* client);


#endif /* CONNECTION_H_ */
