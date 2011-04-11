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

#include "types.h"

struct usk_buff {
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

	struct uprotocol *proto;
};


#define uskb_direct(uskb) ((uskb)->direct)

#define uskb_set_direct(uskb, direct) ((uskb)->direct = direct)


/* Inline functions to set properties of usk_buff */
static inline void *
uskb_network_header(const struct usk_buff *uskb)
{
	return uskb->network_header;
}

static inline void
uskb_set_network_header(struct usk_buff *uskb)
{
	uskb->network_header = uskb->header + uskb->header_len;
}

static inline void *
uskb_transport_header(const struct usk_buff *uskb)
{
	return uskb->transport_header;
}

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

static inline void
uskb_set_header(struct usk_buff *uskb, const void *header_buff, const int max_len)
{
	uskb->header = header_buff;
	uskb->header_max = len
}

static inline void
uskb_set_payload(struct usk_buff *uskb, const void *payload)
{
	uskb->payload = payload;
}



extern struct usk_buff *
uskb_alloc_in();

extern struct usk_buff *
uskb_alloc_out(int header_size)

extern void
uskb_free(struct usk_buff *uskb);


#endif /* __USKBUFF_H */
