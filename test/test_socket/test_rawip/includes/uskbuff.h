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
	u8		 direct:1, /* 0:out,1:in */
			 unused:7;
	unsigned char 	*header;
	unsigned char 	*payload;
	unsigned int   	 header_len,	/* Length of current header */
			 payload_len,	/* Length of pure payload, do not change during transmission. */
			 total_len,	/* header_len + payload_len */
			 header_max;	/* max header length */
	unsigned char 	*network_header;
	unsigned char 	*transport_header;
};


#define uskb_direct(uskb) ((uskb)->direct)

#define uskb_set_direct(uskb, direct) ((uskb)->direct = direct)


/* Inline functions to set properties of usk_buff */
static inline unsigned char *
uskb_network_header(const struct usk_buff *uskb)
{
	return uskb->network_header;
}

static inline void
uskb_set_network_header(struct usk_buff *uskb, const int offset)
{
	uskb->network_header = uskb->header + offset;
}

static inline unsigned char *
uskb_transport_header(const struct usk_buff *uskb)
{
	return uskb->transport_header;
}

static inline void
uskb_set_transport_header(struct usk_buff *uskb, const int offset)
{
	uskb->transport_header = uskb->network_header + offset;
}

static inline void
uskb_header_grow(struct usk_buff *uskb, const int len)
{
	uskb->header_len += len;
}

static inline void
uskb_set_buff(struct usk_buff *uskb, unsigned char *buff)
{
	uskb->buff = buff;
}

static inline void
uskb_set_header(struct usk_buff *uskb, unsigned char *header_buff)
{
	uskb->header = header_buff;
}



extern struct usk_buff *
uskb_alloc();

extern void
uskb_destroy(struct usk_buff *uskb);


#endif /* __USKBUFF_H */
