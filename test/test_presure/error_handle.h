#ifndef __ERROR_HANDLE_H__
#define __ERROR_HANDLE_H__

#include <stdio.h>
#include <errno.h>
#include <string.h>

#define PERROR(message)											\
		do {													\
			printf(message); 									\
			printf(" errno:%d(%s)\n", errno, strerror(errno));	\
		} while(0)

/*
 * macros to return
 */
#define NEGATIVE_OR_RETURN(value, message) \
		do {if (value > 0) { PERROR(message); return -1;}} while (0)

#define POSITIVE_OR_RETURN(value, message) \
		do {if (value < 0) { PERROR(message); return -1;}} while (0)

#define ZERO_OR_RETURN(value, message) \
		do {if (value != 0) { PERROR(message); return -1;}} while (0)

#define NONE_ZERO_OR_RETURN(value, message) \
		do {if (value == 0) { PERROR(message); return -1;}} while (0)

/*
 * macros to continue
 */
#define NEGATIVE_OR_CONTINUE(value, message) \
		do {if (value > 0) { PERROR(message); continue;}} while (0)

#define POSITIVE_OR_CONTINUE(value, message) \
		do {if (value < 0) { PERROR(message); continue;}} while (0)

#define ZERO_OR_CONTINUE(value, message) \
		do {if (value != 0) { PERROR(message); continue;}} while (0)

#define NONE_ZERO_OR_CONTINUE(value, message) \
		do {if (value == 0) { PERROR(message); continue;}} while (0)

/*
 * macros to break
 */
#define NEGATIVE_OR_BREAK(value, message) \
		do {if (value > 0) { PERROR(message); break;}} while (0)

#define POSITIVE_OR_BREAK(value, message) \
		do {if (value < 0) { PERROR(message); break;}} while (0)

#define ZERO_OR_BREAK(value, message) \
		do {if (value != 0) { PERROR(message); break;}} while (0)

#define NONE_ZERO_OR_BREAK(value, message) \
		do {if (value == 0) { PERROR(message); break;}} while (0)

#endif
