/*
 * =====================================================================================
 *
 *       Filename:  uprotocol.h
 *
 *    Description:  protocol related structure, functions & macro.
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

#ifndef __UPROTOCOL_H
#define __UPROTOCOL_H

#include "list.h"

#include "usock.h"

/* Max number of protocol. */
#define MAX_PROTO_NUM 10

struct uprotocol {
	void 		(*close)(struct usock *sk, 
				 long timeout);

	int		(*connect)(struct usock *sk, 
				   struct sockaddr_in *uaddr, 
				   int addr_len);

	int		(*disconnect)(struct usock *sk, 
				      int flags);

	struct usock *	(*accept) (struct usock *sk, 
				   int flags, 
				   int *err);

	int		(*ioctl)(struct usock *sk, 
				 int cmd, 
				 unsigned long arg);

	int		(*init)(struct usock *sk);

	void		(*destroy)(struct usock *sk);

	void		(*shutdown)(struct usock *sk, 
				    int how);

	int		(*setsockopt)(struct usock *sk, 
				      int level, 
				      int optname, 
				      char *optval,
				      int optlen);

	int		(*getsockopt)(struct usock *sk, 
				      int level, 
				      int optname, 
				      char *optval, 
				      int *option);     
#if 0
	int		(*sendmsg)(struct kiocb *iocb, 
				   struct usock *sk, 
				   struct msghdr *msg, 
				   size_t len);

	int		(*recvmsg)(struct kiocb *iocb, 
				   struct usock *sk, 
				   struct msghdr *msg,
				   size_t len, 
				   int noblock, 
				   int flags,
				   int *addr_len);

	int		(*sendpage)(struct usock *sk, 
				    struct page *page,
				    int offset, 
				    size_t size, 
				    int flags);
#endif
	int		(*bind)(struct usock *sk,
				struct sockaddr_in *uaddr, 
				int addr_len);

	char		name[32];

	int		proto_num;

	struct list_head	node;

	/* Hooks for lower layer */
/*MUST*/int		(*header_len)(struct usk_buff *uskb); 

/*MUST*/int		(*handler)(struct usk_buff *uskb);
};

extern int 
uprotocol_register(struct uprotocol *proto);

extern struct uprotocol *
uprotocol_find(int proto_num);

#endif /* __UPROTOCOL_H */
