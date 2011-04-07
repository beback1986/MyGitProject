/*
 * =====================================================================================
 *
 *       Filename:  init.c
 *
 *    Description:  init all necessary things in usock.
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

#include "uprotocol.h"
#include "utcp.h"

int
init(void)
{
	protocol_register(utcp);

	if (utcp_init())
		return -1;

	return 0;
}
