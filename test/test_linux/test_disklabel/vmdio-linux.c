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
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <aio.h>

#include "vmdio.h"

#define __BW_VMDIO_MAX_TIMEO 3

bw_vmdio_device_t *bw_vmdio_open_dev(const char *dev)
{
	int fd;
	bw_vmdio_device_t *bwdev = NULL;

	fd = open(dev, O_RDONLY);
	if (fd == -1) {
		printf("BWVMDIO:Open device %s failed.%d(%s)\n", 
				dev, errno, strerror(errno));
		goto failed;
	}

	bwdev = calloc(1, sizeof(bw_vmdio_device_t));
	if (!bwdev) {
		printf("BWVMDIO:Alloc mem for bw_vmdio_device_t failed.%d(%s)\n", 
				errno, strerror(errno));
		goto failed;
	}

	bwdev->fd = fd;

failed:
	return bwdev;
}

bw_vmdio_error_t bw_vmdio_close_dev(bw_vmdio_device_t *dev)
{
	return close(dev->fd);
}

static inline void __bw_vmdio_fill_sig(struct sigevent *sige, bw_vmdio_iomode_t mode,
				void (*_func)(sigval_t), void *args)
{
	memset(sige, 0, sizeof(struct sigevent));
	if (mode == BW_VMDIO_ASYNC_CALLBACK) {
		sige->sigev_value.sival_ptr 	= args;
//		sige->sigev_signo		= SIGALRM;
		sige->sigev_notify 		= SIGEV_THREAD;
		sige->sigev_notify_function	= _func;
		sige->sigev_notify_attributes	= NULL;
	} else {
		sige->sigev_value.sival_int	= 123456;
//		sige->sigev_signo		= SIGALRM;
		sige->sigev_notify		= SIGEV_SIGNAL;
	}
}

static inline void __bw_vmdio_fill_aio(struct aiocb64 *cb, int fd, uint64_t off, uint32_t len, char *buf)
{
	cb->aio_fildes 	= fd;
	cb->aio_buf	= buf;
	/* NOTE:Convert uint32_t to size_t(aio_nbytes). Be aware of here. */
	cb->aio_nbytes	= len;
	cb->aio_offset	= off;
}

bw_vmdio_error_t __bw_vmdio_wait(const struct aiocb64 *aiocb, const struct timespec *timeo)
{
	int ret;
	bw_vmdio_error_t bwerr = BW_VMDIO_SUCCESS;

	aio_suspend64(&aiocb, 1, timeo);
	ret = aio_error64(aiocb);
	if (ret) {
		printf("BW_VMDIO:wait error:%d(%s)", ret, strerror(ret));
		bwerr = BW_VMDIO_ERROR_WAIT;
	}

	return bwerr;
}

void __bw_vmdio_gen_callback(sigval_t sigval)
{
	bw_vmdio_aio_t *aio = NULL;
	bw_vmdio_error_t bwerr = BW_VMDIO_SUCCESS;
	int ret;

	aio = (bw_vmdio_aio_t*)sigval.sival_ptr;

	ret = aio_error64(aio->aiocb);
	if (ret) {
		printf("BW_VMDIO: aio error:%d(%s)", ret, strerror(ret));
		bwerr = BW_VMDIO_ERROR_AIO;
	}
	/* NOTE:Convert size_t(aio_nbytes) to uint32_t. Be aware of here. */
	aio->callback(bwerr, aio->aiocb->aio_nbytes, aio->args);

	return ;
}

bw_vmdio_aio_t *bw_vmdio_aio_create(bw_vmdio_iomode_t mode, 
				bw_vmdio_aio_callback_t callback, void *args)
{
	bw_vmdio_aio_t *aio = NULL;
	struct aiocb64 *aiocb = NULL;

	aio = calloc(1, sizeof(bw_vmdio_aio_t));
	aiocb = calloc(1, sizeof(struct aiocb64));
	if (!aio || !aiocb) {
		printf("BW_VMDIO:Can not alloc mem for aio or aiocb\n");
		goto failed;
	}

	aio->mode 	= mode;
	aio->aiocb 	= aiocb;
	aio->callback 	= callback;
	aio->args 	= args;

	__bw_vmdio_fill_sig(&aiocb->aio_sigevent, mode, __bw_vmdio_gen_callback, aio);

	return aio;
failed:
	if (aio)
		free(aio);
	if (aiocb)
		free(aiocb);
	return NULL;
}

void bw_vmdio_aio_delete(bw_vmdio_aio_t *aio)
{
	if (aio) {
		if (aio->aiocb)
			free(aio->aiocb);
		free(aio);
	}
}

bw_vmdio_error_t bw_vmdio_read(bw_vmdio_device_t *dev, uint64_t off, uint32_t len, 
				char *buf, bw_vmdio_aio_t *aio)
{
	bw_vmdio_error_t bwerr = BW_VMDIO_SUCCESS;
	struct aiocb64 *aiocb;
	bw_vmdio_iomode_t mode;
	struct timespec timeo = {.tv_sec=__BW_VMDIO_MAX_TIMEO,.tv_nsec=0};

	/* If aio==NULL, we create a tmp aiocb to use. */
	if (aio == NULL) {
		mode = BW_VMDIO_SYNC;
		aiocb = calloc(1, sizeof(struct aiocb64));
		if (!aiocb) {
			printf("BW_VMDIO:Can not alloc mem for aiocb\n");
			bwerr = BW_VMDIO_ERROR_NOMEM;
			goto failed;
		}
		__bw_vmdio_fill_sig(&aiocb->aio_sigevent, BW_VMDIO_SYNC, NULL, NULL);
	} else {
		mode  = aio->mode;
		aiocb = aio->aiocb;
	}

	__bw_vmdio_fill_aio(aiocb, dev->fd, off, len, buf);
	if (aio_read64(aiocb)) {
		printf("BW_VMDIO:aio_read64 fail.%d(%s)\n", errno, strerror(errno));
		bwerr = BW_VMDIO_ERROR_READ;
		goto failed;
	}

	switch (mode) {
	case BW_VMDIO_SYNC:
		bwerr = __bw_vmdio_wait(aiocb, &timeo);
		free(aiocb);
		break;
	case BW_VMDIO_ASYNC_WAIT:
	case BW_VMDIO_ASYNC_CALLBACK:
		break;
	default:
		printf("BW_VMDIO:Unkown io mode %d\n", mode);
		bwerr = BW_VMDIO_ERROR_INVAL;
	};

failed:
	return bwerr;
}

bw_vmdio_error_t bw_vmdio_aio_wait(bw_vmdio_device_t *dev, bw_vmdio_aio_t *aio, 
				struct timeval *timeo)
{
	struct timespec ts = {
		.tv_sec		= timeo->tv_sec,
		.tv_nsec	= (timeo->tv_usec * 1000),
	};

	return __bw_vmdio_wait(aio->aiocb, &ts);
}
