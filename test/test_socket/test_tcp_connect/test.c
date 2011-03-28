#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>

int main (int argc, char *args[]) 
{
	struct sockaddr_in addr;
	addr.sin_addr.s_addr = inet_addr("224.0.0.0");
	printf("inet_addr(224.0.0.0): %x\n", addr.sin_addr.s_addr);
	printf("htonl(0xe0000000):    %x\n", htonl(0xe0000000));
	printf("%d\n", (1 == 1));
	printf("%x\n", (addr.sin_addr.s_addr & htonl(0xf0000000)));
	printf("%d\n", (addr.sin_addr.s_addr & htonl(0xf0000000)) == htonl(0xe0000000));
}
