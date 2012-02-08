/*
 * =====================================================================================
 *
 *       Filename:  vmdio-linux.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/12/2012 01:10:03 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Dingyuan
 *        Company:  BWSTOR
 *
 * =====================================================================================
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <aio.h>
#include <errno.h>

#include "vmdio.h"

#define __BWVMDIO_MAX_TIMEO 3

bwvmdio_device_t *bwvmdio_open_dev(const char *dev)
{
	int fd;
	bwvmdio_device_t *bwdev = NULL;

	fd = open(dev, O_RDONLY);
	if (fd == -1)
		goto failed;

	bwdev = calloc(1, sizeof(bwvmdio_device_t));
	if (!bwdev)
		goto failed;

	bwdev->fd = fd;

failed:
	return bwdev;
}

bwvmdio_error_t bwvmdio_close_dev(bwvmdio_device_t *dev)
{
	return close(dev->fd);
}

static inline void __bwvmdio_fill_aio(struct aiocb64 *cb, int fd, uint64_t off, uint32_t len, char *buf)
{
	cb->aio_fildes 	= fd;
	cb->aio_buf	= buf;
	cb->aio_nbytes	= len;
	cb->aio_offset	= off;
}

static inline struct aiocb64 *__bwvmdio_get_aio(int fd, uint64_t off, uint32_t len, char *buf)
{
	struct aiocb64 *cb;

	cb = calloc(1, sizeof(struct aiocb64));
	if (!cb) {
		printf("Can not alloc mem for aiocb!\n");
		goto failed;
	}

	__bwvmdio_fill_aio(cb, fd, off, len, buf);

failed:
	return cb;
}

static inline bwvmdio_aio_t *__bwvmdio_get_bwaio(struct aiocb64 *aio)
{
	bwvmdio_aio_t *bwaio = NULL;

	bwaio = calloc(1, sizeof(bwvmdio_aio_t));
	if (!bwaio) {
		printf("Can not alloc mem for bwaio!\n");
		goto failed;
	}
	bwaio->aiocb = aio;

failed:
	return bwaio;
}

int32_t __bwvmdio_wait(const struct aiocb64 *cb, const struct timespec *timeo)
{
	aio_suspend64(&cb, 1, timeo);
	return aio_error64(cb);
}

bwvmdio_error_t bwvmdio_read(bwvmdio_device_t *dev, uint64_t off, uint32_t len, char *buf, int32_t mode, bwvmdio_aio_t **paio)
{
	int32_t ret;
	struct aiocb64 *cb;

	cb = __bwvmdio_get_aio(dev->fd, off, len, buf);

	ret = aio_read64(cb);
	if (ret) {
		printf("aio_read64 fail.\n");
		goto failed;
	}

	if (mode == BWVMDIO_ASYNC) {
		if (!paio) {
			ret = EINVAL;
			printf("provide paio parameter in BWVMDIO_ASYNC mode\n");
			goto failed;
		}
		(*paio) = __bwvmdio_get_bwaio(cb);
	} else { /* Consider other mode to be BWVMDIO_SYNC. */
		struct timespec timeo = {.tv_sec=__BWVMDIO_MAX_TIMEO,.tv_nsec=0};
		ret = __bwvmdio_wait(cb, &timeo);
	}

failed:
	return ret;
}

bwvmdio_error_t bwvmdio_wait(bwvmdio_device_t *dev, bwvmdio_aio_t *bwaio, int64_t timeo_sec)
{
	struct timespec timeo = {.tv_sec=timeo_sec,.tv_nsec=0};

	printf("begin to wait\n");
	return __bwvmdio_wait(bwaio->aiocb, &timeo);
}
