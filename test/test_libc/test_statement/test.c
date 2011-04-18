#include <stdio.h>

int is_zero(int i)
{
	printf("get %d\n", i);
	return i == 0;
}

int main(int argc, char *argv[])
{
	if (0 == 0 || is_zero(1))
		printf("hello\n");

	return 0;
}
