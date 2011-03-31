/*
 * =====================================================================================
 *
 *       Filename:  tcp_header.h
 *
 *    Description:  all TCP header related structure & macro.
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

#ifndef __TCP_HEADER_H
#define __TCP_HEADER_H

#ifndef __FAVOR_BSD
#define __FAVOR_BSD	/* use bsd'ish tcp header */
#endif
#include <netinet/tcp.h>

#endif /* __TCP_HEADER_H */
