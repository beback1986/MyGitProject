/*
 * =====================================================================================
 *
 *       Filename:  mempool.c
 *
 *    Description:  memory management functions.
 *
 *        Version:  1.0
 *        Created:  04/11/2011 01:09:39 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Beback
 *        Company:  Xidian
 *
 * =====================================================================================
 */


#include <string.h>
#include <stdlib.h>

#include "types.h"
#include "mempool.h"


#define MEMPOOL_NEXT_IDLE(pool)						\
	while ((pool)->status[(pool)->cursor] != MEM_IDLE){		\
		(pool)->cursor++;					\
	} 

struct mempool *
mempool_create(const char *name, int size, int count)
{
 	struct mempool *pool;
	int i;

	pool = calloc(1, sizeof(struct mempool));

	pool->name = strdup(name);
	pool->size = size;
	pool->count = count;
	pool->cursor = 0;
	pool->mempod = calloc(count, sizeof(void *));
	if(!pool->mempod)
		goto failed;
	for (i=0; i<count; i++) {
		pool->mempod[i] = calloc(1, size + sizeof(struct mem_status));
		if (!pool->mempod[i])
			goto failed;
		pool->mempod[i]+=sizeof(struct mem_status);
		MEM_SET_STATUS(pool->mempod[i], MEM_IDLE);
	}

	return pool;

failed:
	return NULL;
}

void *
mempool_alloc(struct mempool *pool)
{
	int i;

	while(1) {
		if (MEM_STATUS(pool->mempod[pool->cursor]) == MEM_IDLE) {
			i = pool->cursor;
			pool->cursor++;
			pool->cursor %= pool->count;
			break;
		}
		pool->cursor++;
		pool->cursor %= pool->count;
	}

	MEM_SET_STATUS(pool->mempod[i], MEM_USED);
	return pool->mempod[i];
}

void
mempool_free(void *ptr)
{
	MEM_SET_STATUS(ptr, MEM_IDLE);
}
