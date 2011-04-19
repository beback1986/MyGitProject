/*
 * =====================================================================================
 *
 *       Filename:  protocol.h
 *
 *    Description:  protocol related functions.
 *
 *        Version:  1.0
 *        Created:  04/07/2011 01:40:18 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Beback
 *        Company:  Xidian
 *
 * =====================================================================================
 */

#include "uprotocol.h"
#include "list.h"

static struct uprotocol *proto_list[MAX_PROTO_NUM];

int 
uprotocol_register(struct uprotocol *proto)
{
	if (proto_list[proto->proto_num])
		return -1;

	proto_list[proto->proto_num] = proto;
	return 0;
}

struct uprotocol *
uprotocol_find(int proto_num)
{
	if (proto_num < 0 || proto_num > MAX_PROTO_NUM)
		return NULL;

	return proto_list[proto_num];
}
