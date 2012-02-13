#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

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

void async_callback(bw_vmdio_error_t err, uint32_t len, void *args)
{
	char *buf = args;

	printf("test_callback:err=%d,len=%u,pid=%lu\n", err, len, pthread_self());

	show_result(buf, 512);

	return ;
}

void test_libvmdio_sync(void)
{
	bw_vmdio_device_t *bwdev = NULL;
	bw_vmdio_aio_t    *bwaio = NULL;
	bw_vmdio_error_t   bwerr;
	char              *buf;

	buf = calloc(1, BUFSIZE);

	bwdev = bw_vmdio_open_dev("/dev/sda");
	if (!bwdev) {
		printf("bw_vmdio_open_dev failed.\n");
		goto out;
	}

	bwaio = bw_vmdio_aio_create(BW_VMDIO_SYNC, NULL, NULL);
	if (!bwaio) {
		printf("create bw_vmdio_aio_t failed.\n");
		goto out;
	}

	printf("begin to read.pid=%lu\n", pthread_self());
	bwerr = bw_vmdio_read(bwdev, 4096000, BUFSIZE, buf, bwaio);
	if (bwerr) {
		printf("read fail.%d\n", bwerr);
		goto out;
	}

	show_result(buf, 512);

out:
	bw_vmdio_aio_delete(bwaio);
	return ;
}

void test_libvmdio_async_callback(void)
{
	bw_vmdio_device_t *bwdev = NULL;
	bw_vmdio_aio_t    *bwaio = NULL;
	bw_vmdio_error_t   bwerr;
	char              *buf;

	buf = calloc(1, BUFSIZE);

	bwdev = bw_vmdio_open_dev("/dev/sda");
	if (!bwdev) {
		printf("bw_vmdio_open_dev failed.\n");
		goto out;
	}

	bwaio = bw_vmdio_aio_create(BW_VMDIO_ASYNC_CALLBACK, async_callback, buf);
	if (!bwaio) {
		printf("create bw_vmdio_aio_t failed.\n");
		goto out;
	}

	printf("begin to read.pid=%lu\n", pthread_self());
	bwerr = bw_vmdio_read(bwdev, 4096000, BUFSIZE, buf, bwaio);
	if (bwerr) {
		printf("read fail.%d\n", bwerr);
		goto out;
	}

	while (1)
		sleep(1);

out:
	bw_vmdio_aio_delete(bwaio);
	return ;
}

void test_libvmdio_async_wait(void)
{
	char 		  *buf;
	bw_vmdio_device_t *bwdev = NULL;
	bw_vmdio_aio_t 	  *bwaio = NULL;
	bw_vmdio_error_t   bwerr;
	struct timeval	   ts = {.tv_sec=10, .tv_usec=0,};

	buf = calloc(1, BUFSIZE);

	bwdev = bw_vmdio_open_dev("/dev/sda");
	if (!bwdev) {
		printf("bw_vmdio_open_dev failed.\n");
		goto out;
	}

	bwaio = bw_vmdio_aio_create(BW_VMDIO_ASYNC_WAIT, NULL, NULL);
	if (!bwaio) {
		printf("create bw_vmdio_aio_t failed.\n");
		goto out;
	}

	printf("begin to read\n");
	bwerr = bw_vmdio_read(bwdev, 4096000, BUFSIZE, buf, bwaio);
	if (bwerr) {
		printf("read fail.%d\n", bwerr);
		goto out;
	}

	printf("begin to wait.pid=%lu\n", pthread_self());
	bwerr = bw_vmdio_aio_wait(bwdev, bwaio, &ts);
	if (bwerr) {
		printf("wait fail.%d\n", bwerr);
		goto out;
	}
	show_result(buf, 512);

out:
	bw_vmdio_aio_delete(bwaio);
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

	test_libvmdio_sync();
	test_libvmdio_async_wait();
	test_libvmdio_async_callback();
	return 0;
}
