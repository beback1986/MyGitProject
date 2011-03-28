#include <sys/types.h>
#include <attr/xattr.h>
#include <inttypes.h>

#include "error_handle.h"

#define BUF_SIZE 300

#define FILE_PATH "/mnt/volumes/vol-2/test/sn_beta_2/readme"

int main (int argc, char *argv[])
{
	int ret;
	char name[30] = "glusterfs.open-fd-count";
	char value[BUF_SIZE];
	uint32_t *disk_layout = NULL;
	int      cnt = 0;
	int      type = 0;
	int      start_off = 0;
	int      stop_off = 0;

	memset(value, 0, BUF_SIZE);

//	ret = getxattr(FILE_PATH, "user.glusterfs-booster-mount", value, 100);
//	ret = getxattr(FILE_PATH, "glusterfs.content", value, 100);
//	ret = getxattr(FILE_PATH, "security.selinux", value, 300);
//	ret = getxattr(FILE_PATH, "glusterfs.inodelk-count", value, 300);
	ret = getxattr(FILE_PATH, "trusted.glusterfs.dht.linkto", value, BUF_SIZE);
//	ret = getxattr(FILE_PATH, "trusted.afr.127.0.0.1-2", value, 300);
	POSITIVE_OR_RETURN(ret, "can not get xattr");

/*	disk_layout = (uint32_t *)value;
	cnt = htonl (disk_layout[0]);
	type = htonl (disk_layout[1]);
	start_off = htonl (disk_layout[2]);
	stop_off = htonl (disk_layout[3]);
	printf("%d\n%d\n%u\n%u\n",
		cnt, type, start_off, stop_off);
*/
	printf("got value of size %d (%s)\n", ret, value);

	return 0;
}
