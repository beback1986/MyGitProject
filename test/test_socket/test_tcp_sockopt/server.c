#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <errno.h>

#define SERVER_PORT 30001 // define the defualt connect port id
#define LENGTH_OF_LISTEN_QUEUE 10 //length of listen queue in server
#define BUFFER_SIZE 2974400
#define ERR_STR_BUF 100
 
#define PERROR(message)								\
		do {								\
			printf(message);					\
			printf(" errno:%d(%s)\n", errno, strerror(errno));	\
		} while(0)

#define RETURN(message) \
		do { PERROR(message); return -1;} while (0)

int main(int argc, char **argv)

{
	int servfd,clifd;
	int ret = 1;
	char *buf;
	char *errno_s = NULL;
	struct sockaddr_in servaddr,cliaddr;
	unsigned int times = 0;
	unsigned long long total_size = 0;

	buf = calloc(1, BUFFER_SIZE);
	if (!buf)
		RETURN("can not alloc memory!");

	if ((servfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) < 0)
		RETURN("create socket error!");

	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERVER_PORT);
//	servaddr.sin_addr.s_addr = htonl(0xe0000000);
//	servaddr.sin_addr.s_addr = inet_addr("224.0.0.0");
	servaddr.sin_addr.s_addr = INADDR_ANY;

	if (bind(servfd,(struct sockaddr*)&servaddr,sizeof(servaddr))<0)
		RETURN("bind error!");

	if (listen(servfd,LENGTH_OF_LISTEN_QUEUE) < 0)
		RETURN("call listen failure!");

	socklen_t length = sizeof(cliaddr);
	clifd = accept(servfd,(struct sockaddr*)&cliaddr,&length);
	sleep(10);
	while(1)
	{
		memset(buf, '\0', BUFFER_SIZE);
		ret = recv (clifd, buf, BUFFER_SIZE, 0);
		if(ret <= 0)
			break;
		times += 1;
		total_size += ret;
	}
	printf("total recv %llu bytes, in %u times\n", total_size, times);
	return 0;

err:
	errno_s = malloc(ERR_STR_BUF);
	ret = sprintf(errno_s, "%d", errno);
	perror(errno_s);
	return -1;
}
