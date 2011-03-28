#include <stdio.h>
//#include <utime.h>	/* for struct utimbuf,utime() */
#include <sys/stat.h>	/* for struct stat,stat() */

#include "error_handle.h"

#define PATH_NAME "/mnt/glusterfs-mnt-1/ding/yuan/hello"

int main(void)
{
	int ret;
	struct stat stat_buf;

	ret = stat(PATH_NAME, &stat_buf);
	ZERO_OR_RETURN(ret, "can not stat file");

	printf("%u\n", stat_buf.st_ino);
	return 0;
}
