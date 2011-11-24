#include <sys/file.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define OPEN_MODE (O_RDWR|O_CREAT)

int main(int argc, char *argv[])
{
	int fd;
	char *filepath;
	char *buf;

	buf = calloc(1, 20);

	if (argc < 2) {
		printf("input file path\n");
		return -1;
	}
	filepath = strdup(argv[1]);
	fd = open(filepath, OPEN_MODE);
	if (flock(fd, LOCK_EX) == -1) 
		printf("can not lock file\n");
	scanf("%s", buf);

	if (flock(fd, LOCK_UN) == -1)
		printf("can not unlock file\n");
	close(fd);

	return 0;
}

