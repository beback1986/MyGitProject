#include <stdio.h>
#include <string.h>

struct node {
	char hello[10];
};

void test_long(void)
{
	long l;
	void *p;
	unsigned long ul;

	l = -1000;
	ul = l;
	printf("ul=%lx\n", ul);
	p = (void *)ul;
	printf("p=%p\n", p);
}

int
main(void)
{
	char *hello;
	int *i;
	struct node n[2];
	void *p;

	strcpy(n[0].hello, "123456");
	strcpy(n[1].hello, "abcdef");

	p = n;

	printf("%s\n", p);

	p +=1;
	printf("%s\n", p);

	i = p;

	test_long();

	return 0;
}
