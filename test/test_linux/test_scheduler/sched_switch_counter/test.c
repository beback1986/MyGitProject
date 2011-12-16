#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <sched.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#define THREAD_NUM 200
#define SWITCH_COUNT 1000000
//#define THREAD_NUM 1
//#define SWITCH_COUNT 1

long long rdtsc()
{
    __asm("rdtsc");
}

int fd_list[THREAD_NUM][2];
int p_list[THREAD_NUM][2];
pthread_t th_list[THREAD_NUM][2];
unsigned long time_list[THREAD_NUM][2];

void *thread_1(void *arg)
{
	int i, th_num = (int)arg;
	struct timeval tv;
#if 0
	char send    = 's';
	char receive;
#endif

	gettimeofday(&tv, NULL);
	time_list[th_num][0] = tv.tv_usec+tv.tv_sec*1000000;
	printf("Before Context Switch Time %llu, pid %d\n", (tv.tv_usec+tv.tv_sec*1000000), getpid());
	for (i = 0; i < SWITCH_COUNT; i++) {
		sched_yield();
#if 0
		read(fd_list[th_num][0], &receive, 1);
		write(p_list[th_num][1], &send, 1);
#endif
	}

	pthread_exit(0);
}

void *thread_2(void *arg)
{
	int i, th_num = (int)arg;
	struct timeval tv;
#if 0
	char send    = 's';
	char receive;
#endif

	for (i = 0; i < SWITCH_COUNT; i++) {
		sched_yield();
#if 0
		write(fd_list[th_num][1], &send, 1);
		read(p_list[th_num][0], &receive, 1);
#endif
	}
	gettimeofday(&tv, NULL);
	time_list[th_num][1] = tv.tv_usec+tv.tv_sec*1000000;
	printf("After Context Switch Time %llu, pid %d\n", (tv.tv_usec+tv.tv_sec*1000000), getpid());

	pthread_exit(0);
}

int main()
{
	int i;
	unsigned long time_total;
	struct timeval tv_begin, tv_end;
	struct sched_param param;
	param.sched_priority = 0;

	sched_setscheduler(getpid(), SCHED_FIFO, &param);

	memset(fd_list, '\0', THREAD_NUM*sizeof(int)*2);
	memset(p_list, '\0', THREAD_NUM*sizeof(int)*2);

	for (i=0; i<THREAD_NUM; i++) {
		pipe(fd_list[i]);
		pipe(p_list[i]);
	}

	gettimeofday(&tv_begin, NULL);
	for (i=0; i<THREAD_NUM; i++) {
		pthread_create(&th_list[i][0], NULL, thread_1, (void *)i);
		pthread_create(&th_list[i][1], NULL, thread_2, (void *)i);
	}

	for (i=0; i<THREAD_NUM; i++) {
		pthread_join(th_list[i][0], NULL);
		pthread_join(th_list[i][1], NULL);
	}
	gettimeofday(&tv_end, NULL);

	time_total = (tv_end.tv_usec+tv_end.tv_sec*1000000) - (tv_begin.tv_usec+tv_begin.tv_sec*1000000);

/*
	for (i=0; i<THREAD_NUM; i++) {
		time_total += time_list[i][1] - time_list[i][0];
	}
*/

	printf("total time %llu\n", time_total);
	printf("switch count:%llu\n", THREAD_NUM*SWITCH_COUNT*2);
	printf("thread switch time %llu\n", time_total/(THREAD_NUM*SWITCH_COUNT*2));

	return 0;
}

