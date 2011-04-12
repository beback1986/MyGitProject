/*
 * =====================================================================================
 *
 *       Filename:  usock.h
 *
 *    Description:  userspace sock structure & related macros.
 *                  It designed to be works like struct sock in the kernel.
 *
 *        Version:  1.0
 *        Created:  03/31/2011 03:23:18 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Beback
 *        Company:  Xidian
 *
 * =====================================================================================
 */

#ifndef __USOCK_H
#define __USOCK_H

#include "types.h"
#include "uskbuff.h"

struct usock {
	int usk_mode;

	struct usk_buff_head *usk_write_queue;
	struct usk_buff_head *usk_receive_queue;
};

static inline void
usk_write_queu_append(struct usock *usk, struct usk_buff *uskb)
{
	uskb_enqueue_tail(usk->usk_write_queue, uskb);
}

static inline void
usk_receive_queue_append(struct usock *usk, struct usk_buff *uskb)
{
	uskb_enqueue_tail(usk->usk_receive_queue, uskb);
}

#endif /* __USOCK_H */
