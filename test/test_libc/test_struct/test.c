#include <stdio.h>
#include <stdlib.h>

struct parent {
	struct son {
		int score;
	} *son;
	int count;
};

void init_parent(struct parent *p)
{
	p->son = calloc(2, sizeof(struct son));
	p->son[0].score = 80;
	p->son[1].score = 90;
	p->count = 2;
}

void show_parent(struct parent *p)
{
	int i;

	for (i=0; i<p->count; i++) {
		printf("son[%d] score=%d\n", i, p->son[i].score);
	}
}

int main(int argc, char *argv[])
{
	struct parent p = {.count=10};

	init_parent(&p);

	show_parent(&p);

	return 0;
}
