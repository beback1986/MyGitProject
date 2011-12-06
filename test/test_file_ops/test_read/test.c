#define _GNU_SOURCE
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include "error_handle.h"

#define BUF_SIZE (4096)

#define OPEN_MODE (O_RDONLY)

/*
 * align_size MUST be order of 2.
 * the size of alloced memory must be twice of align_size.
 */
static void *memalign(void *p, unsigned long align_size)
{
	return (void *)(((unsigned long)p & ~(align_size-1)) + align_size);
}

/*
 * For option phrase.
 */

struct flist {
	char **list;
	long count;
};

struct options {
	int   thread_num;
	char *flist_path;
	size_t block_size;
};

int read_flist(struct options *opts, struct flist **pflist)
{
	int ret = 1, count = 0;
	struct flist *flist;
	char *path;
	unsigned char r;
	FILE *fp;
	int plen;

	path = calloc(1, CUR_PATH_MAX);
	(*pflist) = flist = calloc(1, sizeof(*flist));
	if (!path || !flist) {
		printf("Can not alloc mem for path.\n");
		goto out;
	}

	fp = fopen(opts->flist_path, "r");
	if (fp == NULL) {
		printf("can not open flist file\n");
		goto out;
	}

	r = 'a';
	while (r != EOF) {
		r = fgets(NULL, CUR_PATH_MAX, fp);
		count++;
	}
	fseek(fp, 0, SEEK_SET);
	flist->count = count
	flist->list = calloc(count, sizeof(char *));
	if (!flist->list) {
		printf("Can not alloc mem\n");
		goto out;
	}

	r = 'a';
	while (1) {
		memset(path, '\0', CUR_PATH_MAX);
		r = fgets(path, CUR_PATH_MAX, fp);
		if (r == EOF) {
			printf("file end\n");
			opts->flist_count = i - 1;
			break;
		}
		flist->list[i] = strdup(path);
		if (!flist->list[i]) {
			printf("Can not alloc mem.\n");
			goto out;
		}
		flist->list[i][strlen(path)] = '\0';
	}

	fclose(fp);
	ret = 0;

out:
	return ret;
}

/*
 * For read & counter threads.
 */

struct read_args {
	struct flist 	*flist;
	unsigned int 	 seed;
	size_t 		 block_size;
	int 		 last_counter;
	int 		 counter; 		/* Run counter. */
};

struct read_args *init_reader(size_t block_size, unsigned int seed, struct flist *flist)
{
	struct read_args *read
}

void *read_worker(void *)
{
}

struct counter_args {
	size_t block_size;
	int thread_num;
	struct read_args *;
};

void *counter_worker(void *)
{
}

/*
 * Functions for worker.
 */
typedef void *(*worker_routine_t)(void *);

struct worker {
	worker_routine_t wroutine;
	void * wargs;
};

struct worker_group {
	int count;
	struct worker *wlist;
	pthread_t *ids;
};

struct worker_group *workers_create(struct worker wlist[], int count)
{
	struct worker_group *wg;
	int i, ret;

	wg = calloc(1, sizeof(*wg));
	if (!wg)
		goto failed;
	wg->count = count;
	wg->wlist = calloc(count, sizeof(worker));
	wg->ids = calloc(count, sizeof(pthread_t));
	if (!wg->wlist || !wg->ids)
		goto failed;
	memcpy(wg->wlist, wlist, sizeof(worker)*count);

	for (i=0; i<count; i++) {
		ret = pthread_create(wlist->ids[i], NULL, 
				wg->wlist[i].wroutine, wg->wlist[i].wargs);
		if (ret) {
			printf("create thread failed!\n");
			goto failed;
		}
	}

failed:
	return NULL;
}

int workers_wait()
{
	return 0;
}

void workers_stop(struct worker_group *wg)
{
	int i, ret;

	for (i=0; i<wg->count; i++) {
		ret = pthread_cancel(wg->ids[i]);
		if (ret)
			printf("Can not stop pthread:%d\n", wg->ids[i]);
	}
}

int main (int argc, char *argv[])
{
	int fd;
	int ret = 1;
	int total_size = 0;
	char *buf;
	char *filepath;

	buf = calloc(1, BUF_SIZE*2);
	if (!buf) {
		printf("can not alloc buf\n");
		return -1;
	}
	buf = memalign(buf, BUF_SIZE);

	if (argc < 2) {
		printf("input file path\n");
		return -1;
	}
	filepath = strdup(argv[1]);
	fd = open(filepath, OPEN_MODE);
	POSITIVE_OR_RETURN(fd, "can not open file");

	while (ret > 0) {
		memset(buf, '\0', BUF_SIZE);
		ret = read (fd, buf, BUF_SIZE);
		POSITIVE_OR_RETURN(ret, "can not write to file.");
		if (ret == 0) {
			printf("reach the end.\n");
			break;
		}
		total_size += ret;
		printf("read %d words:\n", ret);
	}

	printf("total size is %d\n", total_size);

	close(fd);

	return 0;
}
