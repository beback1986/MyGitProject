#include <stdio.h>

int main(int argc, char *argv[])
{
	char s[100] = "/usr/src/debug/glibc-20090510T1842";
	printf("original = %s\n", s);
	printf("dirname  = %s\n", dirname(s));
	printf("original = %s\n", s);
	printf("basename = %s\n", basename(s));
	printf("original = %s\n", s);
	return 0;
}
