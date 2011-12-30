#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <pthread.h>

void *my_thread(void *args)
{
	int i = 0;
	char *p = NULL;

	while (i++ < 5) {
		sleep(1);
	}
	i = (int)p[0];
	printf("i=%d\n", i);

	return NULL;
}

struct signal_name_map {
	int signal_num;
	char *signal_name;
};

#define SIGNAL_MAP_NUM 3
struct signal_name_map signal_name_map[SIGNAL_MAP_NUM] = {
	{.signal_num=SIGKILL, .signal_name="SIGKILL",},
	{.signal_num=SIGTERM, .signal_name="SIGTERM",},
	{.signal_num=SIGSEGV, .signal_name="SIGSEGV",},
};

void my_sig_handler (int signum)
{
	int i;

	printf("enter signal handler\n");
	for (i=0; i<SIGNAL_MAP_NUM; i++) {
		if (signum == signal_name_map[i].signal_num) {
			printf ("recieve signal %s\n", signal_name_map[i].signal_name);
			return;
		}
	}
	printf("no match signal for %d\n", signum);
}

int main ()
{
	pthread_t tid;

	pthread_create(&tid, NULL, my_thread, NULL);

	signal(SIGKILL, my_sig_handler);
	signal(SIGTERM, my_sig_handler);
	signal(SIGSEGV, my_sig_handler);

//	pthread_kill(tid, SIGTERM);
	while (1) {
		sleep(1);
		printf ("sleeping \n");
	}
	return 0;
}
