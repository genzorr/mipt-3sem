#ifndef CONNECTION_H_
#define CONNECTION_H_

#define TO_CLIENT		1
#define TO_SERVER		2
#define LAST_MESSAGE	255

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


int msqCreate(char* path);

#endif /* CONNECTION_H_ */
