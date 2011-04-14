/*
 * =====================================================================================
 *
 *       Filename:  uip.c
 *
 *    Description:  IP related functions.
 *
 *        Version:  1.0
 *        Created:  04/12/2011 10:03:09 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Beback
 *        Company:  Xidian
 *
 * =====================================================================================
 */

#include "types.h"
#include "uprotocol.h"
#include "uip.h"

/* IP header checksums related. */
u16
ip_csum (u16 *buf, int nwords)
{
	unsigned long sum;
	for (sum = 0; nwords > 0; nwords--)
		sum += *buf++;
	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	return ~sum;
}

/* Interfaces to outer world. */
int
uip_header_len(struct usk_buff *uskb)
{
	struct iphdr *iph;

	iph = uskb_iphdr(uskb);
	if (!iph)
		return 0;

	return (iph->hdr * 4);
}

struct uprotocol *
uip_find_proto(struct usk_buff *uskb)
{
	struct iphdr *iph;
	struct uprotocol *proto;

	iph = uskb_iphdr(uskb);
	if (!iph)
		return 0;
	proto = uprotocol_find(iph->protocol);

	return proto;
}

int
uip_output(struct usk_buff *uskb)
{
}

int
uip_input(struct usk_buff *uskb)
{
	struct iphdr     *iph;
	struct uprotocol *proto;
	int ret;

	iph = uskb_iphdr(uskb);
	proto = uprotocol_find(iph->protocol);
	if (!proto) {
		printf("protocol not found for %d\n", iph->protocol);
		return -1;
	}

	ret = proto->handler(uskb);

	return ret;
}
