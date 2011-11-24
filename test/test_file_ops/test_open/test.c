#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

#include "error_handle.h"

#define CUR_PATH_MAX 128

//#define OPEN_MODE (O_RDWR|O_CREAT|O_APPEND)
#define OPEN_MODE (O_RDONLY)

struct options {
	char *flist_path;
	char **flist;
	long flist_count;
};

int open_write(char *file_path)
{
	int fd, ret;

//	printf("open file: %s\n", file_path);
	fd = open(file_path, OPEN_MODE);
	if (fd < 0) {
		printf("Can not open file:%s, (%d)%s\n", file_path, errno, strerror(errno));
		return 1;
	}
//	ret = write (fd, "hello", 5);
//	POSITIVE_OR_RETURN(ret, "can not write to file");
	ret = close(fd);
	POSITIVE_OR_RETURN(ret, "can not close file");

	return ret;
}

int read_flist(struct options *opts)
{
	int ret = 1, i;
	unsigned char r;
	FILE *fp;
	int plen;

	if (opts->flist_count == 0 || opts->flist_path == NULL) {
		printf("provide file path and file count\n");
		goto out;
	}

	fp = fopen(opts->flist_path, "r");
	if (fp == NULL) {
		printf("can not open flist file\n");
		goto out;
	}

	opts->flist = calloc(opts->flist_count, sizeof(char *));
	for (i=0; i<opts->flist_count; i++) {
		opts->flist[i] = calloc(1, CUR_PATH_MAX);
		if (opts->flist[i] == NULL) {
			printf("can not alloc mem\n");
			goto out;
		}
		r = fgets(opts->flist[i], CUR_PATH_MAX, fp);
		if (r == EOF) {
			printf("file end\n");
			opts->flist_count = i - 1;
			break;
		}
		plen = strlen(opts->flist[i]);
		opts->flist[i][plen - 1] = '\0';
	}

	ret = 0;
out:
	return ret;
}

struct options * parse_args(int argc, char *argv[])
{
	struct options *opts;
	int i;

	opts = calloc(1, sizeof(*opts));

	for (i=0; i<argc; i++) {
		if (strcmp(argv[i], "-f") == 0) {
			if (i == argc-1) {
				printf("missing args for -f\n");
				goto failed;
			}
			i++;
			opts->flist_path = strdup(argv[i]);
		} else if(strcmp(argv[i], "-c") == 0) {
			if (i == argc-1) {
				printf("missing args for -c\n");
				goto failed;
			}
			i++;
			printf("%s\n", argv[i]);
			opts->flist_count = atol(argv[i]);
		}
	}

	return opts;
failed:
	return NULL;
}

int main(int argc, char *argv[])
{
	struct options *opts;
	int i;

	opts = parse_args(argc, argv);
	if (opts == NULL)
		goto failed;

	read_flist(opts);

	printf("open %ld files\n", opts->flist_count);
	gettimeofday();

	for (i=0; i<opts->flist_count; i++) {
		open_write(opts->flist[i]);
	}

	return 0;
failed:
	return 1;
}
