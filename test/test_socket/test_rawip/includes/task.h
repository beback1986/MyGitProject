/*
 * =====================================================================================
 *
 *       Filename:  task.h
 *
 *    Description:  task related function.
 *
 *    	  * For application to start a task, you must follow:
 *    	    1.task_create()        :Create a task, not run.
 *    	    2.[opt]task_arg_set()  :Set arg for the task.
 *    	    3.task_start()         :Actually start the task.
 *
 *    	  * For task routine to get args, you must follow: 
 *	    1.task_get()       :To get struct task from void *
 *	    2.task_arg_get()   :To get arg from struct task
 *
 *        * For application to wait for a task, you must follow:
 *          task_wait() or task_timewait()
 *
 *        * For application to stop a task, you must follow:
 *          task_stop()
 *
 *    	  * For task routine to return ret, you must follow: 
 *	    task_ret_get()   :To get arg from struct task
 *
 *    	  * For application to get ret, you must follow: 
 *	    task_ret_set()   :To get arg from struct task
 *
 *        Version:  1.0
 *        Created:  04/14/2011 01:21:32 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Beback
 *        Company:  Xidian
 *
 * =====================================================================================
 */

#ifndef __TASK_H
#define __TASK_H

#include <pthread.h>

typedef void *(*task_routine_t) (void *t);

struct task {
	pthread_t 	tid;		/* Thread id. */
	task_routine_t 	rt;
	int		status;		/* The status of belong task. */
	void 		*arg;
	void 		*ret;
};

#define task_get(p)		\
	((struct task *)(p))

#define task_arg_set(t, p)	\
	((t)->arg = (p))

#define task_arg_get(t, type)	\
	((type)(t)->arg)

#define task_ret_set(t, p)	\
	((t)->ret = (p))

#define task_ret_get(t, type)	\
	((type)(t)->ret)

extern struct task *
task_create();

extern int
task_start(struct task *t);

extern int
task_stop(struct task *t);

extern int
task_wait(struct task *t);

extern int
task_timewait(struct task *t);

#endif /* __TASK_H */
