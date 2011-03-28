#include <stdio.h>
#include <fcntl.h>
void mysolib(char * str)
{
	int fd;
	fd = open("/home/beback/test_fs", O_RDWR);
	printf("mysolib:%s\n",str);
	write(fd,"hello",5);
	close(fd);
	return ;
}

int hello_mysolib()
{
	return 32766;
} 
