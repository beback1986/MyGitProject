#include <sys/ioctl.h>
#include <sys/uio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include "error_handle.h"

#ifndef O_DIRECT
#define O_DIRECT	00040000
#endif

#define BLOCK_SIZE 1024
//#define BLOCK_COUNT 1000000000
#define BLOCK_COUNT 1000000

#define FILE_PATH "/mnt/glusterfs-mnt-1/test/hello1"

int main(int argc, char *argv[])
{
	int fd;
	int ret;
	int i;
	double block_size = BLOCK_SIZE;
	double block_count = BLOCK_COUNT;
	suseconds_t cost;
	double cost_d;
	double speed;
	struct iovec iov;
	struct timeval begin;
	struct timeval end;

//	iov.iov_base = calloc (1, BLOCK_SIZE);
//	iov.iov_len = BLOCK_SIZE;

//	gettimeofday(&begin, NULL);

	fd = open(FILE_PATH, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR);
	POSITIVE_OR_RETURN(fd, "can not open file");

	while (1)
		write (fd, "www", 3);
/*	for (i=0; i<BLOCK_COUNT; i++){
		ret = writev(fd, &iov, 1);
		POSITIVE_OR_CONTINUE(ret, "write error continue");
	}*/

	close(fd);

//	gettimeofday(&end, NULL);

/*	cost = (end.tv_sec - begin.tv_sec)*1000000 + end.tv_usec - begin.tv_usec;
	cost_d = cost;
	speed = (block_size * block_count) / cost_d / 1024.0 / 1024.0 * 1000000.0;

	printf("begin at:%d microseconds, %d seconds\n", begin.tv_usec, begin.tv_sec);
	printf("end at:%d microseconds, %d seconds\n", end.tv_usec, end.tv_sec);
	printf("write %d blocks with %d bytes per block to disk\n", BLOCK_COUNT, BLOCK_SIZE);
	printf("time used:%d, io speed:%f\n", cost, speed);*/
}
