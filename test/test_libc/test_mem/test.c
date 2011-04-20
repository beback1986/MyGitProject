
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
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#ifndef __USE_GNU
#define __USE_GNU
#endif
#include <sys/mman.h>

#include "error.h"

int
main(int argc, char *args[])
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
		PERROR("Can not alloc memory from valloc");
		goto failed;
	}

	printf("Allocated memory address:%p\n", buf);

	buf_new = (void *)mremap(buf, 4096, 8192, MREMAP_MAYMOVE);

	printf("Remaped memory address:%p\n", buf_new);

	for (i=0; i*sizeof(char)<page_size*300; i++) {
		p = ((char *)buf) + i;
//		printf("At addr:%p, data:%d\n", p, *p);
	}

	return 0;

failed:
	return -1;
}
