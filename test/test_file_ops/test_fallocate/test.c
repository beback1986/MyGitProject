#include <linux/falloc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#define _XOPEN_SOURCE 600
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define OPEN_MODE (O_RDWR|O_CREAT)

int alloc_keep(int fd, loff_t off, loff_t len)
{
//	return fallocate(fd, FALLOC_FL_KEEP_SIZE, off, len);
	return posix_fallocate(fd, off, len);
}

int alloc_unkeep(int fd, loff_t off, loff_t len)
{
//	return fallocate(fd, 0, off, len);
	return posix_fallocate(fd, off, len);
}

struct options {
	char *file_path;
	int keep;
	loff_t off;
	loff_t len;
};

void test(const struct options *opts)
{
	int fd = 0, ret = 0;
	loff_t off_now = 0;
	char *buf = NULL;
	ssize_t write_bytes = 0;

	buf = calloc(1, opts->len);
	if (!buf) {
		printf("Can not alloc mem.\n");
		goto failed;
	}

	fd = open(opts->file_path, OPEN_MODE, 0644);
	if (fd < 0) {
		printf("Can not open file.(%d)%s\n", errno, strerror(errno));
		goto failed;
	}

#if 1
	if (opts->keep)
		ret = alloc_keep(fd, opts->off, opts->len);
	else
		ret = alloc_unkeep(fd, opts->off, opts->len);
	if (ret) {
		printf("Can not fallocate file.ret = %d (%d)%s\n", ret, errno, strerror(errno));
		goto failed;
	}
#endif

#if 0
	off_now = lseek(fd, opts->off, SEEK_SET);
	if (off_now != opts->off) {
		printf("Can not lseek file.%lld (%d)%s\n",
				off_now, errno, strerror(errno));
		goto failed;
	}

	memset(buf, 1, opts->len);
	write_bytes = write(fd, buf, opts->len);
	if (write_bytes < 0) {
		printf("Can not write. (%d)%s\n", errno, strerror(errno));
		goto failed;
	}
#endif

	printf("test succeed.\n");
	return ;

failed:
	printf("test failed.\n");
	return ;
}

int parse_args(int argc, char *argv[], struct options *opts)
{
	int i;

	opts->keep = 1;
	opts->off = 1024;
	opts->len = 4096;

	for (i=1; i<argc; i++) {
		if (strcmp(argv[i], "-o") == 0) {
			if (i == argc-1) {
				printf("missing args for -o\n");
				goto failed;
			}
			opts->off = atoll(argv[++i]);
		} else if (strcmp(argv[i], "-l") == 0) {
			if (i == argc-1) {
				printf("missing args for -l\n");
				goto failed;
			}
			opts->len = atoll(argv[++i]);
		} else if (strcmp(argv[i], "-m") == 0) {
			if (i == argc-1) {
				printf("missing args for -l\n");
				goto failed;
			}
			i++;
			if (strcmp(argv[i], "keep_size") == 0) {
				opts->keep = 1;
			} else if  (strcmp(argv[i], "unkeep_size") == 0) {
				opts->keep = 0;
			} else {
				printf("Unkown args %s for -m\n", argv[i]);
				goto failed;
			}
		} else {
			if (!opts->file_path) {
				opts->file_path = strdup(argv[i]);
			} else {
				printf("invalid option %s.\n", argv[i]);
				goto failed;
			}
		}
	}

	if (!opts->file_path) {
		printf("provide a file path\n");
		goto failed;
	}

	return 0;
failed:
	return 1;
}

int main(int argc, char *argv[])
{
	struct options opts = {0};

	if (parse_args(argc, argv, &opts))
		return -1;
	printf("test info:\n");
	printf("\tfile_path=%s\n", opts.file_path);
	printf("\tkeep=%d\n", opts.keep);
	printf("\toffset=%lld\n", opts.off);
	printf("\tlength=%lld\n", opts.len);
	test(&opts);

	return 0;
}
