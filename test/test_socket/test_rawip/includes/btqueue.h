/*
 * =====================================================================================
 *
 *       Filename:  btqueue.h
 *
 *    Description:  btqueue implementation.
 *    		    1. This queue may block after it is empty.
 *    		    2. This queue is thread-safe for multi customer and one consumer.
 *    		    So, this queue is called btqueue
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

#ifndef __BTQUEUE_H
#define __BTQUEUE_H

#include <pthread.h>

#include "atomic.h"
#include "list.h"

#define btqueue_entry(ptr, type, member) \
	container_of(ptr, type, member)

struct btqueue_node {
	struct btqueue_node *next;
};

struct btqueue {
	struct btqueue_node *head;
	struct btqueue_node *tail;

	/* This is lock for queue node operation.
	 * It should not include the atomic counter field.
	 */
	pthread_mutex_t     loc;

	atomic_t	    len;
};

#define BTQUEUE_INIT(btq)						\
	do {								\
		atomic_init(&(btq)->len);				\
		pthread_mutex_init(&(btq)->loc, NULL);			\
	} while(0)

/* TODO: Implemet lock free enqueue! */
#define btqueue_append(btq, node)					\
	do {								\
		(node)->next = NULL;					\
		pthread_mutex_lock(&(btq)->loc);			\
		(btq)->tail->next = (node);				\
		(btq)->tail = (node);					\
		pthread_mutex_unlock(&(btq)->loc);			\
									\
		atomic_inc(&(btq)->len);				\
	} while(0)

#define btqueue_pop(btq, type, member)	({			\
	atomic_dec(&(btq)->len);				\
								\
	struct btqueue_node *__btn__ = (btq)->head;		\
	(btq)->head = (btq)->head->next;			\
	__btn__->next = NULL;					\
	btqueue_entry(__btn__, type, member);})

#define btqueue_len(btq)	\
	(atomic_val(&(btq)->len))


#endif /* __BTQUEUE_H */
