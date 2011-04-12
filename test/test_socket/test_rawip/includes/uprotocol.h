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

/* Max number of protocol. */
#define MAX_PROTO_NUM 10

struct uprotocol {
	void 		(*close)(struct sock *sk, 
				 long timeout);

	int		(*connect)(struct sock *sk, 
				   struct sockaddr *uaddr, 
				   int addr_len);

	int		(*disconnect)(struct sock *sk, 
				      int flags);

	struct sock *	(*accept) (struct sock *sk, 
				   int flags, 
				   int *err);

	int		(*ioctl)(struct sock *sk, 
				 int cmd, 
				 unsigned long arg);

	int		(*init)(struct sock *sk);

	void		(*destroy)(struct sock *sk);

	void		(*shutdown)(struct sock *sk, 
				    int how);

	int		(*setsockopt)(struct sock *sk, 
				      int level, 
				      int optname, 
				      char __user *optval,
				      int optlen);

	int		(*getsockopt)(struct sock *sk, 
				      int level, 
				      int optname, 
				      char __user *optval, 
				      int __user *option);     

	int		(*sendmsg)(struct kiocb *iocb, 
				   struct sock *sk, 
				   struct msghdr *msg, 
				   size_t len);

	int		(*recvmsg)(struct kiocb *iocb, 
				   struct sock *sk, 
				   struct msghdr *msg,
				   size_t len, 
				   int noblock, 
				   int flags,
				   int *addr_len);

	int		(*sendpage)(struct sock *sk, 
				    struct page *page,
				    int offset, 
				    size_t size, 
				    int flags);

	int		(*bind)(struct sock *sk,
				struct sockaddr *uaddr, 
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
