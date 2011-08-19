#include <stdio.h>

int main(int argc, char *argv[])
{
	char s1[10] = "aaaa";
	char s2[10] = "aaaaa";
	int ret;

	ret = strcmp(s1, s2);
	printf("compare s1:%s, s2:%s, return:%d\n", s1, s2, ret);

	return 0;
}
