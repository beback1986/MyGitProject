#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <glob.h>

void test_glob(void)
{
	glob_t globbuf = {
		.gl_pathc = 0,
		.gl_pathv = NULL,
		.gl_offs = 0};

	glob("/dev/sd*", 0, NULL, &globbuf);
	globfree(&globbuf);
}

struct test_s {
	char **list;
	int count;
};

int main(int argc, char *argv[])
{
	int i;
	struct test_s ttt = {0};

	ttt.list = calloc(10, sizeof(char **));
	ttt.count = 3;
	ttt.list[0] = strdup("hello0");
	ttt.list[1] = strdup("hello1");
	ttt.list[2] = strdup("hello2");

	for (i=0; i<ttt.count; i++) {
		printf("%s\n", ttt.list[i]);
	}

	test_glob();
	return 0;
}
