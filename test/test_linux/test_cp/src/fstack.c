/*
 * =====================================================================================
 *
 *       Filename:  fstack.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/19/2011 08:29:31 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  dingyuan
 *        Company:  bwstor, China
 *
 * =====================================================================================
 */

#include <stdlib.h>
#include <string.h>

#include "fstack.h"

struct flist_node *__flist_node_new(const char *filename)
{
	struct flist_node *fn;

	fn = (struct flist_node *)calloc(1, sizeof(struct flist_node));
	fn->next = NULL;
	fn->prev = NULL;
	fn->filename = strdup(filename);
	return fn;
}

struct flist *flist_new()
{
	struct flist *fl;

	fl = (struct flist *)calloc(1, sizeof(struct flist));
	fl->head    = NULL;
	fl->tail    = NULL;
	fl->cur_pos = NULL;
	fl->size    = 0;
	return fl;
}

void flist_free(struct flist *fl)
{
	struct flist_node *fn;
	if (!fl)
		return;
	while (fl->head) {
		fn = fl->head;
		fl->head = fl->head->next;
		free(fn->filename);
		free(fn);
	}
}

int flist_add(struct flist *fl, const char *filename)
{
	struct flist_node *t, *fn;

	if (!fl || !filename)
		goto failed;

	fn = __flist_node_new(filename);
	t = fl->head;
	while (t) {
		if (strcmp(filename, t->filename) <= 0)
			break;
		t = t->next;
	}
	if (!fl->head) {
		/* Insert into an empty list. */
		fn->prev = fn->next = NULL;
		fl->head = fl->tail = fl->cur_pos = fn;
	}
	else if (!t) {
		/* Insert into the tail of the list */
		fl->tail->next = fn;
		fn->prev = fl->tail;
		fn->next = NULL;
		fl->tail = fn;
	}
	else if (!t->prev) {
		/* Insert into the head of the list */
		fl->head->prev = fn;
		fn->next = fl->head;
		fn->prev = NULL;
		fl->cur_pos = fl->head = fn;
	}
	else {
		/* Insert into a normal place in the list */
		t->prev->next = fn;
		fn->prev = t->prev;
		t->prev = fn;
		fn->next = t;
	}
	fl->size++;
	return 0;
failed:
	return 1;
}

char *flist_next(struct flist *fl)
{
	char * ret = NULL;
	if (!fl)
		goto failed;
	if (fl->cur_pos) {
		ret = fl->cur_pos->filename;
		fl->cur_pos = fl->cur_pos->next;
	}
	return ret;
failed:
	return NULL;
}

int flist_prev(struct flist *fl)
{
	if (!fl)
		goto failed;
	if (!fl->cur_pos)
		fl->cur_pos = fl->tail;
	else
		fl->cur_pos = fl->cur_pos->prev;
	return 0;

failed:
	return 1;
}

char * flist_current(struct flist *fl)
{
	if (!fl || !fl->cur_pos)
		return NULL;
	return fl->cur_pos->filename;
}

//////////////////////////////////////////////////////////////////////////

struct dstack_node *__dstack_node_new(struct flist *fl)
{
	struct dstack_node *dn;

	dn = (struct dstack_node *)calloc(1, sizeof(struct dstack_node));
	dn->prev = NULL;
	dn->next = NULL;
	dn->fl   = fl;
	return dn;
}

struct dstack *dstack_new()
{
	struct dstack *ds;

	ds = (struct dstack *)calloc(1, sizeof(struct dstack));
	ds->head = NULL;
	ds->tail = NULL;
	return ds;
}
int dstack_isempty(struct dstack *ds)
{
	if (!ds || !ds->head)
		return 1;
	return 0;
}

int dstack_push_dir(struct dstack *ds)
{
	struct flist *fl;
	struct dstack_node *dn;

	if (!ds)
		goto failed;
	fl = flist_new();
	if (!fl)
		goto failed;
	dn = __dstack_node_new(fl);
	if (!dn)
		goto failed_dn;

	dn->next = ds->head;
	ds->head = dn;
	if (dn->next)
		dn->next->prev = dn;
	if (!ds->tail)
		ds->tail = dn;

	return 0;
failed_dn:
	free(fl);
failed:
	return 1;
}

int dstack_pop_dir(struct dstack *ds)
{
	struct dstack_node *dn;

	if (!ds)
		goto failed;

	dn = ds->head;
	if (!dn)
		/* Reach the botton of the stack */
		goto failed;
	ds->head = dn->next;
	if (dn->next)
		dn->next->prev = NULL;
	else
		/* This node is the last one in stack... */
		ds->tail = NULL;
	flist_free(dn->fl);
	free(dn);
	return 0;
failed:
	return 1;
}

int *dstack_cflist_next(struct dstack *ds, char *pbuff)
{
	char *filename;
	struct dstack_node *dn;

	if (!ds || !ds->head)
		goto failed;

	if (!flist_current(ds->head->fl))
		goto failed;

	dn = ds->tail;
	while (dn) {
		filename = flist_current(dn->fl);
//		if (!filename)
//			goto failed;
		pbuff = strcat(pbuff, "/");
		pbuff = strcat(pbuff, filename);
		dn = dn->prev;
	}
	filename = flist_next(ds->head->fl);
	return 0;
failed:
	return 1;
}

int dstack_cflist_prev(struct dstack *ds)
{
	if (!ds || !ds->head)
		goto failed;

	return flist_prev(ds->head->fl);

failed:
	return 1;
}

int dstack_cflist_add(struct dstack *ds, const char *filename)
{
	if (!ds || !ds->head)
		goto failed;

	return flist_add(ds->head->fl, filename);
failed:
	return 1;
}
