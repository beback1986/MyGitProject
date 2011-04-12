#include <stdio.h>

#define func(a, b, i)				\
	({					\
		i=a;				\
		a=b;				\
		b=i;				\
		i=100;				\
	})

void
main(void)
{
	int a,b,i;
	int c;

	a = 10;
	b = 30;
	c = func(a, b, i);

	printf("c is:%d\n", c);
}
