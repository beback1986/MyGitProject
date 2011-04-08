
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define MAX_THREADS       50

pthread_cond_t  data_cond;
pthread_mutex_t data_loc;
int data;
int op_count;

void *hello(void *threadid)
{
	long tid;
	int tmp;
	int i;

	tid = (long)threadid;
	printf("Num:%d decrease thread start\n", tid);
	for (i = 0; i<50000; i++) {
		pthread_mutex_lock(&data_loc);
		while (data <= 50) {
			pthread_cond_wait(&data_cond, &data_loc);
		}
		tmp = data;
		tmp -= 1;
		data = tmp;
		op_count++;
//		printf("data decreased to %8d after operate %d times\n", data, op_count);
		pthread_mutex_unlock(&data_loc);
	}
	pthread_exit(NULL);
}
void *nihao(void *threadid)
{
	long tid;
	int tmp;
	int i;

	tid = (long)threadid;
	printf("Num:%d increase thread start\n", tid);
	for (i = 0; i<50050; i++) {
		pthread_mutex_lock(&data_loc);
		tmp = data;
		tmp += 1;
		data = tmp;
		op_count++;
//		printf("data increased to %8d after operate %d times\n", data, op_count);
		if (data > 50) {
			pthread_cond_broadcast(&data_cond);
		}
		pthread_mutex_unlock(&data_loc);
	}
	pthread_exit(NULL);
}

int main (int argc, char *argv[])
{
	pthread_t th_inc_id[MAX_THREADS];
	pthread_t th_dec_id[MAX_THREADS];
	int ret;
	long i;

	pthread_cond_init(&data_cond, NULL);
	pthread_mutex_init(&data_loc, NULL);
	data = 0;
	op_count = 0;

	for(i=0; i<MAX_THREADS; i++){
		ret = pthread_create(&th_inc_id[i], NULL, hello, (void *)i);
		if (ret){
			printf("ERROR; return code from pthread_create() is %d\n", ret);
			exit(-1);
		}
		ret = pthread_create(&th_dec_id[i], NULL, nihao, (void *)i);
		if (ret){
			printf("ERROR; return code from pthread_create() is %d\n", ret);
			exit(-1);
		}
	}
//	pthread_exit(NULL);
	for(i=0; i<MAX_THREADS; i++){
		pthread_join(th_inc_id[i], NULL);
		pthread_join(th_dec_id[i], NULL);
	}

	printf("data is %d\n", data);

	return -1;
}
