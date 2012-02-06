/*
 * =====================================================================================
 *
 *       Filename:  bwlist.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/06/2012 03:21:13 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  DingYuan
 *        Company:  BWSTOR
 *
 * =====================================================================================
 */

#ifndef __BWLIST_H__
#define __BWLIST_H__

struct bwlist_head {
	struct bwlist_head *next, *prev;
};

static inline void bwlist_head_init(struct list_head *list)
{
        list->next = list;
        list->prev = list;
}

#endif /* __BWLIST_H__ */
