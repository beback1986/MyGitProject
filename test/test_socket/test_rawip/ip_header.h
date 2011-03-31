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
#ifndef __USE_BSD
#define __USE_BSD	
#endif
#include <netinet/ip.h>

#endif /* __IP_HEADER_H */
