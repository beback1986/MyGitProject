#include <stdio.h>
#include <time.h>
#include <errno.h>

int main(void)
{
/*
	time_t t = 0;
	char *time_s;

	time_s = ctime(&t);
	printf("%s\n",time_s);
*/
	struct tm time_tm;
	time_t t;
	struct timeval time_now;

	time_tm.tm_sec  = 0;	/* the time time_t begin:Thu Jan 1 08:00:00 1970 */
	time_tm.tm_min  = 0;
	time_tm.tm_hour = 8;
	time_tm.tm_mday = 1;
	time_tm.tm_mon  = 0;
	time_tm.tm_year = 70;
 	time_tm.tm_wday = 0;
	time_tm.tm_yday = 0;
	time_tm.tm_isdst= 0;
	t = mktime(&time_tm);
	printf("%d\n",t);
	perror("test_time:");

	struct tm * tm_p;
	time_t tt = 100;
	tm_p = gmtime(&tt);
	printf("%d-%d-%d-%d-%d-%d\n", tm_p->tm_year, tm_p->tm_mon, tm_p->tm_mday, tm_p->tm_hour, tm_p->tm_min, tm_p->tm_sec);
	time_t time_now = time(NULL);
	printf("%x\n",time_now);
}
