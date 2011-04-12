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

#include "types.h"
#include "btqueue.h"
#include "uskbuff.h"

static struct btqueue sender_queue;

void
usender(void)
{
	struct usk_buff *uskb;
	while (1) {
		dequeue(uskb, sender_queue, q_send);
	}
}
