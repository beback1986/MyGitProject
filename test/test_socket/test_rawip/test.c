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

#include "task.h"
#include "transport.h"

#if 0	//backup some useful thing.
#define P 30001 /* lets flood the sendmail port */

	char datagram[4096];
	struct iphdr *iph = (struct iphdr *) datagram;
	struct tcphdr *tcph = (struct tcphdr *)(datagram + sizeof (struct iphdr));

	iph->ihl = 5;
	iph->version = 4;
	iph->tos = 0;
	iph->tot_len = sizeof (struct iphdr) + sizeof (struct tcphdr);	/* no payload */
	iph->id = htonl (54321);	/* the value doesn't matter here */
	iph->frag_off = 0;
	iph->ttl = 255;
	iph->protocol = 6;
	iph->check = 0;		 /* it will be auto filled by kernel's IP stack */
	iph->saddr = inet_addr ("1.2.3.4");	/* SYN's can be blindly spoofed */
	iph->daddr = sin.sin_addr.s_addr;

	tcph->source = htons (1234);	/* arbitrary port */
	tcph->dest = htons (P);
	tcph->seq = random ();		/* in a SYN packet, the sequence is a random */
	tcph->ack_seq = 0;	/* number, and the ack sequence is 0 in the 1st packet */
	tcph->res2 = 0;
	tcph->doff = sizeof(struct tcphdr)/4;	/* first and only tcp segment */
	tcph->syn = 1;		/* initial connection request */
	tcph->window = htonl (65535);		/* maximum allowed window size */
	tcph->check = 0;	/* set it to 0 before computing the actual checksum */
	tcph->urg_ptr = 0;

	tcph->check = tcp_csum(iph->saddr, 
				iph->daddr, 
				sizeof(struct tcphdr), 
				(unsigned short*)(datagram + sizeof(struct iphdr)));
#endif

int 
main(int argc, char *argv[])
{
	struct task *rcv_task,
		    *snd_task;

	utcp_init();
	usender_init();
	ureceiver_init();


	rcv_task = ureceiver_start();
	if (!rcv_task)
		goto rcv_task_fail;
	snd_task = usender_start();
	if (!snd_task)
		goto snd_task_fail;

	task_wait(rcv_task);
	task_wait(snd_task);

	printf("receive task return %d\n", task_ret_get(rcv_task, int));
	printf("send task return %d\n", task_ret_get(snd_task, int));

	return 0;

snd_task_fail:
	task_stop(rcv_task);
rcv_task_fail:
	return -1;
}
