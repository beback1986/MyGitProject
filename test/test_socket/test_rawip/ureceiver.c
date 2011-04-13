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
#include <sys/socket.h>

#include "types.h"
#include "uerror.h"
#include "uskbuff.h"
#include "uprotocol.h"
#include "uip.h"

#define RCV_BUFFER_SIZE 8192 /* Single packets are usually less than 8192 bytes. */
#define RCV_BUFFER_COUNT 1024

struct mempool *rcv_mempool;


/* Initial all header pointer in uskb. */
int
do_rcv(struct usk_buff *uskb)
{
	struct uprotocol *proto;
	int ret;
	int len;

	/* At this moment, one thing is for sure: This package MUST be 
	 * an datagram of IP. So we can point out header for both network
	 * layer and transport layer. */
	uskb_set_network_header(uskb, 0);
	len = uip_header_len(uskb);
	if (!len || uskb_header_grow(len))
		goto discard;

	uskb_set_transport_header(uskb);

	proto = uip_find_proto(uskb);
	if (!proto)
		goto unkown_proto;

	len = proto->header_len(uskb);
	if (!len || uskb_header_grow(len))
		goto discard;

	uskb->proto = proto;

	ret = uip_input(uskb);

	return 0;

unkown_proto:
discard:
	return -1;
}

int
ureceiver()
{
	struct usk_buff *uskb;
	void 	*buffer;
	int fd;

	fd = socket (PF_INET, SOCK_RAW, IPPROTO_TCP);
	if (fd < 0) {
		PERROR("Can not create socket!");
		return -1;
	}
	while (1) {
		buffer = mempool_alloc(rcv_mempool);
		if (recv(fd, buffer, RCV_BUFFER_SIZE, 0) < 0)
			PERROR("Receive error!");
		uskb = uskb_alloc_in();
		uskb_set_header(uskb, buffer, RCV_BUFFER_SIZE);
		do_rcv(uskb);
	}
	close(fd);

	return 0;
}

void
ureceiver_init(void)
{
	rcv_mempool = mempool_create("rcv_mempool", RCV_BUFFER_SIZE, RCV_BUFFER_COUNT);
}
