#define __USE_GNU
#include <stdio.h>
#include <sched.h>
#include <linux/sched.h>

int main(int argc, char *argv[])
{
	int policy;
	char *word;

	policy = sched_getscheduler(0);

	switch (policy) {
	case SCHED_OTHER: 	word = "SCHED_OTHER";break;
	case SCHED_BATCH: 	word = "SCHED_BATCH";break;
	case SCHED_IDLE: 	word = "SCHED_IDLE";break;
	case SCHED_FIFO: 	word = "SCHED_FIFO";break;
	case SCHED_RR: 		word = "SCHED_RR";break;
	default : 		word = "UNKNOW";
	}

	printf("policy is %s\n", word);

	return 0;
}
