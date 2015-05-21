#include <stdlib.h>
#include <stdio.h>

void set_pp(int **pp1, int **pp2)
{
	*pp1 = *pp2;
}

void func(int **pp)
{
	int *p = *pp;

	printf("p=%p\n", p);

	p++;
}

int main(int argc, char* argv[])
{
	int *p = NULL;
	int i;

	p = calloc(10, sizeof(int));
	printf("p=%p address of p %p\n", p, &p);
	func(&p);
	printf("p=%p address of p %p\n", p, &p);

	for (i = 0; i != 1; i++)
		printf("i=%d\n", i);

	return 0;
}
