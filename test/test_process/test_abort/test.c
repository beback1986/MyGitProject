#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>

int Abort()
{
	abort();
}

int main(int argc, char *argv[])
{
	struct rlimit lim;

	getrlimit(RLIMIT_CORE, &lim);
	printf("For core size, cur limit is:%d, hard limit is:%d\n", lim.rlim_cur, lim.rlim_max);

	lim.rlim_cur = -1;
	setrlimit(RLIMIT_CORE, &lim);

	getrlimit(RLIMIT_CORE, &lim);
	printf("After setrlimit, cur limit is:%d, hard limit is:%d\n", lim.rlim_cur, lim.rlim_max);

	Abort();
}
