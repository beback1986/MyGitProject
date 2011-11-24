#include <sys/ioctl.h>
#include <linux/fs.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>

#define ENFS_IOC_SHOW_FILE_LAYOUT _IO('b',2)

#define MAX_EXTS_NR     64

struct enfs_show_layout_req {
	unsigned long offset;
	unsigned int count;
};

struct enfs_blkext {
	unsigned long long pbnr;
	unsigned long long count;
};

struct enfs_flayout {
	unsigned long ext_count;
	unsigned long long total_blkcount;
	unsigned long long index_off;
	struct enfs_blkext extsinfo[MAX_EXTS_NR];
};

#define LAYOUT_REQ_SIZE ((sizeof(struct enfs_flayout)>sizeof(struct enfs_show_layout_req)?\
			sizeof(struct enfs_flayout) : sizeof(struct enfs_show_layout_req)))

int main(int argc, char *argv[])
{
	int fd = -1;
	int i;
	char *arg = NULL;
	char *file_path = NULL;
	struct enfs_show_layout_req *req;
	struct enfs_flayout *res;

	arg = strdup("hello");

	if (argc < 2) {
		printf("please input a file path!\n");
		return 1;
	}
	file_path = strdup(argv[1]);

	req = (struct enfs_show_layout_req *)calloc(1, LAYOUT_REQ_SIZE);
	res = (struct enfs_flayout *)req;
	if (!req)
		goto error;
	req->offset = 0;
	req->count = 40960;

	fd = open(file_path, O_RDONLY);
	if (fd < 0)
		goto error;

	if (ioctl(fd, ENFS_IOC_SHOW_FILE_LAYOUT, req) < 0)
		goto error;

	printf("ext_count=%lu\n",       res->ext_count);
	printf("total_blkcount=%llu\n", res->total_blkcount);
	printf("index_off=%llu\n",      res->index_off);
	for (i=0; i<res->ext_count; i++) {
		printf("ext[%d].pbnr=%llu\n", i, res->extsinfo[i].pbnr);
		printf("ext[%d].count=%llu\n", i, res->extsinfo[i].count);
	}
	close(fd);

	return 0;

error:
	if (fd >= 0)
		close(fd);
	printf("Get error(%d):%s\n", errno, strerror(errno));
	return 1;
}
