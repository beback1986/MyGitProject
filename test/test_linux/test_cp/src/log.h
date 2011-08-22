/*
 * =====================================================================================
 *
 *       Filename:  log.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/19/2011 07:28:32 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  dingyuan
 *        Company:  bwstor, China
 *
 * =====================================================================================
 */

#ifndef __LOG_H_
#define __LOG_H_

/* Set 1 enable verbose mode, set 0 to disable verbose mode. */
void set_verbose(int key);
/* Print messages while in verbose mode. */
int vprint(const char *format, ...);
/* Print error messages. */
int eprint(const char *format, ...);

#endif /* __LOG_H_ */
