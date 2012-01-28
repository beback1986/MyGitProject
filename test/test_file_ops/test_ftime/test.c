#include <stdio.h>
#include <utime.h>	/* for struct utimbuf,utime() */
#include <sys/stat.h>	/* for struct stat,stat() */
#include <errno.h>	/* for perror() */

int main(void)
{
/*	struct utimbuf utb;
	utb.modtime = 100000;
	if(utime("/home/beback/test_fs",&utb)){
		perror();
		return -1;
	}
	printf("%x\n",utb.modtime);
	return 1;
*/
	struct stat stat_buf;
	if(stat("/home/beback/test_fs",&stat_buf)){
		perror("test_ftime:");
		return -1;
	}
	printf("%x\n",stat_buf.st_mtime);
	return 0;
}
