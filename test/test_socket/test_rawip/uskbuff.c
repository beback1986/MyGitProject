/*
 * =====================================================================================
 *
 *       Filename:  uskbuff.h
 *
 *    Description:  userspace sk_buff structure & related functions.
 *                  It designed to be works like struct sk_buff in the kernel.
 *
 *        Version:  1.0
 *        Created:  04/06/2011 10:26:18 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Beback
 *        Company:  Xidian
 *
 * =====================================================================================
 */

#include <stdlib.h>

#include "uskbuff.h"

struct usk_buff *
__uskb_alloc()
{
	struct usk_buff *uskb;
	uskb = calloc(1, sizeof(struct usk_buff));
	return (struct usk_buff *)uskb;
}

static inline strcut usk_buff *
uskb_alloc_in()
{
	struct usk_buff *uskb;
	uskb = __uskb_input();
	uskb_set_direct(uskb, 1);
	return uskb;
}

static inline strcut usk_buff *
uskb_alloc_out(int header_size)
{
	struct usk_buff *uskb;
	uskb = __uskb_input();
	uskb_set_direct(uskb, 0);
	return uskb;
}

void
uskb_free(struct usk_buff *uskb)
{
	free(uskb);
}
