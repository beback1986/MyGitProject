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

#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>

#include "timer.h"
#include "list.h"

/*
 * struct timeval compare function.
 *  1: first >  second
 *  0: first == second
 * -1: first <  second
 */
static inline int __timeval_cmp(const struct timeval *first, const struct timeval *second)
{
	uint64_t first_total, second_total;
	int ret;

	first_total   = (uint64_t)first->tv_sec * 1000000 + (uint64_t)first->tv_usec;
	second_total  = (uint64_t)second->tv_sec * 1000000 + (uint64_t)second->tv_usec;

	if (first_total > second_total) {
		ret = 1;
	} else if (first_total < second_total) {
		ret = -1;
	} else {
		ret = 0;
	}

	return ret;
}

void __timeval_add(struct timeval *tmret, const struct timeval *tmadd)
{
	uint64_t tmret_total, tmadd_total;

	tmret_total  = (uint64_t)tmret->tv_sec * 1000000 + (uint64_t)tmret->tv_usec;
	tmadd_total  = (uint64_t)tmadd->tv_sec * 1000000 + (uint64_t)tmadd->tv_usec;

	tmret_total += tmadd_total;

	tmret->tv_sec  = tmret_total / 1000000;
	tmret->tv_usec = tmret_total % 1000000;
}

void __timeval_sub(struct timeval *tmret, const struct timeval *tmsub)
{
	uint64_t tmret_total, tmsub_total;

	tmret_total  = (uint64_t)tmret->tv_sec * 1000000 + (uint64_t)tmret->tv_usec;
	tmsub_total  = (uint64_t)tmsub->tv_sec * 1000000 + (uint64_t)tmsub->tv_usec;

	if (tmret_total < tmsub_total)
		tmret_total = 0;
	else
		tmret_total -= tmsub_total;

	tmret->tv_sec  = tmret_total / 1000000;
	tmret->tv_usec = tmret_total % 1000000;
}

/*
 * Insert a timer to queue list. Keep the list a priority queue.
 * Uset timer->tm_timeo to jurge. So this filed shuold be caculated before
 * invake this routine.
 */
void __bw_timer_queue_insert(bw_timer_queue_t *queue, bw_timer_timer_t *timer)
{
	bw_timer_timer_t *cur;
#if 0
	bw_timer_timer_t *head;

	bw_list_for_each_entry(head, &queue->tmq_list, tm_node) {
		printf("BWTIMER:DEBUG: before inster:tick=%lu.%6lu, timeo=%lu.%lu\n",
				head->tm_tick.tv_sec, head->tm_tick.tv_usec, 
				head->tm_timeo.tv_sec, head->tm_timeo.tv_usec);
	}
#endif
	/* TODO:Check this timer whether belong to this queue or not. */
	bw_list_for_each_entry(cur, &queue->tmq_list, tm_node) {
		if (__timeval_cmp(&timer->tm_timeo, &cur->tm_timeo) < 0) {
			break;
		}
	}

	bw_list_add_tail(&timer->tm_node, &cur->tm_node);

	queue->tmq_count++;

#if 0
	bw_list_for_each_entry(head, &queue->tmq_list, tm_node) {
		printf("BWTIMER:DEBUG: after  inster:tick=%lu.%6lu, timeo=%lu.%lu\n",
				head->tm_tick.tv_sec, head->tm_tick.tv_usec, 
				head->tm_timeo.tv_sec, head->tm_timeo.tv_usec);
	}
#endif
	return ;
}

void __bw_timer_queue_delete(bw_timer_queue_t *queue, bw_timer_timer_t *timer)
{
	/* TODO:Check this timer whether belong to this queue or not. */
	bw_list_del(&timer->tm_node);
	queue->tmq_count--;
}

/*
 * If queue is empty, deactive this timer.
 * Otherwise, use the queue head's timeo to update queue's timeo.
 * 0: Success
 * -1:Failed
 */
int __bw_timer_queue_update_timeo(bw_timer_queue_t *queue)
{
	bw_timer_timer_t *head;
	struct itimerspec *timeo = &queue->tmq_timeo;
	struct timeval tmnow;
	struct timeval tmnext;

	memset(timeo, 0, sizeof (struct itimerspec));
	if (queue->tmq_count == 0) {
		printf("BWTIMER: queue is empty, can not update timeo\n");
		timeo->it_value.tv_sec  = 0;
		timeo->it_value.tv_nsec = 0;
		goto update;
	}

	bw_list_for_each_entry(head, &queue->tmq_list, tm_node) {
		tmnext = head->tm_timeo;
		break;
	}

	gettimeofday(&tmnow, NULL);
	__timeval_sub(&tmnext, &tmnow);
	if (tmnext.tv_sec == 0 && tmnext.tv_usec == 0) {
		printf("BWTIMER: Queue head is too old, update timeo failed.\n");
		goto failed;
	}

	timeo->it_value.tv_sec  = tmnext.tv_sec;
	timeo->it_value.tv_nsec = tmnext.tv_usec * 1000;

update:
	printf("BWTIMER:DEBUG: Update timeo to:%lu,%lu, spec:%lu,%lu\n", 
				tmnext.tv_sec, tmnext.tv_usec,
				timeo->it_value.tv_sec, timeo->it_value.tv_nsec);
	timer_settime(queue->tmq_id, 0, timeo, NULL);
	return 0;
failed:
	return -1;
}

/*
 * Update timer's timeo field, use system time.
 * begin=1: use tm_begin to update.
 * begin=0: use tm_tick to update.
 */
void __bw_timer_timer_update_timeo(bw_timer_timer_t *timer, int begin)
{
	struct timeval tmv;
	struct timeval tmnow;

	if (begin)
		tmv = timer->tm_begin;
	else
		tmv = timer->tm_tick;

	gettimeofday(&tmnow, NULL);

	__timeval_add(&tmnow, &tmv);
	timer->tm_timeo = tmnow;
}

void __bw_timer_routine(sigval_t val)
{
	bw_timer_timer_t *head;
	bw_timer_queue_t *queue = val.sival_ptr;
	BW_LIST_HEAD(timeo_list);
	struct timeval tmnow;

retry:
	gettimeofday(&tmnow, NULL);
	bw_list_for_each_entry(head, &queue->tmq_list, tm_node) {
		break;
	}
	if(__timeval_cmp(&head->tm_timeo, &tmnow) > 0)
		goto out;
	__bw_timer_queue_delete(queue, head);
	head->tm_handler(head->tm_args);
	__bw_timer_timer_update_timeo(head, 0);
	__bw_timer_queue_insert(queue, head);
	goto retry;

out:
	if (__bw_timer_queue_update_timeo(queue)) {
		printf("BWTIMER: update queue time out failed. Retry.\n");
		goto retry;
	}
}

bw_timer_queue_t *bw_timer_create_queue(void)
{
	bw_timer_queue_t *queue = NULL;
	int ret;

	queue = calloc(1, sizeof(bw_timer_queue_t));
	if (!queue) {
		printf("BWTIMER: Can not alloc mem for timer queue.\n");
		goto failed;
	}

	BW_INIT_LIST_HEAD(&queue->tmq_list);

	queue->tmq_sige.sigev_value.sival_ptr	= queue;
	queue->tmq_sige.sigev_signo		= SIGALRM;
	queue->tmq_sige.sigev_notify		= SIGEV_THREAD;
	queue->tmq_sige.sigev_notify_function	= __bw_timer_routine;
	queue->tmq_sige.sigev_notify_attributes	= NULL;
	ret = timer_create(CLOCK_REALTIME, &queue->tmq_sige, &queue->tmq_id);
	if (ret) {
		printf("BWTIMER: timer_create failed. %d(%s)\n", errno, strerror(errno));
		goto failed;
	}

	queue->tmq_count = 0;

	return queue;
failed:
	if (queue)
		free(queue);
	return NULL;
}

bw_timer_timer_t *bw_timer_create_timer(struct timeval tv_begin,
		struct timeval tv_tick, bw_timer_handler_t handler, void *args)
{
	bw_timer_timer_t *timer = NULL;

	timer = calloc(1, sizeof(bw_timer_timer_t));
	if (!timer) {
		printf("BWTIMER: Can not alloc mem for timer.\n");
		goto failed;
	}

	BW_INIT_LIST_HEAD(&timer->tm_node);
	timer->tm_begin   = tv_begin;
	timer->tm_tick    = tv_tick;
	timer->tm_handler = handler;
	timer->tm_args    = args;

	return timer;
failed:
	if (timer)
		free(timer);
	return NULL;
}

int32_t bw_timer_queue_timer(bw_timer_queue_t *queue, bw_timer_timer_t *timer)
{
	int ret;

	__bw_timer_timer_update_timeo(timer, 1);
	__bw_timer_queue_insert(queue, timer);
	ret = __bw_timer_queue_update_timeo(queue);
	if (ret)
		printf("BWTIMER: queue timer update timeo failed, ignore\n");

	return 0;
}

int32_t bw_timer_unqueue_timer(bw_timer_queue_t *queue, bw_timer_timer_t *timer)
{
	return 0;
}
