/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/19/2011 07:01:50 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  dingyuan
 *        Company:  bwstor, China
 *
 * =====================================================================================
 */

#include "copy.h"

int main(int argc, char *argv[])
{
	struct copy_opts opts;

	set_verbose(1);
	copy(&opts, "test", "test2", 1);
	return 0;
}
