#include <stdio.h>

void func1()
{
	printf("normal: %s in file:%s:%d called\n", __func__, __FILE__, __LINE__);
}
void inline func2()
{
	printf("inline: %s in file:%s:%d called\n", __func__, __FILE__, __LINE__);
}

#define func3()  \
	do {  \
		printf("inline: %s in file:%s:%d called\n", __func__, __FILE__, __LINE__);  \
	} while (0);

int main (int argc, char *argv[])
{
	func1();
	func2();
	func3();
	return 0;
}
