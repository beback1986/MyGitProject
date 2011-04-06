/*
 * =====================================================================================
 *
 *       Filename:  tcp_header.h
 *
 *    Description:  all TCP header related structure & macro.
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

#ifndef __TCP_HEADER_H
#define __TCP_HEADER_H

#ifdef __FAVOR_BSD
#undef __FAVOR_BSD	/* use bsd'ish tcp header */
#endif
#include <netinet/tcp.h>

#include "types.h"
#include "uskbuff.h"

static inline struct tcphdr *
uskb_tcphdr(struct usk_buff *uskb)
{
	return (struct tcphdr *)uskb_transport_header(uskb);
}


/* Debug method, from dump.c */
extern void
tcphdr_dump(struct tcphdr *iph);


#endif /* __TCP_HEADER_H */
