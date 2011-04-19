/*
 * =====================================================================================
 *
 *       Filename:  mempool.h
 *
 *    Description:  memory management structure & functions defination.
 *
 *        Version:  1.0
 *        Created:  04/11/2011 11:25:22 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Beback
 *        Company:  Xidian
 *
 * =====================================================================================
 */

#ifndef __MEMPOOL_H
#define __MEMPOOL_H

#include "types.h"

/* TODO: Implement mempool with better performance. */

struct mem_status {
	int status;
};

#define MEM_PTR_TO_STATUS(ptr)						\
	((struct mem_status *)((ptr)-sizeof(struct mem_status)))

#define MEM_STATUS(ptr)							\
	(MEM_PTR_TO_STATUS(ptr)->status)

#define MEM_SET_STATUS(ptr, s)						\
	(MEM_PTR_TO_STATUS(ptr)->status = s)

struct mempool {
	char 	*name;
	int 	size;
	int 	count;

	/* 'cursor' is an point to current allocated memory position. */
	int	cursor;

	/* There is a status at the begining of each memory block.
	 * It use enum below to represent the status of the belonging block. 
	 */
	void	**mempod;
};

enum {
	MEM_IDLE = 1,
	MEM_USED,
};

extern struct mempool *
mempool_create(const char *name, int size, int count);

extern void *
mempool_alloc(struct mempool *pool);

extern void
mempool_free(void *ptr);

/* TODO: Implement functions below
extern void 
mempool_destroy(struct mempool *pool);

extern struct void
mempool_expand(struct mempool *pool, int count);

extern struct void
mempool_shrink(struct mempool *pool, int count);
*/

#endif /* __MEMPOOL_H */
