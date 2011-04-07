/*
 * =====================================================================================
 *
 *       Filename:  ip_header.h
 *
 *    Description:  all IP header related structure & macro.
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

#ifndef __IP_HEADER_H
#define __IP_HEADER_H

/* use bsd'ish ip header */
/* these headers are for a Linux system, but */
/* the names on other systems are easy to guess.. */
#ifdef __USE_BSD
#undef __USE_BSD	
#endif
#include <netinet/ip.h>

#include "types.h"
#include "uskbuff.h"


static inline struct iphdr *
uskb_iphdr(struct usk_buff *uskb)
{
	return (struct iphdr *)uskb_network_header(uskb);
}



/* Debug method, from dump.c */
extern void
iphdr_dump(struct iphdr *iph);


#endif /* __IP_HEADER_H */
