#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <fcntl.h>
#define NR_call_ding 325

char *file_path = NULL;
int main(int argc, char *argv[])
{
	long a = 4;
	int fd, mode = 0;
	loff_t off = 1048576;
	loff_t len = 4096;

	if (argc < 2) {
		printf("provide file path\n");
		return -1;
	}

	file_path = strdup(argv[1]);

	fd = open(file_path, O_RDWR|O_CREAT, 0644);
	if (fd < 0) {
		printf("Can not open file.(%d)%s\n", errno, strerror(errno));
		goto out;
	}

	syscall(285, fd, mode, off, len);

//	printf("%d\n",syscall(NR_call_ding,a));
	a = getpid();
	printf("pid:%d\n",a);

out:
	return 0;
}
