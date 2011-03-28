#include <stdio.h>

void func1()
{
}
void func2()
{
	int register i = 0;
	int j = 0;
//	int k = 0;
//	int l = 0;
//	int m = 0;
	j++;
	i=j;
//	k--;
}
void func3(int i, int j)
{
	int k;
	i++;
	j++;
	k++;
}

int main(int argc, char* argv[])
{
	int j=0,k=0,l=0;
	int register i=0;
	func1();
	func2();
	func3(i,j);
//	printf("hello");
//	int i=0;
//	i ++;
//	return 0;
}
