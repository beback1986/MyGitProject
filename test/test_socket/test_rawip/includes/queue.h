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

#define queue_entry(ptr, type, member) \
	container_of(ptr, type, member)

struct queue_node {
	struct queue_node *next;
};

struct queue {
	struct 	queue_node *head;
	struct 	queue_node *tail;
	int 		    len;
};

#define QUEUE_INIT(que)							\
	do {								\
									\
	} while(0)

static inline void
enqueue(struct queue *que, struct queue_node *que_node)
{
	que->tail->next = que_node;
	que_node->next = NULL;
}

#define __dequeue(ptr, que, member)					\
	do {								\
		ptr = queue_entry(que->head, typeof(*ptr), member);	\
	} while(0)

#define dequeue_wait(ptr, que, member)					\
	do {								\
									\
	} while(0)
