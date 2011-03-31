
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

#include "uerror.h"

int
main(void)
{
	int fd = socket (PF_INET, SOCK_RAW, IPPROTO_TCP);
	if (fd < 0) {
		PERROR("Can not create socket!");
		return -1;
	}
	char buffer[8192]; /* single packets are usually not bigger than 8192 bytes */
	while (recv(fd, buffer, 8192, 0) > 0)
		printf ("Caught tcp packet: %s\n", buffer);
	close(fd);

	return 0;
}
