/*
 * =====================================================================================
 *
 *       Filename:  bw_timer.h
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

#ifndef __BW_TIMER_H__
#define __BW_TIMER_H__

#define BWOSL_LIN_USER

/* For struct timeval. */
#if defined BWOSL_LIN_USER
#include <time.h>
#elif defined BWOSL_WIN_USER
#else
#error "Unkown platform!!!"
#endif

typedef int32_t (*bw_timer_handler_t)(void *);

#if defined BWOSL_LIN_USER
#include "timer_lin.h"
#elif defined BWOSL_WIN_USER
#include "timer_win.h"
#else
#error "Unkown platform!!!"
#endif

struct _bw_timer_queue;
struct _bw_timer_timer;

typedef struct _bw_timer_queue bw_timer_queue_t;
typedef struct _bw_timer_timer bw_timer_timer_t;

extern bw_timer_queue_t *bw_timer_create_queue(void);

extern bw_timer_timer_t *bw_timer_create_timer(struct timeval tv_begin, 
				struct timeval tv_tick, bw_timer_handler_t handler);

extern int32_t bw_timer_queue_timer(bw_timer_queue_t *queue, bw_timer_timer_t *timer);

extern int32_t bw_timer_unqueue_timer(bw_timer_queue_t *queue, bw_timer_timer_t *timer);

#endif /* __BW_TIMER_H__ */
