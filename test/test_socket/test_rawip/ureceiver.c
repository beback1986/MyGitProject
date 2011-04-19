/*
 * =====================================================================================
 *
 *       Filename:  ureceiver.c
 *
 *    Description:  receiver loop.
 *
 *        Version:  1.0
 *        Created:  04/07/2011 03:42:18 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Beback
 *        Company:  Xidian
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

#include "types.h"
#include "mempool.h"
#include "uerror.h"
#include "task.h"

#include "uskbuff.h"
#include "uprotocol.h"
#include "uip.h"
#include "utransport.h"

static struct mempool *rcv_mempool;

/* Initial all header pointer in uskb. */
int
__do_rcv(struct usk_buff *uskb)
{
	struct uprotocol *proto;
	int ret;
	int len;

	/* At this moment, one thing is for sure: This package MUST be 
	 * an datagram of IP. So we can point out header for both network
	 * layer and transport layer. */
	uskb_set_network_header(uskb);
	len = uip_header_len(uskb);
	if (!len || uskb_header_grow(uskb, len))
		goto discard;

	uskb_set_transport_header(uskb);

	proto = uip_find_proto(uskb);
	if (!proto)
		goto unkown_proto;

	len = proto->header_len(uskb);
	if (!len || uskb_header_grow(uskb, len))
		goto discard;

	uskb->proto = proto;

	ret = uip_input(uskb);

	return 0;

unkown_proto:
discard:
	return -1;
}

void *
ureceiver(void *t)
{
	struct task *rcv_task;
	struct usk_buff *uskb;
	void *buffer;
	int sock_opt;
	int fd;
	int *ret;
	int count = 0;

	rcv_task = task_get(t);
	ret = malloc(sizeof(int));

	fd = socket (PF_INET, SOCK_RAW, IPPROTO_TCP);
	if (fd < 0) {
		PERROR("Can not create socket!");
		*ret = -1;
		goto out;
	}

	sock_opt = 1;
	if(setsockopt (fd, IPPROTO_IP, IP_HDRINCL, &sock_opt, sizeof (sock_opt)) < 0){
		PERROR("Can not set HDRINCL!");
		*ret = -1;
		goto out;
	}

	while (1) {
		count++;
		printf("rcv %d package\n", count);
		buffer = mempool_alloc(rcv_mempool);
		if (recv(fd, buffer, RCV_BUFFER_SIZE, 0) < 0)
			PERROR("Receive error!");
		uskb = uskb_alloc_in();
		uskb_set_header(uskb, buffer, RCV_BUFFER_SIZE);
		__do_rcv(uskb);
	}
	close(fd);

	*ret = 0;
out:
	task_ret_set(rcv_task, ret);
	return NULL;
}

struct task *
ureceiver_start(void)
{
	struct task *rcv_task;

	rcv_task = task_create(ureceiver);

	/* It could return a NULL when failed.
	 * So the caller should deal with it. */
	task_start(rcv_task);

	return rcv_task;
}

void
ureceiver_init(void)
{
	rcv_mempool = mempool_create("rcv_mempool", RCV_BUFFER_SIZE, RCV_BUFFER_COUNT);
}
