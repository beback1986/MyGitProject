#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <aio.h>

void set_aiocb(struct aiocb64 *cb, int fd, int64_t offset, uint32_t length, char *buf)
{
	memset(cb, 0, sizeof(struct aiocb64));

	cb->aio_fildes 	= fd;
	cb->aio_buf 	= buf;
	cb->aio_nbytes 	= length;
	cb->aio_offset 	= offset;
}

int do_aio(int fd, int64_t offset,  int32_t length, char *buf)
{
	int ret;
	struct aiocb64 cb, *pcb;
	const struct timespec timeo = {.tv_sec=3,.tv_nsec=0};

	set_aiocb(&cb, fd, offset, length, buf);

	ret = aio_read64(&cb);
	if (ret) {
		printf("aio_read64 failed.%d(%s)\n", errno, strerror(errno));
		goto failed;
	}

wait:
	pcb = &cb;
	aio_suspend64(&pcb, 1, &timeo);
	ret = aio_error64(&cb);
	if (ret == EAGAIN) {
		printf("Continue to suspend for 1 time.\n");
		goto wait;
	} else if (ret == EINPROGRESS) {
		printf("Continue to suspend for 1 time.\n");
		goto wait;
	} else if (ret == 0) {
		printf("Read finished.\n");
	} else {
		printf("Read failed.%d(%s)\n", errno, strerror(errno));
		goto failed;
	}

failed:
	return ret;
}

int main(int argc, char *argv[])
{
	int fd, ret, i, j;
	char buff[512] = {0};

	fd = open("/dev/sda", O_RDONLY);

	ret = do_aio(fd, 4096000, 512, buff);
	if (ret)
		goto failed;
	for (i=0; i<64; i++) {
		for (j=0; j<8; j++) {
			printf("%x ", buff[i*8+j]);
		}
		printf("\n");
	}

failed:
	close(fd);

	return 0;
}

