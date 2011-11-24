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
