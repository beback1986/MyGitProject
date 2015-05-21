#define _GNU_SOURCE
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <inttypes.h>

//#define OPEN_MODE (O_RDWR|O_CREAT)
//#define OPEN_MODE (O_RDONLY)
#define OPEN_MODE (O_RDWR|O_CREAT|O_DIRECT)

#define WORD_SIZE 512
#define WORD_COUNT 0

struct options {
//	append direct read write;
	int 		 mode;
	uint32_t 	 block_size;
	int 		 block_count;
	uint32_t	 file_pos;
	char 		*filepath;
};
struct options g_opts;

struct thread_info {
	struct options *opts;
	int fd;
};
//struct thread_info th_info;

/*
 * align_size MUST be order of 2.
 * the size of alloced memory must be twice of align_size.
 */
static void *memalign(void *p, unsigned long align_size)
{
	return (void *)(((unsigned long)p & ~(align_size-1)) + align_size);
}

void *routine(void *arg)
{
	struct thread_info *th_info = (struct thread_info *)arg;
	struct options *opts = th_info->opts;
	int fd = th_info->fd;
	char *buf = NULL;
	int i, ret;
	off_t cur_size = 0;

	buf = malloc(opts->block_size*2);
	if (buf == NULL || th_info == NULL) {
		printf("Can not alloc mem.(%d)%s\n", errno, strerror(errno));
		goto out;
	}
	buf = (char *)memalign((void *)buf, opts->block_size);
	memset(buf, 'a', opts->block_size);
	lseek(fd, opts->file_pos, SEEK_SET);
	for (i=0; i<opts->block_count; i++){
		ret = write(fd, buf, opts->block_size);
		if (ret < 0) {
			printf("Can not write to file.(%d)%s\n", 
				errno, strerror(errno));
			goto out;
		}
		cur_size+=ret;

/*		ret = read(fd, buf, opts->block_size);
		if (ret < 0) {
			printf("Can not read from file.(%d)%s\n", 
				errno, strerror(errno));
			goto out;
		}
		printf("%d bytes readed\n", ret);
		cur_size+=ret;*/

/*		ret = fsync(fd);
		if (ret < 0) {
			printf("Can not sync to file.(%d)%s\n", 
				errno, strerror(errno));
			goto out;
		}

		ret = fstat(fd, &st_buf);
		if (ret < 0) {
			printf("Can not stat file.(%d)%s\n", 
				errno, strerror(errno));
			goto out;
		}
		if (cur_size != st_buf.st_size) {
			printf("error occurs!!!!!\n");
		}*/
	}

out:
/*	if (fd >= 0)
		close(fd);*/
	printf("thread exit.\n");
	return NULL;
}

int controler(int fd)
{
	char *buf = NULL;
	int ret;
	int i;

	buf = calloc(1, WORD_SIZE);
	if (buf == NULL) {
		printf("Can not alloc mem.(%d)%s\n", errno, strerror(errno));
		goto failed;
	}

	memset(buf, 'b', WORD_SIZE);
	for (i=0; i<WORD_COUNT; i++) {
		printf("input to continue:");
		getchar();

		ret = write(fd, buf, WORD_SIZE);
		if (ret < 0) {
			printf("Can not write to file.(%d)%s\n", 
				errno, strerror(errno));
			goto failed;
		}
		printf("write finished.\n");
	}
	return 0;
failed:
	return -1;
}

int parse_args(int argc, char *argv[], struct options *opts)
{
	int i;

	memset(opts, '\0', sizeof(*opts));

	/* default optsion */
	opts->block_size = 4096;
	opts->block_count = 1024;
	opts->file_pos = 512;

	for (i=1; i<argc; i++) {
		if (strcmp(argv[i], "-d") == 0) {
			opts->mode |= O_DIRECT;
		} else if (strcmp(argv[i], "-a") == 0) {
			opts->mode |= O_APPEND;
		} else if (strcmp(argv[i], "-b") == 0) {
			if (i == argc-1) {
				printf("missing args for -b\n");
				goto failed;
			}
			i++;
			opts->block_size = atol(argv[i]);
		} else if(strcmp(argv[i], "-c") == 0) {
			if (i == argc-1) {
				printf("missing args for -c\n");
				goto failed;
			}
			i++;
			opts->block_count = atol(argv[i]);
		} else {
			if (!opts->filepath)
				opts->filepath = strdup(argv[i]);
		}
	}
	if (!opts->filepath) {
		printf("provide a file path\n");
		goto failed;
	}

	return 0;
failed:
	return 1;
}

int getfd(struct options *opts)
{
	return open(opts->filepath, OPEN_MODE, 0644);
}

int main(int argc, char *argv[])
{
	struct thread_info *th_info;
	pthread_t th_id = -1;
	int fd = -1;

	if (parse_args(argc, argv, &g_opts))
		return -1;

	fd = getfd(&g_opts);
	if (fd < 0) {
		printf("Can not open file.(%d)%s\n", errno, strerror(errno));
		goto out;
	}

	th_info = calloc(1, sizeof(*th_info));
	th_info->fd = fd;
	th_info->opts = &g_opts;
	pthread_create(&th_id, NULL, routine, (void *)th_info);

	controler(fd);

out:
	if (th_id != -1) {
		printf("wait thread:%lu to exit\n", th_id);
		pthread_join(th_id, NULL);
	}

	if (fd >= 0) {
		close(fd);
	}
	return 0;
}
