/*
 * =====================================================================================
 *
 *       Filename:  uskbuff.h
 *
 *    Description:  userspace sk_buff structure & related macros.
 *                  It designed to be works like struct sk_buff in the kernel.
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

#ifndef __USKBUFF_H
#define __USKBUFF_H 

#include <string.h>
#include <pthread.h>
#include <netinet/in.h> /* for struct sockaddr_in */

#include "types.h"
#include "btqueue.h"

struct usk_buff_head {
	struct usk_buff  *prev;
	struct usk_buff  *next;
	u16		 qlen;
	pthread_mutex_t loc;
};

struct usk_buff {
	/* Used to join in write_queue and receive queue.
	 * This 2 field should always at top. */
	struct usk_buff *prev;
	struct usk_buff *next;

	u8	 direct:1, /* 0:out,1:in */
		 unused:7;
	void 	*header;
	void 	*payload;
	u16   	 header_len,	/* Length of current header */
		 payload_len,	/* Length of pure payload, 
				 * do not change during transmission. */
		 header_max;	/* max header length */
	void 	*network_header;
	void 	*transport_header;

	/* For queue this uskb to send queue, used ONLY in usender. */
	struct btqueue_node q_send;

	/* ONLY for usender use. Transport protocol provide port num.
	 * Network protocol provide addr. */
	struct sockaddr_in  sin;

	struct uprotocol    *proto;
};

/* sockaddr related operations. */
#define uskb_sin_set_port(uskb, port)	\
	do {(uskb)->sin.port = (port);} while(0)

#define uskb_sin_set_addr(uskb, addr)	\
	do {(uskb)->sin.sin_addr.s_addr = (addr);} while(0)

#define uskb_sin(uskb)	\
	(&(uskb)->sin)

/* uskb queue related operations. */
static inline void
uskb_queue_init(struct usk_buff_head *list)
{
	list->prev = list->next = (struct usk_buff *)list;
	pthread_mutex_init(&list->loc, NULL);
	list->qlen = 0;
}

static inline void
__uskb_insert(struct usk_buff_head *list,
	      struct usk_buff      *new,
	      struct usk_buff      *prev,
	      struct usk_buff      *next)
{
	new->prev = prev;
	new->next = next;
	prev->next = next->prev = new;
	list->qlen ++;
}

static inline struct usk_buff *
__uskb_delete(struct usk_buff_head *list,
	      struct usk_buff      *old)
{
	if (old == (struct usk_buff *)list)
		return NULL;
	old->prev->next = old->next;
	old->next->prev = old->prev;
	old->next = old->prev = NULL;
	list->qlen --;
	return old;
}


static inline void
__uskb_enqueue_tail(struct usk_buff_head *list, struct usk_buff *new)
{
	__uskb_insert(list, new, (struct usk_buff *)list, list->prev);
}

static inline struct usk_buff *
__uskb_dequeue_head(struct usk_buff_head *list)
{
	return __uskb_delete(list, list->next);
}

static inline void
uskb_enqueue_tail(struct usk_buff_head *list, struct usk_buff *new)
{
	pthread_mutex_lock(&list->loc);
	__uskb_enqueue_tail(list, new);
	pthread_mutex_unlock(&list->loc);
}

static inline struct usk_buff *
uskb_dequeue_head(struct usk_buff_head *list)
{
	struct usk_buff *head;

	pthread_mutex_lock(&list->loc);
	head = __uskb_dequeue_head(list);
	pthread_mutex_unlock(&list->loc);

	return head;
}


#define uskb_direct(uskb) ((uskb)->direct)

#define uskb_set_direct(uskb, __D) ((uskb)->direct = (__D))


/* Inline functions to set properties of usk_buff */
#define uskb_network_header(uskb)	\
	((uskb)->network_header)

static inline void
uskb_set_network_header(struct usk_buff *uskb)
{
	uskb->network_header = uskb->header + uskb->header_len;
}

#define uskb_transport_header(uskb)	\
	((uskb)->transport_header)

static inline void
uskb_set_transport_header(struct usk_buff *uskb)
{
	uskb->transport_header = uskb->network_header + uskb->header_len;
}

/* in: grow and set header
 * out: set header and group
 */
static inline int
uskb_header_grow(struct usk_buff *uskb, const int len)
{
	if ((uskb->header_len + len) > uskb->header_max)
		return -1;
	uskb->header_len += len;
	return 0;
}

#define uskb_total_len(uskb)	\
	((uskb)->header_len + (uskb)->payload_len)

static inline void
uskb_set_header(struct usk_buff *uskb, void *header_buff, const int max_len)
{
	uskb->header = header_buff;
	uskb->header_max = max_len;
}

static inline void
uskb_set_payload(struct usk_buff *uskb, void *payload)
{
	uskb->payload = payload;
}

/* GNU style void * operation. 
 * (void *)++ == (char *)++ */
#define __uskb_header(uskb)								\
	((uskb)->direct ?								\
	        ((uskb)->header)+(uskb)->header_max-(uskb)->header_len	\
	       :(uskb)->header)

static inline int
uskb_header_cpy(struct usk_buff *uskb, void *buff)
{
	memcpy(buff, __uskb_header(uskb), uskb->header_len);
	return uskb->header_len;
}

static inline int
uskb_payload_cpy(struct usk_buff *uskb, void *buff)
{
	memcpy(buff, uskb->payload, uskb->payload_len);
	return uskb->payload_len;
}

static inline int
uskb_datagram_cpy(struct usk_buff *uskb, void *buff)
{
	int len;
	len = uskb_header_cpy(uskb, buff);
	/* GNU style void * operation. */
	len += uskb_payload_cpy(uskb, buff+len);
}



extern struct usk_buff *
uskb_alloc_in();

extern struct usk_buff *
uskb_alloc_out(int header_size);

extern void
uskb_free(struct usk_buff *uskb);


#endif /* __USKBUFF_H */
