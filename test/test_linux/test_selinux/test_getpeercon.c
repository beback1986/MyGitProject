#include <selinux/selinux.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>

void main(void)
{
	char *buf;
	socklen_t size;
	security_context_t con_get;
	int ret = 0;
	int fd;

	size = 255 + 1;
	buf = malloc(size);
	fd = socket(AF_INET,SOCK_STREAM,0);
	ret = getsockopt(fd, SOL_SOCKET, SO_PEERSEC, buf, &size);
//	if(!ret){
//		printf("sock num:%d\tcon:%s\n",fd,con_get);
//	}
	printf("%s\n",buf);
	close(fd);
}
