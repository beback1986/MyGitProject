/*
 * Volume Metadata I/O Library
 */

#ifndef VMDIO_H
#define VMDIO_H

#ifdef VMDIO_LINUX
#include "vmdio-linux.h"
#else
#include "vmdio-windows.h"
#endif

enum {
		VMDIO_EOPENDEV		=	-1,
		VMDIO_EGETDEVINFO	=	-2,
		VMDIO_ESEEKPOINTER	=	-3,
		VMDIO_EREADDEV		=	-4,
		VMDIO_EWRITEDEV		=	-5,
		VMDIO_ECLOSEDEV		=	-6
};

int	vmdio_read(const char *volume, vmdio_offset_t offset, vmdio_length_t length, void *data);
int	vmdio_write(const char *volume, vmdio_offset_t offset, vmdio_length_t length, const void *data);

#endif	/* VMDIO_H */
