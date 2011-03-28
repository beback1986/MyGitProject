#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

#include "error_handle.h"

#define BUF_SIZE 128

#define FILE_TO_OPEN "/mnt/glusterfs-mnt-1/test/hello1"

int main (int argc, char *argv[])
{
	int fd, ret;
	char *buf;

	buf = calloc(1, BUF_SIZE);

//	fd = open(FILE_TO_OPEN, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR);
	fd = open(FILE_TO_OPEN, O_RDWR);
	POSITIVE_OR_RETURN(fd, "can not open file");

//	ret = write (fd, "hello", 5);
//	POSITIVE_OR_RETURN(ret, "can not write to file");

	close(fd);

	return 0;
}
