#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <errno.h>

#define SERVER_PORT 30001 // define the defualt connect port id
#define BUFFER_SIZE 1024

#define PERROR(message)								\
		do {								\
			printf(message);					\
			printf(" errno:%d(%s)\n", errno, strerror(errno));	\
		} while(0)

#define RETURN(message) \
		do { PERROR(message); return -1;} while (0)

int main(int argc, char **argv)
{    
	int servfd,clifd,ret = 1;
	struct sockaddr *  servaddr_p;
	struct sockaddr_in servaddr,cliaddr;
	socklen_t socklen = sizeof(servaddr);
	char *buf;

	char hello[10] = "hello";
      
	if (argc < 2)
	{
		printf ("please input ip \n");
		exit(1);
	}

	bzero(&servaddr,sizeof(servaddr));
	servaddr_p = (struct sockaddr*)&servaddr;

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERVER_PORT);
	inet_aton(argv[1], &servaddr.sin_addr);

	buf = calloc (1, BUFFER_SIZE);

	if ((servfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		RETURN("socket create error!");
	ret = sendto(servfd, hello, strlen(hello), 0, servaddr_p, socklen);

	close(clifd);
	return 0;
}
