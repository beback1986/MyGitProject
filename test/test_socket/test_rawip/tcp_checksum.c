/*
**************************************************************************
Function: tcp_sum_calc()
**************************************************************************
Description: 
	Calculate TCP checksum
***************************************************************************
*/

#include <stdio.h>

#include "tcp_checksum.h"

//#define CSUM_WORD(__word_16) ((((__word_16)<<8)&0xFF00)+(((__word_16)>>8)&0xFF))
#define CSUM_WORD(__word_16) (__word_16)

u16 tcp_sum_calc(u16 len_tcp, u16 src_addr[],u16 dest_addr[], int padding, u16 buff[])
{
	u16 prot_tcp=6;
	u16 padd=0;
	u16 word16;
	u32 sum;	
	int i;
	
	// Find out if the length of data is even or odd number. If odd,
	// add a padding byte = 0 at the end of packet
	if (padding&1 == 1){
		padd=1;
		buff[len_tcp]=0;
	}

	//initialize sum to zero
	sum=0;

	// make 16 bit words out of every two adjacent 8 bit words and 
	// calculate the sum of all 16 bit words
	for (i=0; i<len_tcp+padd; i++){
		word16 = CSUM_WORD(buff[i]);
		sum += (unsigned long)word16;
	}

	// add the TCP pseudo header which contains:
	// the IP source and destinationn addresses,
	for (i=0; i<2; i++){
		word16 = CSUM_WORD(src_addr[i]);
		sum += word16;	
	}
	for (i=0; i<2; i++){
		word16 = CSUM_WORD(dest_addr[i]);
		sum += word16; 	
	}

	// the protocol number and the length of the TCP packet
	word16 = CSUM_WORD(htons(len_tcp));
	sum += word16;

	word16 = CSUM_WORD(htons(prot_tcp));
	sum += word16;

	// keep only the last 16 bits of the 32 bit calculated sum and add the carries
	sum = (sum & 0xFFFF)+(sum >> 16);
	printf("sum:0x%x\n", sum);
		
	// Take the one's complement of sum
	sum = ~sum;
	printf("sum after one's complement:0x%x\n", sum);

	return ((unsigned short) sum);
}

unsigned short
tcp_csum(u32 saddr, u32 daddr, u16 tcp_len, u16 *buf)
{
	unsigned short _saddr[2];
	unsigned short _daddr[2];

	_saddr[0] = (unsigned short) (saddr & 0xffff);
	_saddr[1] = (unsigned short) (saddr >> 16);

	_daddr[0] = (unsigned short) (daddr & 0xffff);
	_daddr[1] = (unsigned short) (daddr >> 16);

	printf("%x split into %x,%x\n", saddr, _saddr[0], _saddr[1]);
	return tcp_sum_calc(tcp_len, _saddr, _daddr, (tcp_len%2), buf);
}
