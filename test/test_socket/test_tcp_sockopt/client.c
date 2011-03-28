#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <errno.h>

#define SERVER_PORT 30001 // define the defualt connect port id
#define BUFFER_SIZE 40960
#define SOCKOPT_BUFFER_SIZE 128

#define PERROR(message)								\
		do {								\
			printf(message);					\
			printf(" errno:%d(%s)\n", errno, strerror(errno));	\
		} while(0)

#define RETURN(message) \
		do { PERROR(message); return -1;} while (0)

int saved_buflen = 0;

int getsockopt_sndbuf(int fd, int package_id)
{
	char *buf;
	int *sockopt_buflen;
	socklen_t buf_len;

	buf = calloc (1, SOCKOPT_BUFFER_SIZE);
	buf_len = SOCKOPT_BUFFER_SIZE;

	if (!buf)
		RETURN("can't alloc sockopt buffer!");

	if (getsockopt(fd, SOL_SOCKET, SO_SNDBUF, buf, &buf_len) < 0)
		RETURN("can't get sockopt!");

	sockopt_buflen = (int *)buf;
	printf("SNDBUF:%d\n", (*sockopt_buflen));
/*	if (saved_buflen != (*sockopt_buflen)) {
		saved_buflen = (*sockopt_buflen);
		printf("SNDBUF changed to:%d while send:%d package\n", saved_buflen, package_id);
	}*/

	free(buf);
	return 0;
}

int main(int argc, char **argv)
{    
	int servfd,ret = 1;
	struct sockaddr_in servaddr;
	socklen_t socklen = sizeof(servaddr);
	char *buf;
	char hello[10] = "hello";
	int i = -1;
      
	if (argc < 2)
	{
		printf ("please input ip \n");
		exit(1);
	}

	buf = calloc (1, BUFFER_SIZE);
	if (!buf)
		RETURN("can't alloc send buffer!");

	if ((servfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
		RETURN("socket create error!");

	getsockopt_sndbuf(servfd, i);

	bzero(&servaddr,sizeof(servaddr));
	inet_aton(argv[1],&servaddr.sin_addr);
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERVER_PORT);

	if (connect(servfd,(struct sockaddr*)&servaddr, socklen) < 0)
		RETURN("can't connect to server!");
	getsockopt_sndbuf(servfd, i);

	for (i = 0; i < 10; i++) {
		if (send(servfd, buf, BUFFER_SIZE, MSG_DONTWAIT) < 0)
			RETURN("can't send msg!");
		getsockopt_sndbuf(servfd, i);
	}
	if (send(servfd, hello, strlen(hello), 0) < 0)
		RETURN("can't send msg!");
	getsockopt_sndbuf(servfd, i);

	close(servfd);
	return 0;
}
