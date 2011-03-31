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
	int servfd, clifd, ret = 1, i;
	struct sockaddr_in servaddr,cliaddr;
	socklen_t socklen = sizeof(servaddr);
	char *buf;
	char hello[10] = "hello";
      
	if (argc < 2)
	{
		printf ("please input ip \n");
		exit(1);
	}

	buf = calloc (1, BUFFER_SIZE);

	if ((servfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
		RETURN("socket create error!");

	bzero(&servaddr,sizeof(servaddr));
	inet_aton(argv[1],&servaddr.sin_addr);
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERVER_PORT);
//	servaddr.sin_addr.s_addr = htons(INADDR_ANY);
	printf("%d\n", servaddr.sin_port);

	if (connect(servfd,(struct sockaddr*)&servaddr, socklen) < 0)
		RETURN("can't connect to server!");
	i = 10;
/*	while (--i > 0) {
		ret = send(servfd,buf,BUFFER_SIZE,0);
		printf("%d bytes send\n", ret);
	}*/
	ret = send(servfd, hello, strlen(hello), 0);
	close(clifd);
	return 0;
}
