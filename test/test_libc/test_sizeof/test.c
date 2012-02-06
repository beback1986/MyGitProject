/*
 * =====================================================================================
 *
 *       Filename:  test.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/17/2010 10:45:46 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Dr. Fritz Mehner (mn), mehner@fh-swf.de
 *        Company:  FH Südwestfalen, Iserlohn
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>

int main(int argc, char *argv[])
{
	u_int64_t u64 = 0;
	u_int32_t u32 = 0;
	double    flt = 0;
	struct timeval tv;
	u_int64_t tio = 0;
	u_int64_t tin = 0;
	u_int32_t tiv = 0;
	float     spd = 0;

	u64 = ~u64;
	u32 = ~u32;

	printf("uint64:%llu\n", u64);
	printf("uint32:%lu\n", u32);

	printf("sizeof int:		%d\n", sizeof(int));
	printf("sizeof long int:	%d\n", sizeof(long int));
	printf("sizeof long long int:	%d\n", sizeof(long long int));
	printf("sizeof long:		%d\n", sizeof(long));
	printf("sizeof long long:	%d\n", sizeof(long long));
	printf("sizeof double:%d\n", sizeof(flt));
	flt = u32*1000.0;
	printf("double:%f int:%llu\n", flt, (u_int64_t)flt);

	printf("sizeof long int:%d\n", sizeof(long int));
	printf("sizeof long long int:%d\n", sizeof(long long int));
	printf("sizeof unsigned long:%d\n", sizeof(unsigned long));
	printf("sizeof uint64_t:%d\n", sizeof(u_int64_t));

	printf("sizeof tv.tv_sec:%d\n", sizeof(tv.tv_sec));
	printf("sizeof tv.tv_usec:%d\n", sizeof(tv.tv_usec));
	gettimeofday(&tv, NULL);
	printf("old time sec:%d, usec:%d\n", tv.tv_sec, tv.tv_usec);
	tio = (u_int64_t)tv.tv_sec*1000000+(u_int64_t)tv.tv_usec;
	printf("old time:%llu\n", tio);

	sleep(1);

	gettimeofday(&tv, NULL);
	printf("new time sec:%d, usec:%d\n", tv.tv_sec, tv.tv_usec);
	tin = (u_int64_t)tv.tv_sec*1000000+(u_int64_t)tv.tv_usec;
	printf("new time:%llu\n", tin);

	tiv = tin - tio;
	spd = 1048576*1.0/tiv;
	printf("time interval:%u\n", tiv);
	printf("speed:%f\n", spd);
	return 0;
}
