#include <stdio.h>

//#define _ASM_ALIGN      __ASM_SEL(.balign 4, .balign 8)

#ifdef CONFIG_SMP
#define LOCK_PREFIX \
		".section .smp_locks,\"a\"\n"   \
		_ASM_ALIGN "\n"                 \
		_ASM_PTR "661f\n" /* address */ \
		".previous\n"                   \
		"661:\n\tlock; "
#else
#define LOCK_PREFIX ""
#endif


typedef struct {
	volatile int counter;
} atomic_t;

#define ATOMIC_INIT(i)  { (i) }

static inline void atomic_inc(atomic_t *v)
{
	asm volatile(LOCK_PREFIX "incl %0"
			: "+m" (v->counter));
}

static inline void atomic_dec(atomic_t *v)
{
	asm volatile(LOCK_PREFIX "decl %0"
			: "+m" (v->counter));
}

#define atomic_read(v) ((v)->counter)

int main(int argc, char *argv[])
{
	atomic_t a = ATOMIC_INIT(0);

	printf("before inc:%d\n", atomic_read(&a));
	atomic_inc(&a);
	printf("after  inc:%d\n", atomic_read(&a));
	atomic_dec(&a);
	printf("after  dec:%d\n", atomic_read(&a));

	return 0;
}
