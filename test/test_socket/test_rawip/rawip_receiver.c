
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>



int
main(void)
{
	int fd = socket (PF_INET, SOCK_RAW, IPPROTO_TCP);
	char buffer[8192]; /* single packets are usually not bigger than 8192 bytes */
	while (read (fd, buffer, 8192) > 0)
		printf ("Caught tcp packet: %s\n", 
				buffer+sizeof(struct tcphdr));
}
