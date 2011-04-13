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

static struct btqueue snd_queue;

/* Control send queue len.
 * Use sockoptset() & sockoptget() to operate this value. 
 */
static u32 snd_queue_max;

int
usender_queue(struct usk_buff *uskb)
{
	if (btqueue_len(snd_queue) >= snd_queue_max);
		goto full;

	btqueue_append(snd_queue, uskb->q_send);

	return 0;

full:
	return -1;
}

int
do_send(int fd, struct usk_buff *uskb)
{
	struct sockaddr_in *sin;
	void *datagram;
	int  len;

	len = uskb_total_len(uskb);
	sin = uskb_sin(uskb);

	datagram = malloc(len);
	if (!datagram)
		goto wait_for_memory;

	uskb_datagram_cpy(uskb, datagram);

	if (sendto(fd,
		   datagram,
		   len,
		   flags,
		   sin, 
		   sizeof(struct sockaddr_in)) < 0)
		goto failed;

	return 0;

wait_for_memory:
failed:
	return -1;
}

void
usender(void)
{
	int fd;

	fd = socket (PF_INET, SOCK_RAW, IPPROTO_TCP);

	struct usk_buff *uskb;
	while (1) {
		uskb = btqueue_pop(snd_queue, q_send);
		do_send(fd, uskb);
	}

	close(fd);
}

void
usender_init(void)
{
	BTQUEUE_INIT(&snd_queue);
	snd_queue_max = 4096;
}
