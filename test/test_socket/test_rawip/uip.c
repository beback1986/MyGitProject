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

#include "uprotocol.h"
#include "uip.h"

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

int
uip_input(struct usk_buff *uskb)
{
}
