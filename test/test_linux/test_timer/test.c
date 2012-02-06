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
void timer_thread(sigval_t val)
{
	printf("thread run, val=%d\n", val.sival_int);
	sleep(5);
	printf("thread finished\n");
}
void test_timer_create(void)
{
	struct sigevent sige;
	timer_t		timerid;
	long		ret;
	struct itimerspec tick;

	signal(SIGALRM, sig_handler);

	sige.sigev_value.sival_int	= 111;
	sige.sigev_signo		= SIGALRM;
	sige.sigev_notify		= SIGEV_THREAD;
	sige.sigev_notify_function	= timer_thread;
	sige.sigev_notify_attributes	= NULL;
	ret = timer_create(CLOCK_REALTIME, &sige, &timerid);
	if (ret) {
		printf("timer_create error:%d(%s)\n", errno, strerror(errno));
		return ;
	}

	tick.it_value.tv_sec = 3;
	tick.it_value.tv_nsec = 0;
	tick.it_interval.tv_sec = 1;
	tick.it_interval.tv_nsec = 0;
	timer_settime(timerid, 0, &tick, NULL);

	while(1) {
		pause();
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
	test_setitimer();
//	test_timer_create();

	return 0;
}
