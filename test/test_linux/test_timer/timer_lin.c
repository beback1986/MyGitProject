/*
 * =====================================================================================
 *
 *       Filename:  bw_timer_lin.c
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

#include "timer.h"
#include "bwlist.h"


bw_timer_queue_t *bw_timer_create_queue(void)
{
	bw_timer_queue_t *queue = NULL;

	queue = calloc(1, sizeof(bw_timer_queue_t));
	if (!queue) {
		printf("BWTIMER: Can not alloc mem for timer queue.\n");
		goto failed;
	}

	bwlist_head_init(queue->timer_list);

	return queue;
failed:
	if (queue)
		free(queue);
	return NULL;
}

bw_timer_timer_t *bw_timer_create_timer(struct timeval tv_begin,
				struct timeval tv_tick, bw_timer_handler_t handler)
{
	bw_timer_timer_t *timer = NULL;

	timer = calloc(1, sizeof(bw_timer_timer_t));
	if (!timer) {
		printf("BWTIMER: Can not alloc mem for timer.\n");
		goto failed;
	}

	BW_INIT_LIST_HEAD(timer->tm_node);
	timer->tm_begin   = ;
	timer->tm_tick    = ;
	timer->tm_handler = ;

	return timer;
failed:
	if (timer)
		free(timer);
	return NULL;
}

int32_t bw_timer_queue_timer(bw_timer_queue_t *queue, bw_timer_timer_t *timer)
{
	return 0;
}
