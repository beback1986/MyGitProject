/*
 * Test for timer related system interfaces(NOT glibc interfaces).
 * TODO: finish this.
 */
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>

void sig_handler(int signum)
{
	struct timeval tv;
	struct timezone	tz;

	gettimeofday(&tv, &tz);
	printf("recieve msg:%d, at:%d\n", signum, tv.tv_sec);
}

/**************************************************************
 * for timer_create() related interfaces.
 **************************************************************/
struct timer_struct {
	int 			priv;
	timer_t 		timer_id;
	struct itimerspec 	tick;
} g_ts = {.priv = 111};

void timer_thread(sigval_t val)
{
	struct timer_struct *ts = val.sival_ptr;
	printf("thread run, val=%d\n", ts->priv);
	sleep(5);
	printf("thread finished\n");
}
void test_timer_create(void)
{
	struct sigevent sige;
	struct itimerspec tspec;
	long		ret;
	int 		count = 0;

	signal(SIGALRM, sig_handler);

	sige.sigev_value.sival_ptr	= &g_ts;
	sige.sigev_signo		= SIGALRM;
	sige.sigev_notify		= SIGEV_THREAD;
	sige.sigev_notify_function	= timer_thread;
	sige.sigev_notify_attributes	= NULL;
	ret = timer_create(CLOCK_REALTIME, &sige, &g_ts.timer_id);
	if (ret) {
		printf("timer_create error:%d(%s)\n", errno, strerror(errno));
		return ;
	}

	g_ts.tick.it_value.tv_sec = 0;
	g_ts.tick.it_value.tv_nsec = 0;
	g_ts.tick.it_interval.tv_sec = 0;
	g_ts.tick.it_interval.tv_nsec = 0;
	timer_settime(g_ts.timer_id, 0, &g_ts.tick, NULL);

	while(1) {
		timer_gettime(g_ts.timer_id, &tspec);
		printf("still it_value=%d.%d it_interval=%d.%d seconds.\n",
				tspec.it_value.tv_sec,
				tspec.it_value.tv_nsec,
				tspec.it_interval.tv_sec,
				tspec.it_interval.tv_nsec);
		if (tspec.it_value.tv_sec < 2 && count < 10) {
			tspec.it_value.tv_sec = 4;
			timer_settime(g_ts.timer_id, 0, &tspec, NULL);
			count ++;
		}
		sleep(1);
	}
}

/**************************************************************
 * for setitimer() related interfaces.
 **************************************************************/
void test_setitimer(void)
{
	int ret;
	struct itimerval tick1;
	struct itimerval tick2;

	tick1.it_value.tv_sec = 3;
	tick1.it_value.tv_usec = 0;
	tick1.it_interval.tv_sec = 3;
	tick1.it_interval.tv_usec = 0;

	tick2.it_value.tv_sec = 3;
	tick2.it_value.tv_usec = 0;
	tick2.it_interval.tv_sec = 5;
	tick2.it_interval.tv_usec = 0;

	signal(SIGALRM, sig_handler);
	ret = setitimer(ITIMER_REAL, &tick1, NULL);
	if (ret) {
		printf("set timer failed\n");
		return;
	}

	ret = setitimer(ITIMER_REAL, &tick2, NULL);
	if (ret) {
		printf("set timer failed\n");
		return;
	}

	while(1) {
		pause();
	}
}

/**************************************************************
 * for alarm() related interfaces.
 **************************************************************/
void test_alarm()
{
}

/**************************************************************
 * for nanosleep() related interfaces.
 **************************************************************/
void test_nanosleep()
{
}

int main(int argc, char *argv[])
{
//	test_setitimer();
	test_timer_create();

	return 0;
}
