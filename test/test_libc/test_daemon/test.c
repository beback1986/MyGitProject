#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#define PERROR(message)								\
		do {								\
			printf(message);					\
			printf(" errno:%d(%s)\n", errno, strerror(errno));	\
		} while(0)

int main(int argc, char *argv[])
{
	int ret;

	ret = daemon(0, 0);
	if (ret != 0) {
		PERROR("Can not run as daemon!");
		return -1;
	}

	while (1) {
		sleep(2);
		printf("hello");
	}

	return 0;
}
