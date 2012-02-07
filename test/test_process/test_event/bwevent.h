/*
 * =====================================================================================
 *
 *       Filename:  bwevent.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/07/2012 10:09:57 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  DingYuan
 *        Company:  BWSTOR
 *
 * =====================================================================================
 */

#ifndef __BWEVENT_H__
#define __BWEVENT_H__

#define LINUX

#ifdef WINDOWS
#include "bwtimer_win.h"
#elif LINUX
#include "bwtimer_lin.h"
#else
#error "Unkown platform!!!"
#endif

enum {
	BWEVENT_ERROR_BADFD = 1;
	BWEVENT_ERROR_SET = 1;
	BWEVENT_ERROR_WAIT = 1;
};

bwevent_create();

bwevent_close();

bwevent_set();

bwevent_wait();


#endif /* __BWEVENT_H__ */
