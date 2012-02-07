/*
 * =====================================================================================
 *
 *       Filename:  test.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/07/2012 04:42:17 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  DingYuan
 *        Company:  XIDIAN
 *
 * =====================================================================================
 */

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/file.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <libgen.h>
#include <unistd.h>

#define PID_FILE "/var/log/test.pid"

int gen_dir(char *path)
{
	int ret = 0;

	ret = mkdir(path, 0755);

	if (ret && errno==EEXIST) {
		ret = 0;
	} else if (ret) {
		printf("ret=%d,errno=%d(%s)\n", ret, errno, strerror(errno));
	}

	return ret;
}

int gen_parent(char *path)
{
	char *dirpath;

	dirpath = dirname(strdup(path));

	return gen_dir(dirpath);
}

int lock_pidfile(int fd, uint32_t timeo)
{
	int ret = -1;
	int lock_mode = LOCK_EX;

	if (timeo != 0)
		lock_mode |= LOCK_NB;

	while (ret) {
		ret = flock(fd, lock_mode);
		if (ret == 0)
			break;
		if (errno != EAGAIN)
			break;
		if (timeo == 0)
			break;
		sleep(1);
		timeo--;
	}

	return ret;
}

int unlock_pidfile(int fd)
{
	int ret = 0;

	ret = flock(fd, LOCK_UN);
	if (ret) {
		printf("unlock error.%d(%s)\n", errno, strerror(errno));
		goto out;
	}

out:
	return ret;
}

#define GET_RIGHT_WAIT_TIME 10

int get_right(char *pid_path)
{
	int ret, fd;

	ret = gen_parent(pid_path);
	if (ret) {
		printf("gen_parent error\n");
		goto fail;
	}

	fd = open(pid_path, O_RDWR|O_CREAT, 0644);
	if (fd < 0) {
		printf("open error, %d(%s)\n", errno, strerror(errno));
		ret = errno;
		goto fail;
	}

	ret = lock_pidfile(fd, GET_RIGHT_WAIT_TIME);
	if (ret) {
		printf("lock_pidfile error\n");
		goto fail;
	}

	return fd;
fail:
	return -1;
}

int put_right(int fd)
{
	return unlock_pidfile(fd);
}

int main(int argc, char *argv[])
{
	int fd;

	fd = get_right(PID_FILE);
	if (fd < 0)
		printf("get right fail\n");

	sleep(10);

	put_right(fd);

	return 0;
}
