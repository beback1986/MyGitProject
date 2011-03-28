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
#define BUFFER_SIZE 1024
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
	int servfd;
	int ret = 1;
	char *buf;
	char *errno_s = NULL;
	struct sockaddr_in servaddr,cliaddr;
	socklen_t cli_socklen = sizeof(cliaddr);
	struct sockaddr *  cliaddr_p;

	buf = calloc(1, BUFFER_SIZE);

	if ((servfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		RETURN("create socket error!");

	bzero(&servaddr,sizeof(servaddr));
	bzero(&cliaddr,sizeof(servaddr));
        cliaddr_p = (struct sockaddr *) &cliaddr;

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERVER_PORT);
	servaddr.sin_addr.s_addr = INADDR_ANY;

	if (bind(servfd,(struct sockaddr*)&servaddr,sizeof(servaddr))<0)
		RETURN("bind error!");

	while(1)
	{
		sleep(1);
		memset(buf, '\0', BUFFER_SIZE);
		ret = recvfrom(servfd, buf, BUFFER_SIZE, 0, cliaddr_p, &cli_socklen);
		if(ret < 0)
			RETURN("recv error!");
		else if(ret > 0){
			printf("recv:%s\n", buf);
			break;
		}
	}
	return 0;
}
