/*
 * =====================================================================================
 *
 *       Filename:  task.c
 *
 *    Description:  task related function.
 *
 *        Version:  1.0
 *        Created:  04/14/2011 05:16:21 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Beback
 *        Company:  Xidian
 *
 * =====================================================================================
 */

#include <stdlib.h>

#include "task.h"

struct task *
task_create(task_routine_t rt)
{
	struct task *t;

	t = calloc(1, sizeof(struct task));
	if (!t)
		return NULL;

	t->rt = rt;

	return t;
}

int
task_start(struct task *t)
{
	return pthread_create(&t->tid, NULL, t->rt, t->arg);
}
