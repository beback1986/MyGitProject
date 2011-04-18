#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>
#include "error_handle.h"

#define MAX_THREADS 1

#define BUF_SIZE 1048576
#define BUF_COUNT 40960

#define DEFAULT_FILE_PREFIX "/cluster2/test1/test_file"

#define MAX_FILE_PATH 100

#define NON_BLOCK_MODE 1

char *file_prefix;

char *buf = NULL;

void debuger(int counter)
{
	printf("test write for %d times\n", counter);
}

int
sys_open(const char *file_path, int flags, mode_t mode)
{
	int ret;
#ifdef NON_BLOCK_MODE
	flags |= O_NONBLOCK;
//	printf("open file:%s in NONBLOCK mode\n", file_path);
#endif
	ret = open(file_path, flags, mode);

	return ret;
}

ssize_t
sys_write(int fd, const void *buf, size_t count)
{
	ssize_t ret;

	ret = write(fd, buf, count);
#ifdef NON_BLOCK_MODE
	if (ret == EAGAIN) {
		ret = 0;
		printf("debug_ding:non block mode continue to write\n");
	}
#endif
	return ret;
}

int
sys_close(int fd)
{
	return close(fd);
}

void *
test_thread (void *arg)
{
	int fd;
	int ret;
	int i;
	int loop_counter = 0;
	int *thread_num = (int *)arg;
	char file_path[MAX_FILE_PATH];

	sprintf(file_path, "%s%d", file_prefix, *thread_num);
	printf("file_path:%s\n", file_path);
	i = BUF_COUNT;
	while (i-->0) {
		fd = sys_open(file_path, O_RDWR|O_CREAT|O_APPEND, S_IRUSR|S_IWUSR);
		POSITIVE_OR_RETURN(fd, "can not open file");

		ret = sys_write (fd, buf, BUF_SIZE);
		POSITIVE_OR_RETURN(ret, "write error");

		ret = sys_write (fd, buf, BUF_SIZE);
		POSITIVE_OR_RETURN(ret, "write error");

		ret = sys_close (fd);
		ZERO_OR_RETURN(ret, "can not close file");

		loop_counter++;
		if ((loop_counter%30)==0) {
			debuger(loop_counter);
		}
	}
	return 0;
}

int main (int argc, char *argv[])
{
	int i;
	int args[MAX_THREADS];
	void * ptr;
	pthread_t th_id[MAX_THREADS];

	if (argc > 1) {
		file_prefix = argv[1];
		printf("use user specified file location:%s\n", file_prefix);
	}
	else {
		file_prefix = DEFAULT_FILE_PREFIX;
		printf("use default file location:%s\n", file_prefix);
	}

	buf = malloc(BUF_SIZE);
	if(buf < 0){
		printf("can not alloc memory");
		return -1;
	}
	memset(buf, BUF_SIZE, '\0');

	for (i=0; i<MAX_THREADS; i++){
		args[i] = i;
		pthread_create (&th_id[i], NULL, test_thread, (void *)&args[i]);
	}
	for (i=0; i<MAX_THREADS; i++) {
		printf ("waiting for thread %d\n", i);
		pthread_join (th_id[i], NULL);
	}
	return 0;
}
