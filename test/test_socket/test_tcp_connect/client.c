#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <errno.h>

#define SERVER_PORT 30002 // define the defualt connect port id
#define CLIENT_PORT 30001

#define BUFFER_SIZE 1024

#define TRY_COUNT 100000

#define PERROR(message)								\
		do {								\
			printf(message);					\
			printf(" errno:%d(%s)\n", errno, strerror(errno));	\
		} while(0)

#define RETURN(message) \
		do { PERROR(message); return -1;} while (0)

int main(int argc, char **argv)
{    
	int fd,ret = 1,i;
	struct sockaddr_in servaddr,cliaddr;
	socklen_t socklen = sizeof(servaddr);
	char *buf;
	char hello[10] = "hello";
      
	if (argc < 2) {
		printf ("please input server ip \n");
		exit(1);
	}

	buf = calloc (1, BUFFER_SIZE);

	if ((fd = socket(AF_INET,SOCK_STREAM,0)) < 0)
		RETURN("socket create error!");

	bzero(&servaddr,sizeof(servaddr));
	bzero(&cliaddr,sizeof(cliaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERVER_PORT);
	inet_aton(argv[1],&servaddr.sin_addr);

	cliaddr.sin_family = AF_INET;
	cliaddr.sin_port = htons(CLIENT_PORT);
	cliaddr.sin_addr.s_addr = INADDR_ANY;

	if (bind(fd, (struct sockaddr*)&cliaddr, socklen)<0)
		RETURN("client bind error!");

	i = 0;
	while(1){
		i++;
		if (i == TRY_COUNT) {
			printf("connect %d times, all failed\n", i);
			return -1;
		}
		if (connect(fd, (struct sockaddr*)&servaddr, socklen) >= 0) {
			printf("get connected after tried %d times\n", i);
			break;
		}
	}
/*	if (connect(fd, (struct sockaddr*)&servaddr, socklen) < 0)
		RETURN("can't connect to server!");*/

	ret = send(fd, hello, strlen(hello), 0);
	close(fd);
	return 0;
}
