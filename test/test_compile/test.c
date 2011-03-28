#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
char global_str[6] = "hello";
extern void func(char * s);

void main(int argc, char * argv[])
{
	char s[6]="hello";
/*       
        fd = open("/dev/tty1",O_WRONLY|O_NOCTTY);
	if(fd<0){
		printf("open failed\n");
		return 0;
	}
        write(fd,"hello tty2\n ",11);
*/
	func(s);
	printf("hello\n");
	printf("%x\n",s);

        return ;
}
