/*
 * =====================================================================================
 *
 *       Filename:  atomic.h
 *
 *    Description:  atomic opertions. Make use of pthread and semaphore.
 *
 *        Version:  1.0
 *        Created:  04/11/2011 09:15:05 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Beback
 *        Company:  Xidian
 *
 * =====================================================================================
 */

#ifndef __ATOMIC_H
#define __ATOMIC_H

#define ATOMIC_USE_PTHREAD

#ifdef ATOMIC_USE_PTHREAD

#include <pthread.h>

typedef {
	int 		data;
	pthread_mutex_t data_loc;
	pthread_cond_t 	data_cond;
} atomic_t;

static inline void
atomic_init(atomic_t *atom)
{
	pthread_mutex_init(atom->data_loc, NULL);
	pthread_cond_init(atom->data_cond, NULL);
}

static inline void
atomic_inc(atomic_t *atom)
{
	pthread_mutex_lock(&atom->data_loc);
	if (atom->data == 0)
		pthread_cond_broadcast(&atom->data_cond);
	atom->data++;
	pthread_mutex_unlock(&atom->data_loc);
}

static inline void
atomic_dec(atomic_t *atom)
{
	pthread_mutex_lock(&atom->data_loc);
	while (atom->data < 1)
		pthread_cond_wait(&atom->data_cond, &atom->data_loc);
	atom->data--;
	pthread_mutex_unlock(&atom->data_loc);
}

#else


#endif /*ATOMIC_USE_PTHREAD*/


#endif /* __ATOMIC_H */
