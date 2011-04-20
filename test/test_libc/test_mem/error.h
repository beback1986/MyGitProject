/*
 * =====================================================================================
 *
 *       Filename:  uerror.h
 *
 *    Description:  macros to handle error.
 *
 *        Version:  1.0
 *        Created:  03/31/2011 10:05:36 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Beback
 *        Company:  Xidian
 *
 * =====================================================================================
 */

#ifndef __ERROR_H
#define __ERROR_H

#include <errno.h>
#include <string.h>
#include <stdio.h>

#define PERROR(message)                                         \
        do {                                                    \
            printf(message);                                    \
            printf(" (%d:%s)\n", errno, strerror(errno));  \
        } while(0)

#endif /* __ERROR_H */
