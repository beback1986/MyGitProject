#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

void my_sigint_handle (int signum)
{
	printf ("recieve SIGINT %d\n", signum);
	exit (0);
}

int main ()
{
	signal(SIGINT, my_sigint_handle);
	while (1) {
//		printf ("sleeping \n");
		sleep (5);
	}
	return 0;
}
