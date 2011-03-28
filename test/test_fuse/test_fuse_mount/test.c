#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#define PATH_LEN 100
#define MOUNT_SOURCE "/tmp/image_mount"
#define MOUNT_POINT "/mnt/test"
#define FSTYPE "fuse.glusterfs"

int test_fuse_mount ()
{
	int fd, ret;
	char source[PATH_LEN] = MOUNT_SOURCE;
	char mountpoint[PATH_LEN] = MOUNT_POINT;
	char fstype[30] = FSTYPE;
	char mnt_param_mnt[1024];

	fd = open ("/dev/fuse", O_RDWR);
	if (fd == -1) {
		printf ("cannot open /dev/fuse errno:%d, errstr:%s\n", errno, strerror (errno));
		return -1;
	}
	
	sprintf (mnt_param_mnt,
			"allow_other,default_permissions,max_read=131072,fd=%i,rootmode=%o,user_id=%i,group_id=%i",
			fd, S_IFDIR, getuid (), getgid ());

	ret = mount (source, mountpoint, fstype, 0, mnt_param_mnt);

	if (ret) {
		printf ("cannot mount errno:%d, errstr:%s\n", errno, strerror (errno));
		close (fd);
		return -1;
	}
	return fd;
}

int main (int argc, char *argv[])
{
	int fd;

	fd = test_fuse_mount();
	if (fd < 0)
		return -1

	close (fd);

	return 0;
}
