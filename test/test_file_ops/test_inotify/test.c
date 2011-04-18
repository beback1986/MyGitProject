#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/inotify.h>

#define BUF_SIZE 128

#define PERROR(message)								\
		do {								\
			printf(message);					\
			printf(" errno:%d(%s)\n", errno, strerror(errno));	\
		} while(0)

#define RETURN(message) \
		do { PERROR(message); return -1;} while (0)

int main(int argc, char *argv[])
{
	struct inotify_event *ie;
	int ify_fd;
	int wch_fd;
	int ret;
	char *buf;

	buf = calloc(1, BUF_SIZE);
	if (!buf)
		RETURN("can not allocat memory!");

	ify_fd = inotify_init();
	if (ify_fd == -1)
		RETURN("inotify init failed");

	wch_fd = inotify_add_watch(ify_fd, "/etc/digioceanfs_manager/services/test1/test1.info", IN_CLOSE_WRITE);
	if (wch_fd == -1)
		RETURN("inotify add watch failed");

	ret = read(ify_fd, buf, BUF_SIZE);
	if (ret == -1) 
		RETURN("can not read from inotify descreptor!");

	ie = (struct inotify_event *)buf;
	printf("get inotify event:\n");
	printf("wd=%d\n", 	ie->wd);
	printf("mask=%u\n", 	ie->mask);
	printf("cookie=%u\n", 	ie->cookie);
	printf("len=%u\n", 	ie->len);
	printf("name=%s\n", 	ie->name);

	inotify_rm_watch(ify_fd, wch_fd);
	close(ify_fd);

	return 0;
}
