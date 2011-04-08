#include <stdio.h>
#include <stdlib.h>

void main(void)
{
	char str[14] = "23456789";
	int i;

	i = strtol(str,NULL,10);
	printf("%d\n",i);
}
