/*
 * =====================================================================================
 *
 *       Filename:  bwtimer.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/06/2012 02:29:55 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  DingYuan
 *        Company:  BWSTOR
 *
 * =====================================================================================
 */

#ifndef __BWTIMER_H__
#define __BWTIMER_H__

#define LINUX

#ifdef WINDOWS
#include "bwtimer_win.h"
#elif LINUX
#include "bwtimer_lin.h"
#else
#error "Unkown platform!!!"
#endif

struct _bwtimer_queue;
struct _bwtimer_timer;

typedef struct _bwtimer_queue bwtimer_queue_t;
typedef struct _bwtimer_timer bwtimer_timer_t;


bwtimer_queue_t *bwtimer_create_queue(void);

bwtimer_timer_t *bwtimer_create_timer(int32_t begin_sec, int32_t iter_sec);

int32_t bwtimer_queue_timer(bwtimer_queue_t *queue, bwtimer_timer_t *timer);

#endif /* __BWTIMER_H__ */
