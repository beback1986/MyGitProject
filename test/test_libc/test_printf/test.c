#include <linux/types.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	__u64 a = 1;
	__u32 b = 2;

	printf("%u, %u, %u\n", a, b);

	return 0;
}
