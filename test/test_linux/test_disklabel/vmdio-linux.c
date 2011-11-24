/*
 * Linux implemention
 */

//#include <config.h>
#define _LARGEFILE64_SOURCE
#define _GNU_SOURCE
#define _FILE_OFFSET_BITS 64
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <aio.h>
#include <string.h>
#include <syslog.h>
#include <inttypes.h>
#include "vmdio.h"

#define IO_WAIT_TIME 1

static struct aiocb64 * set_aiocb(int fd, vmdio_offset_t offset, vmdio_length_t length, void *data)
{
	static struct aiocb64 cb;

	memset(&cb, 0, sizeof(struct aiocb64));

	cb.aio_fildes = fd;
	cb.aio_buf = data;
	cb.aio_nbytes = length;
	cb.aio_offset = offset;

	return &cb;
}

static int
do_fd_io(int fd, vmdio_offset_t offset, vmdio_length_t length, void *data, int w)
{
	ssize_t 	n;
	struct aiocb64 	*cb;
	const struct timespec timeout = {
					.tv_sec = IO_WAIT_TIME,
					.tv_nsec = 0
				 };
		
	cb = set_aiocb(fd, offset, length, data);

	if(w){
		if (aio_write64(cb) == -1)
			return VMDIO_EWRITEDEV;
write_suspend:
		if (aio_suspend64(&cb, 1, &timeout)) {
			if (aio_error64(cb) == EINPROGRESS)
				goto write_suspend;
			if (aio_error64(cb) == 0)
				goto write_finish;
			else
				return VMDIO_EWRITEDEV;
		}
write_finish:
		if ((n = aio_return64(cb)) != length)
			return VMDIO_EWRITEDEV;	
	} else {
		if (aio_read64(cb) == -1)
			return VMDIO_EREADDEV;
read_suspend:
		if (aio_suspend64(&cb, 1, &timeout)){
			if (aio_error64(cb) == EINPROGRESS){
				syslog(LOG_USER | LOG_INFO, "Continue suspend for 1 second.\n");
				goto read_suspend;
			}
			if (aio_error64(cb) == 0){
				syslog(LOG_USER | LOG_INFO, "Read finish.Check return value.\n");
				goto read_finish;
			}else{
				syslog(LOG_USER | LOG_INFO, "Read device failed.\n");
				return VMDIO_EREADDEV;
			}
		}
read_finish:
		if ((n = aio_return64(cb)) != length)
			return VMDIO_EREADDEV;	
	}  

	return 0;
}

static int
verify_io_range(vmdio_offset_t offset, vmdio_length_t length, off64_t vsize)
{
	if (offset < 0)
		offset += vsize;
	if (offset < 0 || offset >= vsize || length > (vsize - offset))
		return 1;
	return 0;
}

static int
do_io(const char *volume, vmdio_offset_t offset, vmdio_length_t length, void *data, int w)
{
	int rc, fd, flags;
	off64_t vsize;

	if (!volume || !data || !length || (offset < 0 && -offset < length))
		return VMDIO_ESEEKPOINTER;

	if (w)
		flags = O_WRONLY|O_SYNC;
	else
		flags = O_RDONLY|O_DIRECT;
	fd = open(volume, flags);
	if (fd == -1)
		return VMDIO_EOPENDEV;

	vsize = lseek64(fd, 0, SEEK_END);
	if (vsize == -1) {
		rc = VMDIO_ESEEKPOINTER;
		goto err;
	}
	rc = VMDIO_ESEEKPOINTER;
	if (verify_io_range(offset, length, vsize))
		goto err;

	rc = do_fd_io(fd, offset, length, data, w);
	if (rc)
		goto err;

	if (close(fd))
		return VMDIO_ECLOSEDEV;

	return 0;

err:
	close(fd);
	return rc;
}

int
vmdio_read(const char *volume, vmdio_offset_t offset, vmdio_length_t length, void *data)
{
	return do_io(volume, offset, length, data, 0);
}

int
vmdio_write(const char *volume, vmdio_offset_t offset, vmdio_length_t length, const void *data)
{
	return do_io(volume, offset, length, (void *) data, 1);
}
