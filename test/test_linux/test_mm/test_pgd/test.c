#include <stdio.h>

#define PGDIR_SHIFT	30
#define PTRS_PER_PGD	4
#define pgd_index(address) (((address) >> PGDIR_SHIFT) & (PTRS_PER_PGD - 1))

int main(void)
{
	long addr = 0xc0400000;
	long index;

	index = pgd_index(addr);
	printf("index:%x\n", index);
}
