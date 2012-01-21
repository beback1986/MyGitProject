#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>

#define BUFFER_SIZE 4096

int common_netlink_init(int *sock, struct sockaddr_nl **nl_saddr, int nl)
{
	int ret = -1;
	int rcvbufsz = BUFFER_SIZE;

	*sock = socket(PF_NETLINK, SOCK_DGRAM, nl);
	if (*sock == -1) {
		printf("create socket fail\n");
		goto failed;
	}

	ret = setsockopt(*sock, SOL_SOCKET, SO_RCVBUF, &rcvbufsz,
			    sizeof(rcvbufsz));
	if (ret < 0) {
		printf("set socket opt fail\n");
		close(*sock);
		goto failed;
	}

	struct sockaddr_nl *saddr = (struct sockaddr_nl *)malloc(sizeof(struct sockaddr_nl));
	if (!saddr){
		printf("alloc mem fail\n");
		goto failed;
	}	
	memset(saddr, 0, sizeof(struct sockaddr_nl));

	saddr->nl_family = AF_NETLINK;
	saddr->nl_pid = getpid();
	if (nl == NETLINK_ROUTE)
		saddr->nl_groups =  RTNLGRP_LINK;
	else
		saddr->nl_groups = 0xffffffff;

	ret = bind(*sock, (struct sockaddr *)saddr, sizeof(struct sockaddr_nl));
	if (ret) {
		printf("bind socket fail\n");
		free(saddr);
		close(*sock);
	} else {
		if (saddr != NULL)
			*nl_saddr = saddr;
	}
	
failed:
	return ret;
}

char buff[BUFFER_SIZE];

int main(int argc, char *argv[])
{
	int ret;
	int udev_fd = 0;
	struct sockaddr_nl *udev_addr;

        ret = common_netlink_init(&udev_fd, &udev_addr, NETLINK_KOBJECT_UEVENT);
        if (ret) {
		printf("main: netlink_init for udev failed.%d(%s)", errno, strerror(errno));
		goto failed;
	}

	while (1) {
		memset(buff, '\0', BUFFER_SIZE);
		ret = recv(udev_fd, buff, BUFFER_SIZE, 0);
		printf("recieve:%s\n", buff);
	}

	return 0;
failed:
	return -1;
}
