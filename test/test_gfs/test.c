#include <stdio.h>

#include "lic.h"

int main(int argc, char *argv[])
{
	char tmp[16];
	int ret = 0;

	ret = lic_init();
	if (ret) {
		printf("lic initiate failed.\n");
		goto out;
	}

	ret = lic_check();
	if (ret) {
		printf("lic check failed.\n");
		goto out;
	}

	scanf("%s", tmp);

out:
	return ret;
}
