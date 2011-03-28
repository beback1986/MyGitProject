#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include "error_handle.h"

#define BUF_SIZE 8192

#define FILE_TO_OPEN "/mnt/glusterfs-mnt/test/test_file0"

int main (int argc, char *argv[])
{
	int fd;
	int ret = 1;
	int total_size = 0;
	char *buf;

	buf = calloc(1, BUF_SIZE);

	fd = open(FILE_TO_OPEN, O_RDONLY);
	POSITIVE_OR_RETURN(fd, "can not open file");

	while (ret > 0) {
		sleep (4);
		ret = read (fd, buf, BUF_SIZE);
		POSITIVE_OR_RETURN(ret, "can not write to file");
		total_size += ret;
		printf("read: %s\n", buf);
	}

	printf("total size is %d\n", total_size);

	close(fd);

	return 0;
}
