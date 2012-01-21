#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "vmdio.h"

#define VOLUME	"/dev/sdc"
#define BUFSIZE (51200000)

int readdev(const char *volume, int64_t offset, uint64_t length, void *data)
{
        int rc, fd;

        fd = open(volume, O_RDONLY);
        if (fd == -1)
                return -1;

        lseek64(fd, offset, SEEK_SET);
        rc = read(fd, data, length);
        if (rc <= 0)
                goto err;

        close(fd);

	return 0;
err:
        close(fd);
        return rc;
}

void show_result(char *buf, int size)
{
	int c;
	char *p;

	c = 0;
	p = buf;
	while (c < size) {
		printf("%x ", *p);
		p++;
		c += sizeof(*p);
		if (c%32 == 0)
			printf("\n");
	}
}

void test_libvmdio(void)
{
	int ret;
	char *buf;
	bwvmdio_device_t *bwdev;
	bwvmdio_aio_t *bwaio;

	buf = calloc(1, BUFSIZE);

	bwdev = bwvmdio_open_dev("/dev/sda");
	if (!bwdev) {
		printf("bwvmdio_open_dev failed.\n");
		goto out;
	}

	ret = bwvmdio_read(bwdev, 4096000, BUFSIZE, buf, BWVMDIO_ASYNC, &bwaio);
	if (ret) {
		printf("read fail.%d\n", ret);
		goto out;
	}

	ret = bwvmdio_wait(bwdev, bwaio, 100);
	if (ret) {
		printf("wait fail.%d\n", ret);
		goto out;
	}
	show_result(buf, 512);

out:
	return ;
}

int main(int argc, char *argv[])
{
#if 0
	int ret;
	char *buf;
	char path[10] = VOLUME;

	buf = calloc(1, BUFSIZE);
	if (!buf)
		goto failed;

	ret = readdev(path, 31*4096, BUFSIZE, buf);
	if (ret < 0) {
		printf("return ret=%d\n", ret);
		goto failed;
	}

	printf("return ret=%d\n", ret);
#endif

	test_libvmdio();
	return 0;
}
