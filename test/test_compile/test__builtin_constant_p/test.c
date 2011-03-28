#include <stdio.h>

int test_constant(int arg)
{
	if(__builtin_constant_p(arg))
		printf("it's a constant\n");
	else
		printf("it's not a constant\n");
	return 0;
}

int main(void)
{
	int i = 2;
	test_constant(i);
}
