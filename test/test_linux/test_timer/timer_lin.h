/*
 * =====================================================================================
 *
 *       Filename:  bw_timer_lin.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/06/2012 02:42:10 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  DingYuan
 *        Company:  BWSTOR
 *
 * =====================================================================================
 */

#ifndef __BW_TIMER_LIN_H__
#define __BW_TIMER_LIN_H__

#ifndef __BW_TIMER_H__
#error "You should not include timer_lin.h direct!"
#error "Include timer.h instead!"
#endif

#include "list.h"

struct _bw_timer_queue {
	struct bw_list_head tmq_list;
};

struct _bw_timer_timer {
	struct bw_list_head tm_node;
	bw_timer_handler_t  tm_handler;
	/* Time before first time out. */
	struct timeval      tm_begin;
	/* Time out tick. */
	struct timeval      tm_tick;
	/* Next time out. */
	struct timeval      tm_next_timeo;
};

#endif /* __BW_TIMER_LIN_H__ */
