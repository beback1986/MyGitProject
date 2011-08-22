/*
 * =====================================================================================
 *
 *       Filename:  fstack.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/19/2011 07:01:50 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  dingyuan
 *        Company:  bwstor, China
 *
 * =====================================================================================
 */

#ifndef __FSTACK_H__
#define __FSTACK_H__

struct flist_node {
	struct flist_node *next;
	char * filename;
};

struct flist {
	struct flist_node *head;
	struct flist_node *tail;
	struct flist_node *cur_pos;
	int size;
};

struct flist *flist_new(); //impl
void flist_free(struct flist *fl);//impl
int flist_add(struct flist *fl, const char *filename);//impl
/* Return current node in the list, and move cur_pos forward */
char * flist_next(struct flist *fl);	//impl

struct dstack_node {
	struct dstack_node *prev;
	struct dstack_node *next;
	struct flist *fl;
};

struct dstack {
	struct dstack_node *head;
	struct dstack_node *tail;
};

struct dstack * dstack_new();	//impl
int dstack_isempty(struct dstack *ds);
int dstack_push_dir(struct dstack *ds);	//impl
int dstack_pop_dir(struct dstack *ds);
char * dstack_cflist_next(struct dstack *ds, char *pbuff); //impl
int dstack_cflist_add(struct dstack *ds, const char *filename); //impl

#endif /* __FSTACK_H__ */
