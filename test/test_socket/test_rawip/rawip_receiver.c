
#include <stdio.h>
#include <sys/socket.h>

#include "uskbuff.h"
#include "uerror.h"
#include "ip_header.h"
#include "tcp_header.h"


int
main(void)
{
	struct usk_buff *uskb;
	struct iphdr *iph;
	struct tcphdr *tcph;

	int fd = socket (PF_INET, SOCK_RAW, IPPROTO_TCP);
	if (fd < 0) {
		PERROR("Can not create socket!");
		return -1;
	}
	unsigned char buffer[8192]; /* single packets are usually less than 8192 bytes */
	while (recv(fd, buffer, 8192, 0) > 0) {
		uskb = uskb_alloc_in();
		uskb_set_buff(uskb, buffer);
		ip_uskb_header_init(uskb);
		iph = uskb_iphdr(uskb);
		tcph = uskb_tcphdr(uskb);
		iphdr_dump(iph);
		tcphdr_dump(tcph);
	}
	close(fd);

	return 0;
}
