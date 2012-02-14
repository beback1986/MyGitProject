#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>

#include "dev_event.h"

#define BUFFER_SIZE 4096

#if 0
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

	struct sockaddr_nl *saddr = (struct sockaddr_nl *)calloc(1, sizeof(struct sockaddr_nl));
	if (!saddr){
		printf("alloc mem fail\n");
		goto failed;
	}	

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

#endif

int32_t bwlc_handle_dev_msg(bwlc_devent_msg_t *devent)
{
	printf("recieve msg:type=%d,devpath=%s\n", devent->event_type, devent->dev_path);
	return 0;
}

int main(int argc, char *argv[])
{
	int32_t ret;

	ret = bwlc_uevent_init(BWLC_SCAN_NORMAL);
	if (ret) {
		printf("init bwlc_event failed.\n");
		goto failed;
	}

	while (1)
		sleep(1);

failed:
	return 0;
}
