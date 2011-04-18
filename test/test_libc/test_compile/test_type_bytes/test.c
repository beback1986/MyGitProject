#include <stdio.h>

enum {
	NUM_1,
	NUM_2,
	NUM_3
};

int main(int argc, char *args[])
{
	printf("sizeof(int):%d\n", sizeof(int));
	printf("sizeof(long):%d\n", sizeof(long));

	printf("num1:%d\n", NUM_1);
	printf("num2:%d\n", NUM_2);
	printf("num3:%d\n", NUM_3);

	return 0;
}
