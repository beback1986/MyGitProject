/*
 * =====================================================================================
 *
 *       Filename:  log.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/22/2011 08:43:07 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  dingyuan
 *        Company:  bwstor, China
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdarg.h>

#include "log.h"

static int verbose = 0;

void set_verbose(int key)
{
	if (key)
		verbose = 1;
}
int vprint(const char *format, ...)
{
	va_list arg; 
	int done = 0; 

	if (verbose) {
		va_start(arg, format); 
		done = printf(format, arg); 
		va_end(arg); 
	}

	return done; 	
}
int eprint(const char *format, ...)
{
	va_list arg; 
	int done = 0; 

	va_start(arg, format); 
	done = fprintf(stderr, format, arg); 
	va_end(arg); 

	return done; 	
}
