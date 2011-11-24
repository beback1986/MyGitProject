#define _GNU_SOURCE
#define _FILE_OFFSET_BITS 64
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "vmdio.h"

#define VOLUME	"/dev/sdc"
#define BUFSIZE (4096)

int readdev(const char *volume, vmdio_offset_t offset, vmdio_length_t length, void *data)
{
        int rc, fd, flags;

        fd = open(volume, O_RDONLY);
        if (fd == -1)
                return VMDIO_EOPENDEV;

        llseek(fd, offset, SEEK_SET);
        rc = read(fd, data, length);
        if (rc <= 0)
                goto err;

        close(fd); return 0;

err:
        close(fd);
        return rc;
}

int main(int argc, char *argv[])
{
	char *buf;
	char path[10] = VOLUME;
	int ret, i, c, *p;

	buf = calloc(1, BUFSIZE);
	if (!buf)
		goto failed;

	ret = readdev(path, 31*4096, BUFSIZE, buf);
	if (ret < 0) {
		printf("return ret=%d\n", ret);
		goto failed;
	}

	printf("return ret=%d\n", ret);
	p = (int *)buf;
	c = 0;
	while (c < BUFSIZE) {
		printf("%x ", *p);
		p++;
		c += sizeof(*p);
		if (c%256 == 0)
			printf("\n");
	}
	return 0;

failed:
	printf("errno=%d(%s)\n", errno, strerror(errno));
	return 0;
}
