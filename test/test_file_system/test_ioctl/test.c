#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/fs.h>

void main(void)
{
	int fd;
	int arg = 0;

	fd = open("test.c", O_RDWR);
	ioctl(fd, FS_IOC_GETFLAGS, &arg);
	printf("%x\n",arg);
	close(fd);
}
