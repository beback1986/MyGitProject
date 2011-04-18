#include <sys/file.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	int fd;
	FILE *fp;
	char *filepath;
	char *buf;

	buf = calloc(1, 20);

	if (argc < 2) {
		printf("input file path\n");
		return -1;
	}
	filepath = strdup(argv[1]);
	fp = fopen(filepath, "w+");
	fd = fileno(fp);
	if (flock(fd, LOCK_EX) == -1) 
		printf("can not lock file\n");
	scanf("%s", buf);

	if (flock(fd, LOCK_UN) == -1)
		printf("can not unlock file\n");
	fclose(fp);

	return 0;
}
