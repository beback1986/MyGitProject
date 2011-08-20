#include <stdio.h>

int main(int argc, char *argv[])
{
	char s[100] = "/usr/src/debug/glibc-20090510T1842";
	printf("%s", dirname(s));
	return 0;
}
