#include <stdio.h>
#include <string.h>

struct node {
	char hello[10];
};

int
main(void)
{
	char *hello;
	int *i;
	struct node n[2];
	void *p;

	memset(n[0].hello, 0, 10);
	memset(n[1].hello, 0, 10);

	memcpy(n[0].hello, "123456", 6);
	memcpy(n[1].hello, "abcdef", 6);

	p = n;

	printf("%s\n", p);

	p +=1;
	printf("%s\n", p);

	i = p;

	return 0;
}
