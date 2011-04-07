/*
 * =====================================================================================
 *
 *       Filename:  utcp.c
 *
 *    Description:  tcp protocol implementer
 *
 *        Version:  1.0
 *        Created:  04/07/2011 03:28:18 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Beback
 *        Company:  Xidian
 *
 * =====================================================================================
 */

#include "uskbuff.h"
#include "uprotocol.h"
#include "utcp.h"

int
utcp_header_len(struct usk_buff *uskb)
{
	struct tcphdr *tcph;

	tcph = uskb_tcphdr(uskb);
	if (!tcph)
		return 0;

	return (tcph->doff * 4);
}

static struct uprotocol utcp {
	.proto_num	= 6,
	.name		= "TCP",
	.header_len	=
};

int
utcp_init(void)
{
	return 0;
}
