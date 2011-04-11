/*
 * =====================================================================================
 *
 *       Filename:  queue.h
 *
 *    Description:  queue implementation.
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

#ifndef __QUEUE_H
#define __QUEUE_H

#include <pthread.h>

#include "atomic.h"

#define queue_entry(ptr, type, member) \
	container_of(ptr, type, member)

struct queue_node {
	struct queue_node *next;
};

struct queue {
	struct queue_node *head;
	struct queue_node *tail;

	/* This is lock for queue node operation.
	 * It should not include the atomic counter field.
	 */
	pthread_mutex_t    loc;

	atomic_t	   len;
};

#define QUEUE_INIT(que)							\
	do {								\
		atomic_init(&que->len);					\
		pthread_mutex_init(&que->loc);				\
	} while(0)

/* TODO: Implemet lock free enqueue! */
#define enqueue(que, node)						\
	do {								\
		node->next = NULL;					\
		pthread_mutex_lock(&que->loc);				\
		que->tail->next = node;					\
		que->tail = node;					\
		pthread_mutex_unlock(&que->loc);			\
									\
		atomic_inc(&que->len);					\
	} while(0)

#define dequeue(ptr, que, member)					\
	do {								\
		atomic_dec(que->len);					\
									\
		ptr = queue_entry(que->head, typeof(*ptr), member);	\
		que->head = que->head->next;				\
	} while(0)

#endif /* __QUEUE_H */
