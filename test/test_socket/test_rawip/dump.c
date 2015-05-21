
#include <stdio.h>

#include "uip.h"
#include "utcp.h"


void
iphdr_dump(struct iphdr *iph)
{
	printf("iphdr dump:\n");
	printf("version:%u\n", iph->version);
	printf("ihl:%u\n", iph->ihl);
	printf("tos:%u\n", iph->tos);
	printf("tot_len:%u\n", iph->tot_len);
	printf("id:%u\n", iph->id);
	printf("frag_off:%u\n", iph->frag_off);
	printf("ttl:%u\n", iph->ttl);
	printf("protocol:%u\n", iph->protocol);
	printf("check:%u\n", iph->check);
	printf("saddr:%x\n", iph->saddr);
	printf("daddr:%x\n\n", iph->daddr);
}

void
tcphdr_dump(struct tcphdr *tcph)
{
	printf("tcphdr dump\n");
	printf("source:%u\n", ntohs(tcph->source));
	printf("dest:%u\n", ntohs(tcph->dest));
	printf("seq:%u\n", tcph->seq);
	printf("ack_seq:%u\n", tcph->ack_seq);
	printf("doff:%u\n", tcph->doff);
	printf("fin:%u\n", tcph->fin);
	printf("syn:%u\n", tcph->syn);
	printf("rst:%u\n", tcph->rst);
	printf("psh:%u\n", tcph->psh);
	printf("ack:%u\n", tcph->ack);
	printf("urg:%u\n\n\n\n", tcph->urg);
}
