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

#include "fstack.h"

struct flist_node *__flist_node_new(char *filename)
{
	struct flist_node *fn;

	fn = (struct flist_node *)calloc(1, sizeof(struct flist_node));
	fn->next = NULL;
	fn->filename = strdup(filename);
	return fn;
}

struct flist *flist_new()
{
	struct flist *fl;

	fl = (struct flist *)calloc(1, sizeof(struct flist));
	fl->head    = NULL;
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

int flist_reach_end(struct flist *fl)
{
	if (fl && fl->cur_pos==fl->end)
		return 1;
	return 0;
}

int flist_insert_seq(struct flist *fl, char *filename)
{
	struct flist_node *t, *fn;

	if (!fl || !filename)
		goto failed;

	fn = __flist_node_new(filename);
	t = fl->head;
	while (t) {
		if (strcmp(filename, t->filename) >= 0)
			break;
		t = t->next;
	}
	if (t == fl->head)
		fl->head = fn;
	if (t == NULL)
		fl->end = fn;
	fn->next = t;
	fl->size++;
	return 0;
failed:
	return 1;
}

int flist_pushback(struct flist *fl, char *filename)
{
	struct flist_node *fn;

	if (!fl)
		goto failed;
	fn = __flist_node_new(filename);
	if (!fn)
		goto failed;
	if (fl->end) {
		/* List is not empty,change the end pointer is enough. */
		fl->end->next = fn;
		fl->end = fn;
	}
	else {
		/* List is empty, initial all pointers. */
		fl->head = fl->end = fl->cur_pos = fn;
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
		ret = fl->cur_pos;
		fl->cur_pos = fl->cur_pos->next;
	}
	return ret;
failed:
	return NULL;
}

void flist_reset(struct flist *fl)
{
	fl->cur_pos = fl->head;
}


//////////////////////////////////////////////////////////////////////////

void __dstack_push(struct dstack *ds, struct dstack_node *dn)
{
	dn->next = ds->head;
	ds->head = dn;
}

struct dstack_node *__dstack_node_new(struct flist *fl)
{
	struct dstack_node *dn;

	dn = (struct dstack_node *)calloc(1, sizeof(struct dstack_node));
	dn->next = NULL;
	dn->fl   = fl;
	return dn;
}

struct dstack *dstack_new()
{
	struct dstack *ds;

	ds = (struct dstack *)calloc(1, sizeof(struct dstack));
	ds->head = NULL;
	return ds;
}
int dstack_empty(struct dstatck *ds)
{
	if (!ds || !ds->head)
		return 1;
	return 0;
}

int dstack_push_flist(struct dstack *ds)
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
	__dstack_push(ds, dn);

	return 0;
failed_dn:
	free(fl);
failed:
	return 1;
}
int dstack_pop_flist(struct dstack *ds)
{
	struct flist *fl;

	if (!ds)
		goto failed;

	fl = ds->head;
	ds->head = ds->head->next;
	flist_free(fl);
	return 0;
failed:
	return 1;
}

int dstack_cflist_end(struct dstack *ds)
{
	struct flist *fl;
	if (!ds->head || !ds->head->fl)
		goto out;
	fl = ds->head->fl;
	return flist_end(fl);
out:
	return 0;
}

char *dstack_cflist_next(struct dstack *ds)
{
	char *filename;

	if (!ds)
		goto failed;

	filename = flist_next(ds->head);
	return filename;
failed:
	return NULL;
}

int dstack_cflist_pushback(struct dstack *ds, char *filename)
{
	if (!ds)
		goto failed;

	return file_list_pushback(ds->head, filename);
failed:
	return NULL;
}

int dstack_cflist_insert_seq(struct dstack *ds, char *filename)
{
	if (!ds)
		goto failed;

	return file_list_instert_seq(ds->head, filename);
failed:
	return NULL;
}
