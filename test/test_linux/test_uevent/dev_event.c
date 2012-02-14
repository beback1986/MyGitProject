#include <sys/types.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "dev_event.h"

#define BWLC_UEVENT_BUFSIZE 4096

static bwlc_scan_set_t g_scan_set;

static const char *bwlc_scan_keyword[] = {
	"sd", /* For BWLC_SCAN_NORMAL. */
	"md", /* For BWLC_SCAN_MULTIPATH. */
};

int bwlc_netlink_init(struct sockaddr_nl **nl_saddr, int nl)
{
	int sock = -1, ret;
	int rcvbufsz = BWLC_UEVENT_BUFSIZE;
	struct sockaddr_nl *saddr = NULL;

	sock = socket(PF_NETLINK, SOCK_DGRAM, nl);
	if (sock == -1) {
		printf("BWLC_NETLINK:Create socket fail. %d(%s)\n",
					errno, strerror(errno));
		goto failed;
	}

	ret = setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &rcvbufsz,
			    sizeof(rcvbufsz));
	if (ret < 0) {
		printf("BWLC_NETLINK:Set socket opt fail. %d(%s)\n",
					errno, strerror(errno));
		goto failed;
	}

	saddr = calloc(1, sizeof(struct sockaddr_nl));
	if (!saddr){
		printf("BWLC_NETLINK:Alloc mem for sockaddr fail. %d(%s)\n",
					errno, strerror(errno));
		goto failed;
	}	

	saddr->nl_family = AF_NETLINK;
	saddr->nl_pid = getpid();
	if (nl == NETLINK_ROUTE)
		saddr->nl_groups =  RTNLGRP_LINK;
	else
		saddr->nl_groups = 0x1;
//		saddr->nl_groups = 0xffffffff;

	ret = bind(sock, (struct sockaddr *)saddr, sizeof(struct sockaddr_nl));
	if (ret) {
		printf("BWLC_NETLINK:Bind socket fail. %d(%s)\n", 
					errno, strerror(errno));
		goto failed;
	}
	*nl_saddr = saddr;

	return sock;
failed:
	if (saddr)
		free(saddr);
	if (sock)
		close(sock);
	return -1;
}

int bwlc_netlink_recv(int sock, char *buf, size_t len)
{
	ssize_t ret;

	ret = recv(sock, buf, len, 0);
	if (ret == 0) {
		printf("BWLC_NETLINK:Peer shutdown while recv.\n");
		goto failed;
	} else if (ret < 0) {
		printf("BWLC_NETLINK:Recv error. %d(%s)\n", errno, strerror(errno));
		goto failed;
	}

	return 0;
failed:
	return -1;
}

/*
 * Parse out uevent about blockt device.
 * 1: found
 * 0: not found
 */
int __bwlc_uevent_parse(const char *uevent, char **dev_name, 
				bwlc_devent_type_t *event_type)
{
	char *block, *first_slash, *last_slash;

	/* Parse event type first. */
	if (strlen(uevent) < 4)
		goto failed;

	if (strncmp(uevent, "add@", 4) == 0) {
		(*event_type) = BWLC_DEV_ARRIVE;
	} else if (strncmp(uevent, "remo", 4) == 0) {
		if (strlen(uevent) < 7)
			goto failed;
		if (strncmp(uevent, "remove@", 7) != 0)
			goto failed;
		(*event_type) = BWLC_DEV_REMOVE;
	} else {
		goto failed;
	}

	/* Parse device name. */
	block = strstr(uevent, "block");
	if (!block)
		goto failed;

	first_slash = index(block, '/');
	if (!first_slash)
		goto failed;

	last_slash = rindex(block, '/');
	if (!last_slash)
		goto failed;

	if (first_slash != last_slash)
		goto failed;

	(*dev_name) = strdup(last_slash+1);
	if (!(*dev_name))
		goto failed;

	return 1;
failed:
	(*dev_name) = NULL;
	(*event_type) = 0;
	return 0;
}

int32_t bwlc_uevent_handler(bwlc_devent_type_t event_type, char *dev_name)
{
	int32_t ret = 0;
	bwlc_devent_msg_t *devent = NULL;

	if (strncmp(dev_name, bwlc_scan_keyword[g_scan_set], 
			strlen(bwlc_scan_keyword[g_scan_set])) != 0) {
		printf("BWLC_UEVENT: %s is not in scan set.\n", dev_name);
		goto failed;
	}

	devent = calloc(1, sizeof(bwlc_devent_msg_t));
	if (!devent) {
		printf("BWLC_UEVENT:Can not alloc mem for event msg.\n");
		goto failed;
	}

	devent->event_type = event_type;
	devent->dev_path   = calloc(1, strlen(dev_name)+10);
	if (!devent->dev_path) {
		printf("BWLC_UEVENT:Can not alloc mem for dev_path.\n");
		goto failed;
	}
	sprintf(devent->dev_path, "/dev/%s", dev_name);

	bwlc_handle_dev_msg(devent);

	return ret;
failed:
	if (devent)
		free(devent);
	return -1;
}

void *bwlc_uevent_reciever(void *unused)
{
	int usock = -1, ret;
	struct sockaddr_nl *nl_addr;
	bwlc_devent_type_t event_type;
	char *dev_name;
	char *uevent_buf = NULL;

	uevent_buf = calloc(1, BWLC_UEVENT_BUFSIZE);

	while (1) {
		/* Initial usock or reinitial usock. */
		if (usock == -1) {
			printf("BWLC_UEVENT:Init netlink.\n");
			usock = bwlc_netlink_init(&nl_addr, NETLINK_KOBJECT_UEVENT);
		}

		/* Create sock failed. */
		if (usock == -1) {
			printf("BWLC_UEVENT:Init netlink failed, sleep & retry.\n");
			sleep(1);
			continue;
		}

		/* Block recieve msg. */
		memset(uevent_buf, 0, BWLC_UEVENT_BUFSIZE);
		ret = bwlc_netlink_recv(usock, uevent_buf, BWLC_UEVENT_BUFSIZE);
		if (ret < 0) {
			printf("BWLC_UEVENT:Recv error reinitial usock.\n");
			usock = -1;
			continue;
		}

		/* Parse out block device msg, and msg type. */
		if (!__bwlc_uevent_parse(uevent_buf, &dev_name, &event_type))
			continue;

		printf("BWLC_UEVENT:DEBUG:uevent msg:%s\n", uevent_buf);
		/* Call uevent handler to handle this msg. */
		bwlc_uevent_handler(event_type, dev_name);
	}
}

/*
 * For thread library. It should be replaced by bw_thread.h.
 */
#include <pthread.h>

int32_t bwlc_uevent_init(bwlc_scan_set_t scan_set)
{
	pthread_t pd;
	int32_t   bwret = 0;
	int       ret;

	if (scan_set < 0 || scan_set >= BWLC_SCAN_MAX) {
		printf("BWLC_UEVENT:Unkown scan set %d\n", scan_set);
		goto failed;
	}
	g_scan_set = scan_set;

	ret = pthread_create(&pd, NULL, bwlc_uevent_reciever, NULL);
	if (ret) {
		printf("BWLC_UEVENT:Create thread error.%d(%s)\n",
				ret, strerror(ret));
		bwret = -1;
		goto failed;
	}

failed:
	return bwret;
}
