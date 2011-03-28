#include <unistd.h>
#include <stdio.h>
#include <signal.h>

void sigchld_handler (int signum)
{
	int status = 99;

	wait (&status);
	printf ("child returned with status %d\n", status);
}

int main (int argc, char *argv[])
{
	pid_t pid;

	signal (SIGCHLD, sigchld_handler);

	pid = fork();

	if (pid < 0)
		return 1;
	else if (pid == 0) {
		execv ("/usr/bin/gedit", NULL);
	}
	while (1)
		sleep (3);

	return 0;
}
