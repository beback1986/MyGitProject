#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>

#include "error_handle.h"

#define BUF_SIZE 4096

#define OPEN_MODE (O_RDONLY)

#if 0
/*
 * align_size MUST be order of 2.
 * the size of alloced memory must be twice of align_size.
 */
static void *memalign(void *p, unsigned long align_size)
{
	return (void *)(((unsigned long)p & ~(align_size-1)) + align_size);
}
#endif

/************************************************************
 * Functions for worker inplements.
 ***********************************************************/
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
	struct worker_group *wg = NULL;
	int i, ret;

	wg = calloc(1, sizeof(*wg));
	if (!wg)
		goto failed;
	wg->count = count;
	wg->wlist = calloc(count, sizeof(struct worker));
	wg->ids = calloc(count, sizeof(pthread_t));
	if (!wg->wlist || !wg->ids)
		goto failed;
	memcpy(wg->wlist, wlist, sizeof(struct worker)*count);

	for (i=0; i<count; i++) {
		ret = pthread_create(&wg->ids[i], NULL, 
				wg->wlist[i].wroutine, wg->wlist[i].wargs);
		if (ret) {
			printf("create thread failed!\n");
			goto failed;
		}
	}

	return wg;
failed:
	if (wg)
		free(wg);
	return NULL;
}

int workers_wait(struct worker_group *wg)
{
	while (1) 
		sleep(10);

	return 0;
}

void workers_stop(struct worker_group *wg)
{
	int i, ret;

	for (i=0; i<wg->count; i++) {
		ret = pthread_cancel(wg->ids[i]);
		if (ret)
			printf("Can not stop pthread:%d\n", (int)wg->ids[i]);
	}
}

/*************************************************************
 * For options parse.
 ************************************************************/
struct options {
	int	 read_num;
	int	 write_num;
	char	*flist_path;
	char	*read_flist_path;
	char	*write_flist_path;
	size_t	 block_size;
};

struct options *parse_options(int argc, char *argv[])
{
	struct options *opts = NULL;
	int i;

	opts = calloc(1, sizeof(struct options));
	if (!opts) {
		printf("Can not alloc mem.\n");
		goto failed;
	}
	opts->read_num = 100;
	opts->block_size = 32*1024;

	for (i=1; i<argc; i++) {
		if (strcmp(argv[i], "-r") == 0) {
			if (i == argc-1) {
				printf("missing args for -r\n");
				goto failed;
			}
			i++;
			opts->read_num = atol(argv[i]);
		} else if (strcmp(argv[i], "-w") == 0) {
			if (i == argc-1) {
				printf("missing args for -b\n");
				goto failed;
			}
			i++;
			opts->write_num = atol(argv[i]);
		} else if(strcmp(argv[i], "-b") == 0) {
			if (i == argc-1) {
				printf("missing args for -b\n");
				goto failed;
			}
			i++;
			opts->block_size = atol(argv[i])*1024;
		} else {
			if (!opts->read_flist_path)
				opts->read_flist_path = strdup(argv[i]);
			else if (!opts->write_flist_path)
				opts->write_flist_path = strdup(argv[i]);
		}
	}
	if (!opts->read_flist_path || !opts->write_flist_path) {
		printf("provide a file path\n");
		goto failed;
	}

	return opts;
failed:
	return NULL;
}

#define TOTAL_THREAD_NUM(opts) ((opts)->read_num + (opts)->write_num)

/************************************************************
 * For file list generation.
 ************************************************************/
struct flist {
	char **list;
	unsigned int count;
};

int get_flist(char *flist_path, struct flist **pflist)
{
	int ret = 1, count = 0, i;
	struct flist *flist;
	char *buf;
	char *r;
	FILE *fp;

	buf = calloc(1, BUF_SIZE);
	(*pflist) = flist = calloc(1, sizeof(*flist));
	if (!buf || !flist) {
		printf("Can not alloc mem for path buf.\n");
		goto out;
	}

	fp = fopen(flist_path, "r");
	if (fp == NULL) {
		printf("can not open flist file\n");
		goto out;
	}

	/* Get file list count. alloc mem for pointer list of file path. */
	while (1) {
		r = fgets(buf, BUF_SIZE, fp);
		if (r == NULL)
			break;
		count++;
	}
	fseek(fp, 0, SEEK_SET);
	flist->list = calloc(count, sizeof(char *));
	if (!flist->list) {
		printf("Can not alloc mem\n");
		goto out;
	}

	/* Get file list. */
	for (i=0; i<count; i++) {
		memset(buf, '\0', BUF_SIZE);
		r = fgets(buf, BUF_SIZE, fp);
		if (r == NULL) {
			printf("file end, count=%d, i=%d\n", count, i);
			count = i + 1;
			break;
		}
		flist->list[i] = strdup(buf);
		if (!flist->list[i]) {
			printf("Can not alloc mem.\n");
			goto out;
		}
		flist->list[i][strlen(buf)-1] = '\0';
	}

	free(buf);
	fclose(fp);
	flist->count = count;
	ret = 0;

out:
	return ret;
}

/************************************************************
 * For reader workers.
 ***********************************************************/

struct reader {
	struct flist 		*flist;
	size_t 			 size;
	char 			*buf;
	unsigned int		count;
//	struct random_data 	rdata;
	unsigned int 		seed;
};

struct reader *init_reader(size_t size, unsigned int count, unsigned int seed, struct flist *flist)
{
	struct reader *reader = NULL;

	reader = calloc(1, sizeof(struct reader));
	if (!reader) {
		printf("Can not alloc mem for readers.\n");
		goto failed;
	}

	reader->flist 	= flist;
	reader->size 	= size;
	reader->buf 	= calloc(1, size);
	reader->count	= count;
	if (!reader->buf) {
		printf("Can not alloc buf for reader.\n");
		goto failed;
	}
	reader->seed = seed;

	return reader;
failed:
	if (reader)
		free(reader);
	return NULL;
}

unsigned long long reader_counter = 0;;

void *reader_routine(void *args)
{
	struct reader *reader = (struct reader *) args;
	struct flist *flist = reader->flist;
	int num, fd, ret;

	while (1) {
//		num = rand_r(&reader->seed) % flist->count;
		reader->seed+=10;
		reader->seed%=flist->count;
		num = reader->seed;
		fd = open(flist->list[num], O_RDONLY);
		if (fd < 0) {
			printf("read open encounter %s %d(%s)\n",
				flist->list[num], errno, strerror(errno));
			continue;
		}
		ret = read(fd, reader->buf, reader->size);
		if (ret < 0)
			printf("read encounter %s %d(%s)\n",
				flist->list[num], errno, strerror(errno));
		close(fd);
		reader_counter++;
	}

	printf("read %s exit!%d(%s)\n", flist->list[num], errno, strerror(errno));
	return NULL;
}

unsigned long long writer_counter = 0;;

void *writer_routine(void *args)
{
	struct reader *reader = (struct reader *) args;
	struct flist *flist = reader->flist;
	int num, fd, ret, i;

//printf("start writer: begin=%lu,count=%lu\n", reader->seed, reader->count);
//	for (i=0; i<reader->count; i++) {
	while (1) {
		reader->seed+=1;
		num = reader->seed % flist->count;
		fd = open(flist->list[num], O_RDWR|O_CREAT, 644);
		if (fd < 0) {
			printf("write open encounter %s %d(%s)\n",
				flist->list[num], errno, strerror(errno));
			continue;
		}
		ret = write(fd, reader->buf, reader->size);
		if (ret < 0)
			printf("write encounter %s %d(%s)\n",
				flist->list[num], errno, strerror(errno));
		close(fd);
		writer_counter++;
	}

	printf("writer exit!\n");
	return NULL;
}

/************************************************************
 * For main and its helper functions.
 ***********************************************************/

#define IO_WORKER_READ 0
#define IO_WORKER_WRITE 1

int io_worker_generate(struct worker *worker, unsigned int worker_count, int size, struct flist *flist, int type)
{
	unsigned int seed = 7;
	int i;

	if (!worker_count) 
		goto out;

	if (worker_count > flist->count || flist->count % worker_count) {
		printf("total file number should be multiple of worker_count");
		goto failed;
	}

	for (i=0; i<worker_count; i++) {
		if (type == IO_WORKER_READ) {
			worker[i].wroutine = reader_routine;
		} else if (type == IO_WORKER_WRITE) {
			worker[i].wroutine = writer_routine;
		} else {
			printf("Unkown ioworker!\n");
			goto failed;
		}
		worker[i].wargs = init_reader(size, 0, rand_r(&seed), flist);
//		worker[i].wargs = init_reader(size, (flist->count/worker_count), (flist->count/worker_count*i), flist);
		if (!worker[i].wargs) {
			printf("Can not init reader\n");
			goto failed;
		}
	}

out:
	return 0;
failed:
	return -1;
}

int main(int argc, char *argv[])
{
	struct worker 		*worker_list;
	struct worker_group 	*worker_group;
	struct options 		*opts;
	struct flist 		*read_flist;
	struct flist 		*write_flist;
	int 			 ret;

	opts = parse_options(argc, argv);
	if (!opts) {
		printf("parse options error!\n");
		goto out;
	}

	ret = get_flist(opts->read_flist_path, &read_flist);
	ret |= get_flist(opts->write_flist_path, &write_flist);
	if (ret) {
		printf("get_flist error!\n");
		goto out;
	}

	worker_list = calloc(TOTAL_THREAD_NUM(opts), sizeof(struct worker));
	if (!worker_list) {
		printf("Can not alloc mem for worker list.\n");
		goto out;
	}

	ret = io_worker_generate(worker_list, opts->read_num, opts->block_size, read_flist, IO_WORKER_READ);
	if (ret) {
		printf("gen workers failed!\n");
		goto out;
	}

	ret = io_worker_generate((worker_list + opts->read_num), opts->write_num, opts->block_size, write_flist, IO_WORKER_WRITE);
	if (ret) {
		printf("gen workers failed!\n");
		goto out;
	}

	printf("begin to work.\n");
	worker_group = workers_create(worker_list, TOTAL_THREAD_NUM(opts));
	if (!worker_group) {
		printf("Create workers failed!\n");
		goto out;
	}

	while (1) {
		printf("reader:%llu io, size:%dKB\n", reader_counter, opts->block_size);
		reader_counter = 0;
		printf("writer:%llu io, size:%dKB\n", writer_counter, opts->block_size);
		writer_counter = 0;
		sleep(5);
	}

out:
	return 0;
}

#if 0 //Currently no need to implement counter
struct counter_args {
	size_t block_size;
	int thread_num;
	struct reader *;
};

void *counter_worker(void *args)
{
	return NULL;
}
#endif

#if 0 //Old test.
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
#endif
