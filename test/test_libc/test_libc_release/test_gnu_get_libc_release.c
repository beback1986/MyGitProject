#include <stdio.h>
#include <gnu/libc-version.h>

void main(void)
{
	char * release = NULL;
	release = gnu_get_libc_release();
	printf("%s\n",release);
}
