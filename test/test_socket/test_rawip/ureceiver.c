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

#include "uskbuff.h"
#include "uprotocol.h"
#include "uip.h"
#include "utcp.h"
#include "uerror.h"


/* Initial all header pointer in uskb. */
int
do_rcv(struct usk_buff *uskb)
{
	struct uprotocol *proto;
	struct iphdr *iph;

	/* At this moment, one thing is for sure: This package MUST be 
	 * an datagram of IP. So we can point out header for both network
	 * layer and transport layer. */
	uskb_set_network_header(uskb, 0);
	iph = uskb_iphdr(uskb);
	uskb_header_grow(iph->ihl * 4);
	uskb_set_transport_header(uskb);

	proto = protocol_find(iph->protocol);
	if (!proto)
		return -1;

	uskb->proto = proto;

	if (!uskb_header_grow(proto->header_len(uskb)))
		return -1;

	return 0;
}

int
ureceiver()
{
	struct usk_buff *uskb;
	struct iphdr *iph;
	struct tcphdr *tcph;

	int fd = socket (PF_INET, SOCK_RAW, IPPROTO_TCP);
	if (fd < 0) {
		PERROR("Can not create socket!");
		return -1;
	}
	unsigned char buffer[8192]; /* single packets are usually less than 8192 bytes */
	while (recv(fd, buffer, 8192, 0) > 0) {
		uskb = uskb_alloc_in();
		uskb_set_header(uskb, buffer, 8192);
		do_rcv(uskb);
	}
	close(fd);

	return 0;
}
