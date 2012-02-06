/*
 * =====================================================================================
 *
 *       Filename:  bwtimer_lin.h
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

#ifndef __BWTIMER_LIN_H__
#define __BWTIMER_LIN_H__

#ifndef __BWTIMER_H__
#error "You should not include bwtimer_lin.h direct!"
#error "Include bwtimer.h instead!"
#endif

#include <sys/time.h>

#include "bwlist.h"

struct _bwtimer_queue {
	struct bwlist_head timer_list;
	struct timespec sleep_time;
};

struct _bwtimer_timer {
	struct bwlist_head list;
	struct itimerval tick;
};

#endif /* __BWTIMER_LIN_H__ */
