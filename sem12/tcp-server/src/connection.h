#ifndef CONNECTION_H_
#define CONNECTION_H_

#define NAME_LEN		20

#define BROADCAST_ALL	0
#define BROADCAST_ONE	1

#define MESSAGE_LEN		100


typedef struct socket_params
{
	int sockfd;
	struct sockaddr_in servaddr;
	struct sockaddr_in cliaddr;
} socket_params_t;


typedef struct message
{
	int broadcast;
	struct sockaddr_in receiver;
	typedef struct payload_t
	{
		char name[NAME_LEN];
		char message[MESSAGE_LEN];
	} payload;
} message_t;


int socketCreate(int* sockfd);
int socketSetup_server(int* sockfd, struct sockaddr_in* servaddr);


#endif /* CONNECTION_H_ */
