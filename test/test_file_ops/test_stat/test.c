#define _BSD_SOURCE

#include <stdio.h>
//#include <utime.h>	/* for struct utimbuf,utime() */
#include <sys/stat.h>	/* for struct stat,stat() */
#include <sys/types.h>

#include "error_handle.h"

int main(int argc, char *argv[])
{
	int ret;
	char *file_path;
	struct stat stat_buf;

	if (argc < 2) {
		printf("Provide file path.\n");
		goto out;
	}

	file_path = strdup(argv[1]);

	ret = stat(file_path, &stat_buf);
	ZERO_OR_RETURN(ret, "can not stat file");

	printf("st_ino=%u, st_mode=%o\n", stat_buf.st_ino, stat_buf.st_mode);
	printf("st_dev:major=%d, minor=%d, sizeof dev=%lu\n",
			major(stat_buf.st_dev),
			minor(stat_buf.st_dev),
			sizeof(stat_buf.st_dev));
	printf("st_rdev:major=%d, minor=%d, sizeof dev=%lu\n",
			major(stat_buf.st_rdev),
			minor(stat_buf.st_rdev),
			sizeof(stat_buf.st_rdev));
	if (stat_buf.st_rdev)
		printf("this file is a special file\n");
	else
		printf("this file is not a special file\n");

out:
	return 0;
}
