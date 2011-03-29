#ifndef __TCP_CHECKSUM_H
#define __TCP_CHECKSUM_H

#define __force        __attribute__((force))

typedef unsigned short u16;
typedef unsigned long u32;

typedef u16 __wsum;
typedef u32 __sum16;
typedef u32 __be32;

static inline __sum16 csum_fold(__wsum sum)
{
        asm("addl %1, %0                ;\n"
            "adcl $0xffff, %0   ;\n"
            : "=r" (sum)
            : "r" ((__force u32)sum << 16),
              "0" ((__force u32)sum & 0xffff0000));
        return (__force __sum16)(~(__force u32)sum >> 16);
}

static inline __wsum csum_tcpudp_nofold(__be32 saddr, __be32 daddr,
                                        unsigned short len,
                                        unsigned short proto,
                                        __wsum sum)
{
        asm("addl %1, %0        ;\n"
            "adcl %2, %0        ;\n"
            "adcl %3, %0        ;\n"
            "adcl $0, %0        ;\n"
            : "=r" (sum)
            : "g" (daddr), "g"(saddr),
              "g" ((len + proto) << 8), "0" (sum));
        return sum;
}

/*
 * computes the checksum of the TCP/UDP pseudo-header
 * returns a 16-bit checksum, already complemented
 */
static inline __sum16 csum_tcpudp_magic(__be32 saddr, __be32 daddr,
                                        unsigned short len,
                                        unsigned short proto,
                                        __wsum sum)
{
        return csum_fold(csum_tcpudp_nofold(saddr, daddr, len, proto, sum));
}

extern u16 tcp_sum_calc(u16 len_tcp, u16 src_addr[], u16 dest_addr[], int padding, u16 buff[]);

#endif /* __TCP_CHECKSUM_H */
