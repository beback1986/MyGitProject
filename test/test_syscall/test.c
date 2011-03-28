#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <fcntl.h>
#define NR_call_ding 325
int main(void)
{
	long a = 4;
//	printf("%d\n",syscall(NR_call_ding,a));
	a = getpid();
	printf("pid:%d\n",a);
	return 0;
}
