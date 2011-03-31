/*
 * =====================================================================================
 *
 *       Filename:  usock.h
 *
 *    Description:  userspace sock structure & related macros.
 *                  It designed to be works like struct sock in the kernel.
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

#ifndef __USOCK_H
#define __USOCK_H

#include "types.h"

struct usock {
	int mode;
};

#endif /* __USOCK_H */
