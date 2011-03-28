#include <stdio.h>
#include <stdlib.h>
void *vmefail(size_t size)
{
    fprintf(stderr, "memory alloc (%u bytes) returned NULL.\n", (unsigned)size);
    exit(EXIT_FAILURE);
    return NULL;
}
void * xrealloc (void *ptr, size_t size)
{
    register void *value;
    if (size == 0) size++;
    value = realloc (ptr, size);
    if (value == 0)
        value = vmefail(size);
    return value;
}
void * xcalloc (size_t nmemb, size_t size)
{
    register void *value;
    if (size == 0) size++;
    if (nmemb == 0) nmemb++;
    value = calloc (nmemb, size);
    if (value == 0)
        value = vmefail(size);
    return value;
}

