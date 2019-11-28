#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#include "tools.h"
#include "connection.h"


int socketCreate(void)
{
	int sockfd = 0;
	if((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0){
		perror(NULL);
		exit(1);
	}

	return sockfd;
}

