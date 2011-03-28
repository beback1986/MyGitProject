#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <pthread.h>

#include "error_handle.h"

#define MAX_THREADS 100

#define FILE_PREFIX "files/test_file"
#define MAX_FILE_PATH 100

void *test_thread (void *arg)
{
	int fd;
	int *thread_num = (int *)arg;
	char file_path[MAX_FILE_PATH];

	sprintf(file_path, "%s%d", FILE_PREFIX, *thread_num);
	printf("file_path:%s\n", file_path);
	while (1) {
		fd = open(file_path, O_RDWR|O_CREAT|O_APPEND, S_IRUSR|S_IWUSR);
		POSITIVE_OR_RETURN(fd, "can not open file");
		write (fd, "hello", 5);
		POSITIVE_OR_RETURN(fd, "can not open file");
		close (fd);
	}
	return 0;
}

int main (int argc, char *argv[])
{
	int i;
	int args[MAX_THREADS];
	void * ptr;
	pthread_t th_id[MAX_THREADS];

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
