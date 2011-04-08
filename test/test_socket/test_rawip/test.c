/*
 * =====================================================================================
 *
 *       Filename:  test.c
 *
 *    Description:  test program for raw ip
 *
 *        Version:  1.0
 *        Created:  03/22/2011 09:38:18 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Beback
 *        Company:  Xidian
 *
 * =====================================================================================
 */


#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#include "uip.h"
#include "utcp.h"
#include "types.h"
#include "uerror.h"

#define P 30001 /* lets flood the sendmail port */

#define SEND_COUNT 3

u16		/* this function generates header checksums */
ip_csum (u16 *buf, int nwords)
{
	unsigned long sum;
	for (sum = 0; nwords > 0; nwords--)
		sum += *buf++;
	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	return ~sum;
}

int 
main(int argc, char *argv[])
{
	int s;
	int i;
	char datagram[4096];	/* this buffer will contain ip header, tcp header,
				   and payload. we'll point an ip header structure
				   at its beginning, and a tcp header structure after
				   that to write the header values into it */
	struct iphdr *iph = (struct iphdr *) datagram;
	struct tcphdr *tcph = (struct tcphdr *)(datagram + sizeof (struct iphdr));
	struct sockaddr_in sin;
			/* the sockaddr_in containing the dest. address is used
			 * in sendto() to determine the datagrams path */

	sin.sin_family = AF_INET;
	sin.sin_port = htons (P);/* you byte-order >1byte header values to network
				  * byte order (not needed on big endian machines) */
	sin.sin_addr.s_addr = inet_addr ("192.168.122.3");

	memset (datagram, 0, 4096);

	iph->ihl = 5;
	iph->version = 4;
	iph->tos = 0;
	iph->tot_len = sizeof (struct iphdr) + sizeof (struct tcphdr);	/* no payload */
	iph->id = htonl (54321);	/* the value doesn't matter here */
	iph->frag_off = 0;
	iph->ttl = 255;
	iph->protocol = 6;
	iph->check = 0; /* it will be auto filled by kernel's IP stack */
	iph->saddr = inet_addr ("1.2.3.4");/* SYN's can be blindly spoofed */
	iph->daddr = sin.sin_addr.s_addr;

	tcph->source = htons (1234);	/* arbitrary port */
	tcph->dest = htons (P);
	tcph->seq = random ();/* in a SYN packet, the sequence is a random */
	tcph->ack_seq = 0;/* number, and the ack sequence is 0 in the 1st packet */
	tcph->res2 = 0;
	tcph->doff = sizeof(struct tcphdr)/4;	/* first and only tcp segment */
	tcph->syn = 1;		/* initial connection request */
	tcph->window = htonl (65535);		/* maximum allowed window size */
	tcph->check = 0;/* set it to 0 before computing the actual checksum later */
	tcph->urg_ptr = 0;

//	iph->ip_sum = ip_csum((unsigned short *) datagram, iph->ip_len >> 1);

	tcph->check = tcp_csum(iph->saddr, 
				iph->daddr, 
				sizeof(struct tcphdr), 
				(unsigned short*)(datagram + sizeof(struct iphdr)));

	s = socket(PF_INET, SOCK_RAW, IPPROTO_TCP);	/* open raw socket */
	if (s < 0) {
		PERROR("can not open socket");
		return -1;
	}

/* finally, it is very advisable to do a IP_HDRINCL call, to make sure
 * that the kernel knows the header is included in the data, and doesn't
 * insert its own header into the packet before our data */

	{				/* lets do it the ugly way.. */
		int one = 1;
		const int *val = &one;
		if(setsockopt (s, IPPROTO_IP, IP_HDRINCL, val, sizeof (one)) < 0){
			PERROR("Warning: Cannot set HDRINCL!");
			return -1;
		}
	}

	for (i=0; i<SEND_COUNT; i++) {
		if(sendto(s,		/* our socket */
			  datagram,	/* the buffer containing headers and data */
			  iph->tot_len,	/* total length of our datagram */
			  0,		/* routing flags, normally always 0 */
			  (struct sockaddr *) &sin,	/* socket addr, just like in */
			  sizeof (sin)) < 0)		/* a normal send() */
			PERROR("can not send message");
		else
			printf (".");
	}

	return 0;
}
