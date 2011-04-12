/*
 * =====================================================================================
 *
 *       Filename:  usender.h
 *
 *    Description:  sender loop.
 *
 *        Version:  1.0
 *        Created:  04/08/2011 10:27:18 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Beback
 *        Company:  Xidian
 *
 * =====================================================================================
 */

#include <stdlib.h>

#include "types.h"
#include "btqueue.h"
#include "uskbuff.h"
#include "uip.h"

#include <string.h>
/* Build a sockaddr_in object. NOTE: This macro does not take 
 * the responsibility to translate to net bit order */
#define BUILD_SIN(sin, port, addr)			\
	do {						\
		memset(sin, 0, sizeof(*sin));		\
		sin->sin_family = AF_INET;		\
		sin->port = port;			\
		sin->sin_addr.s_addr = addr;		\
	} while(0)

static struct btqueue sender_queue;

int
do_send(int fd, struct usk_buff *uskb)
{
	struct sockaddr_in *sin;
	struct iphdr *iph;

	sin = calloc(1, sizeof(struct sockaddr_in));
	if (!sin)
		goto wait_for_memory;

	iph = uskb_iphdr(uskb);
	BUILD_SIN(sin, );

	sendto(fd, datagrame, len, flags, sin, sizeof(struct sockaddr));

	return 0;

wait_for_memory:
	return -1;
}

void
usender(void)
{
	int fd;

	fd = socket (PF_INET, SOCK_RAW, IPPROTO_TCP);

	struct usk_buff *uskb;
	while (1) {
		uskb = dequeue(sender_queue, q_send);
		do_send(fd, uskb);
	}

	close(fd);
}

void
usender_init(void)
{
	BTQUEUE_INIT(&sender_queue);
}
