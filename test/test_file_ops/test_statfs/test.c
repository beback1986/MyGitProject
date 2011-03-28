#include <stdio.h>
#include <sys/vfs.h>	/* for struct statfs,statfs() */

#include "error_handle.h"

#define PATH_NAME "socket:[28696]"


int main(void)
{
	int ret;
	struct statfs statfs_buf;

	ret = statfs(PATH_NAME, &statfs_buf);
	ZERO_OR_RETURN(ret, "can not stat file");

	printf("%lx\n", (long)statfs_buf.f_type);
	return 0;
}
