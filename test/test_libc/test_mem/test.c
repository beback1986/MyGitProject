#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>

/* For:
 * getpagesize()
 */
#include <unistd.h>

/* For:
 * valloc()
 */
#include <malloc.h>

/* For:
 * mremap()
 */
#include <sys/mman.h>

#include "error.h"

void *valoc_mem;

void
test_mremap(void)
{
	int page_size;
	void *buf;
	void *buf_new;
	char *p;
	int i;

	page_size = getpagesize();
	printf("Page size:%d.\n", page_size);

	buf = valloc(4096);
	if(!buf) {
		PERROR("Can not alloc memory from valloc.");
		goto failed;
	}
	printf("valloc memory address:\t\t%p\n", buf);

	valoc_mem = buf;

/*	buf_new = mremap(buf, 4096, 8192, MREMAP_MAYMOVE);
	if(buf_new == MAP_FAILED) {
		PERROR("Can not remap memory.");
		goto failed;
	}
	printf("Remaped memory address:\t\t%p\n", buf_new);

	for (i=0; i*sizeof(char)<page_size*300; i++) {
		p = ((char *)buf) + i;
//		printf("At addr:%p, data:%d\n", p, *p);
	}
*/
failed:
	return;
}

void
test_brk(void)
{
	void *prog_break;
	int brk_len;
	int ret;

	prog_break = sbrk(0);
	printf("Programe break address:\t\t%p\n", prog_break);

	if (prog_break > valoc_mem)
		printf("prot-break after valloc:%x\n", prog_break-valoc_mem);
	else
		printf("prot-break before valloc:%x\n", valoc_mem-prog_break);

	brk_len = 0x3000;
	ret = brk(prog_break+brk_len);
	if(ret) {
		PERROR("Can not use brk to set programe break.");
		goto failed;
	}

	prog_break = sbrk(0);
	printf("Programe break address after:\t%p\n", prog_break);

failed:
	return;
}

void
test_malloc(void)
{
	void *ptr;
	int i;

	for (i=0; i<34; i++) {
		ptr = malloc(4096);
//		printf("malloc ptr addr:%p\n", ptr);
		if (ptr > valoc_mem)
			printf("malloc after valloc:%x\n", ptr-valoc_mem);
		else
			printf("malloc before valloc:%x\n", valoc_mem-ptr);
	}

failed:
	return;
}

int
main(int argc, char *args[])
{
	test_mremap();

	test_brk();

	test_malloc();

	return 0;
}
