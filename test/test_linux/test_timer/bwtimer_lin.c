/*
 * =====================================================================================
 *
 *       Filename:  bwtimer_lin.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/06/2012 02:44:08 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  DingYuan
 *        Company:  BWSTOR
 *
 * =====================================================================================
 */

#include <stdlib.h>

#include "bwtimer.h"
#include "bwlist.h"


bwtimer_queue_t *bwtimer_create_queue(void)
{
	bwtimer_queue_t *queue = NULL;

	queue = calloc(1, sizeof(bwtimer_queue_t));
	if (!queue) {
		printf("BWTIMER: Can not alloc mem for timer queue.\n");
		goto out;
	}

	bwlist_head_init(queue->timer_list);
	queue->sleep_time.tv_sec = INIT;
	queue->sleep_time.tv_nsec = 0;

out:
	return queue;
}

bwtimer_timer_t *bwtimer_create_timer(int32_t begin_sec, int32_t iter_sec)
{
	bwtimer_timer_t *timer = NULL;

	timer = calloc(1, sizeof(bwtimer_timer_t));
	if (!timer) {
		printf("BWTIMER: Can not alloc mem for timer.\n");
		goto out;
	}

	bwlist_head_init(timer->list);
	timer->tick.it_interval.tv_sec = begin_sec;
	timer->tick.it_value.tv_sec = iter_sec;

out:
	return timer;
}

int32_t bwtimer_queue_timer(bwtimer_queue_t *queue, bwtimer_timer_t *timer)
{
}
