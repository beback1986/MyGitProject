#include <sys/file.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <limits.h>

#define OPEN_MODE (O_RDWR|O_CREAT)

int lock_unlock(char *filepath)
{
	int fd = -1, ret = -1;

	fd = open(filepath, OPEN_MODE, 0664);
	if (fd < 0) {
		printf("failed to open file %s, %d(%s)\n",
			filepath, errno, strerror(errno));
		goto out;
	}

	printf("LOCK: begin.   %s\n", filepath);
	if (flock(fd, LOCK_EX) == -1) {
		printf("failed to lock file %s, %d(%s)\n",
			filepath, errno, strerror(errno));
		goto out;
	}
	printf("LOCK: end.     %s\n", filepath);

	printf("SEELP: 30 sec. %s\n", filepath);
	sleep(30);

	printf("UNLOCK: begin. %s\n", filepath);
	if (flock(fd, LOCK_UN) == -1) {
		printf("failed to unlock file %s, %d(%s)\n",
			filepath, errno, strerror(errno));
		goto out;
	}
	printf("UNLOCK: end.   %s\n", filepath);

	ret = 0;
out:
	if (fd >= 0)
		close(fd);
	return ret;
}

struct worker_spec {
	pthread_t ws_thid;
	int       ws_id;
	int       ws_iter;
	int       ws_fail;
	int       ws_succ;
	char      ws_path[PATH_MAX];
};

void *worker(void *args)
{
	struct worker_spec *ws = args;
	int i;

	ws->ws_fail = 0;

	for (i=0; i<ws->ws_iter; i++) {
		if (lock_unlock(ws->ws_path)) {
			ws->ws_fail++;
			printf("worker %d failed %d times\n",
				ws->ws_id, ws->ws_fail);
			break;
		} else {
			ws->ws_succ++;
		}
	}

	return NULL;
}

int generator(const char *dirpath, int count, int iter)
{
	struct worker_spec *ws = NULL;
	int i, ret;

	ws = calloc(count, sizeof(struct worker_spec));
	if (!ws) {
		printf("can not alloc mem!!!\n");
		goto out;
	}

	for (i=0; i<count; i++) {
		ws[i].ws_id = i;
		ws[i].ws_iter = iter;
		snprintf(ws[i].ws_path, PATH_MAX, "%s/worker-%d", dirpath, i);
		ret = pthread_create(&ws[i].ws_thid, NULL, &worker, &ws[i]);
		if (ret) {
			printf("create %d thread failed, %d(%s)\n",
				i, ret, strerror(ret));
		}
	}

	for (i=0; i<count; i++) {
		ret = pthread_join(ws[i].ws_thid, NULL);
		if (ret) {
			printf("wait %d thread failed, %d(%s)\n",
				i, ret, strerror(ret));
		}
	}

	printf("Result: dirpath=%s, th_count=%d, iter=%d\n",
			dirpath, count, iter);
	for (i=0; i<count; i++) {
		printf("worker(%d), success(%d), fail(%d)\n",
			ws[i].ws_id, ws[i].ws_succ, ws[i].ws_fail);
	}

out:
	return 0;
}

int main(int argc, char *argv[])
{
	char dirpath[PATH_MAX] = "/mnt/";
	int count = 20;
	int iter  = 100;
	int i;

	for (i = 0; i < argc - 1; i++) {
		if (strcmp(argv[i], "-i") == 0) {
			i++;
			iter = atoi(argv[i]);
		} else if (strcmp(argv[i], "-c") == 0) {
			i++;
			count = atoi(argv[i]);
		} else if (strcmp(argv[i], "-d") == 0) {
			i++;
			snprintf(dirpath, PATH_MAX, "%s", argv[i]);
		}
	}

	generator(dirpath, count, iter);

	return 0;
}
