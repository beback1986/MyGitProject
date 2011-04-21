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

/* For:
 * getrlimit()
 */
#include <sys/resource.h>

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
	unsigned int brk_len;
	int ret;
	int i;

	prog_break = sbrk(0);
	printf("Programe break address:\t\t%p\n", prog_break);

	if (prog_break > valoc_mem)
		printf("prot-break after valloc:%x\n", prog_break-valoc_mem);
	else
		printf("prot-break before valloc:%x\n", valoc_mem-prog_break);

	brk_len = 0x20000000;
	for (i=0; i<6; i++) {
		ret = brk(prog_break+brk_len);
		if(ret) {
			PERROR("Can not use brk to set programe break.");
			goto failed;
		}
		printf("brked %dM memory.\n", brk_len/1024/1024);
		brk_len += 0x20000000;
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
	size_t aloc_unit_size = 0x100000;
	size_t aloc_size = 0;

	for (i=0; i<12; i++) {
		ptr = malloc(aloc_unit_size);
		if (ptr == NULL) {
			printf("Allocated %dM memory.", aloc_size/1024/1024);
			PERROR("Can not malloc memory.");
			goto failed;
		}
		memcpy(ptr, "hellohello", 10);
		aloc_size += aloc_unit_size;
/*		if (ptr > valoc_mem)
			printf("malloc after valloc:%x\n", ptr-valoc_mem);
		else
			printf("malloc before valloc:%x\n", valoc_mem-ptr);*/
	}

failed:
	return;
}

void
test_rlimit(void)
{
	struct rlimit lmt;
	if (getrlimit(RLIMIT_DATA, &lmt)) {
		PERROR("Get data segment size error.");
		goto failed;
	}
	printf("Data segment size limit cur:%d, max:%d\n", lmt.rlim_cur, lmt.rlim_max);

failed:
	return;
}

int
main(int argc, char *args[])
{
	char s[10];

//	test_mremap();

//	test_brk();

	test_malloc();

	test_rlimit();

//	scanf("%s", s);

	return 0;
}
