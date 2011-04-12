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
ip_output(struct usk_buff *uskb)
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
ip_input(struct usk_buff *uskb)
{
}
