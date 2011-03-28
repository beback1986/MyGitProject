#include <sys/stat.h>
#include <sys/uio.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include "optcode_test.h"

#define PATH_LEN 100
#define MOUNT_SOURCE "/tmp/image_mount"
#define MOUNT_POINT "/mnt/test2"
#define FSTYPE "fuse.glusterfs"

#define BUF_SIZE 1024

typedef int (fuse_handler_t) (int fd, struct fuse_in_header *finh, void *msg);

int test_fuse_init (int fd, struct fuse_in_header *finh, void *msg)
{
	int ret;
	struct fuse_init_in *fini = msg;
	struct fuse_init_out fino;

	struct fuse_out_header fouh;
	struct iovec iov[2];

printf("%s called\n", __func__);
	fino.major = fini->major;
	fino.minor = fini->minor;
	fino.max_readahead = 1 << 17;
	fino.max_write = 1 << 17;
	fino.flags = FUSE_ASYNC_READ | FUSE_POSIX_LOCKS;
	/* these values seemed to work fine during testing */
	fino.max_background = 64;
	fino.congestion_threshold = 48;

	iov[0].iov_base = &fouh;
	iov[0].iov_len = sizeof(fouh);
	iov[1].iov_base = &fino;
	iov[1].iov_len = sizeof(fino);

	fouh.len = iov[0].iov_len + iov[1].iov_len;
	fouh.unique = finh->unique;
	fouh.error = 0;

	ret = writev(fd, iov, 2);
	if (ret <= 0) {
		printf("write error.errno %d\n", errno);
		return -1;
	}
	printf("write %d bytes\n", ret);
	return 0;
}

static fuse_handler_t *fuse_ops[100] = {
	test_fuse_init,
	NULL
};

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

int test_fuse_cntl (int fd)
{
	int i, ret;
	struct iovec iov[2];
	struct fuse_in_header *finh;
	struct fuse_init_in *fini;

	iov[0].iov_len = sizeof(*finh);// + sizeof(struct fuse_write_in);
	iov[0].iov_base = calloc(1, sizeof(*finh));
	iov[1].iov_len = BUF_SIZE;
	iov[1].iov_base = calloc(1, BUF_SIZE);

	i = 100;
	while (i) {
		ret = readv (fd, iov, 2);
		if (ret > 0) {
			printf("readv recieve %d bytes\n", ret);
			finh = (struct fuse_in_header *) iov[0].iov_base;
			fini = (struct fuse_init_in *) iov[1].iov_base;
			printf("len:%d,opcode:%d,unique:%d,nodeid:%d,uid:%d,gid:%d,pid:%d,padding:%d\n",
				finh->len,
				finh->opcode,
				finh->unique,
				finh->nodeid,
				finh->uid,
				finh->gid,
				finh->pid,
				finh->padding);
			printf("major:%d,minor:%d,max_readahead:%d,flags:%d\n",
				fini->major,
				fini->minor,
				fini->max_readahead,
				fini->flags);
			if (finh->opcode == 26)
				fuse_ops[0] (fd, finh, fini);
			else
				printf("can not recogonise thie op\n");
		}
		else
			printf("read nothing\n");
		i--;
	}
	return 0;
}

int main (int argc, char *argv[])
{
	int fd;

	fd = test_fuse_mount ();
	if (fd < 0) {
		printf ("error ocoured. %d\n", errno);
		return -1;
	}
	test_fuse_cntl (fd);

	close (fd);
	return 0;
}
