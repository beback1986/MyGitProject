/*
 * =====================================================================================
 *
 *       Filename:  vmdio-linux.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/12/2012 01:10:03 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Dingyuan
 *        Company:  BWSTOR
 *
 * =====================================================================================
 */

#ifndef __VMDIO_LINUX_H__

struct _bwvmdio_device {
	int fd;
	char *dev_path;
	char *dev_name;//This field is the last segmant of dev_path.
};

struct _bwvmdio_aio {
	struct aiocb64	*aiocb;
};

#endif /* __VMDIO_LINUX_H__ */
