#define _BSD_SOURCE

#include <stdio.h>
//#include <utime.h>	/* for struct utimbuf,utime() */
#include <sys/stat.h>	/* for struct stat,stat() */
#include <sys/types.h>

#include "error_handle.h"

#define PATH_NAME "/usr/src"

int main(void)
{
	int ret;
	struct stat stat_buf;

	ret = stat(PATH_NAME, &stat_buf);
	ZERO_OR_RETURN(ret, "can not stat file");

	printf("ino=%u\n", stat_buf.st_ino);
	printf("major=%d, minor=%d, sizeof dev=%u\n",
			major(stat_buf.st_dev),
			minor(stat_buf.st_dev),
			sizeof(stat_buf.st_dev));
	return 0;
}
