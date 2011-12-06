#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	unsigned int seed = 1;
	int i;

	for (i=0; i<10; i++) {
		printf("rand=%d\n", rand_r(&seed)%1000000);
	}

	return 0;
}
