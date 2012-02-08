/*
 * Volume Metadata I/O Library
 */

#ifndef VMDIO_H
#define VMDIO_H

/* For system independent type. */
#include <stdint.h>

#include "list.h"

#ifdef VMDIO_LINUX
#include "vmdio-linux.h"
#else
#include "vmdio-windows.h"
#endif

/* These two structure should be provided in vmdio-{platform}.h */

struct _bwvmdio_device; /* Platform provide. */
typedef struct _bwvmdio_device bwvmdio_device_t;

struct _bwvmdio_aio; /* Platform provide. */
typedef struct _bwvmdio_aio bwvmdio_aio_t;

typedef struct bw_list_head bwvmdio_device_list_t;	/* List head. */

typedef struct _bwvmdio_device_id {
	struct bw_list_head	list;				/* List node. */
	uint64_t		device_number;		/* Unique number to represent device in system. */
	char *			kernel_space_name;	/* Device name in kernel space. */
	char *			user_space_name;	/* Device name in user space. */
} bwvmdio_device_id_t;

typedef struct _bwvmdio_scsi_info {
	uint32_t 		sector_size;	/* Scsi device io block size. */
	uint64_t		capacity;		/* Scsi device capacity. */
	unsigned char	cdb_length;		/* Scsi device cdb length. */
} bwvmdio_scsi_info_t;

typedef struct _bwvmdio_device_info {
	char 		vendor_id[32];	/* Device vendor id. */
	char		product_id[32];	/* Device product id. */
	char		serial_num[64];	/* Device serial number. */
} bwvmdio_device_info_t;

typedef enum _bwvmdio_error {
	BWVMDIO_ERROR_NOSYS,
	BWVMDIO_ERROR_BADFD,
	BWVMDIO_ERROR_INVAL,
	BWVMDIO_ERROR_READ,
	BWVMDIO_ERROR_WAIT,
	BWVMDIO_ERROR_TIMEO,
	BWVMDIO_ERROR_QUERY_SCSI,
	BWVMDIO_ERROR_QUERY_STORAGE,
	BWVMDIO_ERROR_QUERY_DEVICE,
	BWVMDIO_ERROR_QUERY_DEVLIST,
} bwvmdio_error_t;

#define BWVMDIO_SYNC	0
#define BWVMDIO_ASYNC	1

/*
 * Open device.
 */
extern bwvmdio_device_t *bwvmdio_open_dev(const char *dev);

/*
 * Close device.
 */
extern bwvmdio_error_t bwvmdio_close_dev(bwvmdio_device_t *dev);

/*
 * Used in device io. Provide both sync & async mode.
 *  @dev: 	Device describtor.
 *  @offset:	Offset in device, where read begin.
 *  @length:	Read length.
 *  @buff:	Read buffer. Invoker provide buffer.
 *  @mode:	Work mode. In ether BWVMDIO_SYNC or BWVMDIO_ASYNC.
 *  @aio:	If mode is BWVMDIO_ASYNC, this filed should provide to
 *  		describe one aio.
 */
extern bwvmdio_error_t bwvmdio_read(bwvmdio_device_t *dev, uint64_t offset, uint32_t length, char *buff, int32_t mode, bwvmdio_aio_t **aio);

/*
 * Used to wait for aync io finish. This function only work if bwvmdio_read is
 * work in BWVMDIO_ASYNC mode.
 *  @dev: 	Device describtor.
 *  @aio:	The describtor bwvmdio_read return.
 *  @timeo:	Wait time out, in nanoseconds. -1 means wait forever.
 */
extern bwvmdio_error_t bwvmdio_wait(bwvmdio_device_t *dev, bwvmdio_aio_t *aio, int64_t timeo);

/*
 * Get device's scsi infomation, include cdb_length & sector_size.
 */
extern bwvmdio_error_t bwvmdio_query_scsi_info(bwvmdio_device_t *dev, bwvmdio_scsi_info_t *scsi_info);

/*
 * Get device name, to identify unique deivce on this system.
 * On linux, means device file path. eg:/dev/sda
 * On windows, means harddisk name & physical drive name.
 */
extern bwvmdio_error_t bwvmdio_query_device_id(bwvmdio_device_t *dev, bwvmdio_device_id_t *dev_id);

/*
 * Get device's vendor id, product id & serial num.
 */
extern bwvmdio_error_t bwvmdio_query_device_info(bwvmdio_device_t *dev, bwvmdio_device_info_t *dev_info);

/*
 * Get all device connected to this system. Each device use type bwvmdio_device_entry
 * to describe.
 */
extern bwvmdio_error_t bwvmdio_query_device_list(bwvmdio_device_list_t *head);

/*
 * free device list
 */
extern void bwvmdio_free_device_list(bwvmdio_device_list_t *head);

#endif	/* VMDIO_H */
