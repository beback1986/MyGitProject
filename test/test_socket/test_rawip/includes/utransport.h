/*
* =====================================================================================
*
*       Filename:  utransport.h
*
*    Description:  interfaces to actual handle transport.
*
*        Version:  1.0
*        Created:  04/14/2011 10:48:49 AM
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  Beback
*        Company:  Xidian
*
* =====================================================================================
*/

#ifndef __UTRANSPORT_H
#define __UTRANSPORT_H

/* Interfaces for sender. */
extern struct task *
usender_start(void);

extern void
usender_init(void);

extern int
usender_queue(struct usk_buff *uskb)

/* Interfaces for receiver. */
#define RCV_BUFFER_SIZE 8192 /* Single packets are usually less than 8192 bytes. */
#define RCV_BUFFER_COUNT 1024

extern struct task *
ureceiver_start(void);

extern void
ureceiver_init(void);

#endif /* __UTRANSPORT_H */
