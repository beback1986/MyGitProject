#include <linux/types.h>
#include <stdio.h>

#define BUF_SIZE 10

void test_snprintf(void)
{
	char buffer[BUF_SIZE] = {0}, *p = NULL;
	int tatal = BUF_SIZE, p_len;

	int i;

	p = buffer;

	p_len = snprintf(buffer, 2, "");
#if 0
	for (i=10; i<100; i++) {
		if (tatal - (p - buffer) <= 1)
			goto out;
		p_len = snprintf(p, tatal-(p-buffer), "%d", i);
		p += p_len;
	}
#endif

out:
	printf("snprintf p_len=%d gen=%s\n", p_len, buffer);
}

int main(int argc, char *argv[])
{
	__u64 a = 0x1;
	__u32 b = 2;

	printf("%u, %u, %u\n", a, b);

	printf("%u, %u\n", sizeof(__u32), sizeof(__u64));
	a <<= 2;
	printf("a=%llu\n", a);
	printf("%04d\n", 123);

	test_snprintf();

//	printf("%*.*Lx\n", sizeof(a)*2, sizeof(a)*2, a);

	return 0;
}
