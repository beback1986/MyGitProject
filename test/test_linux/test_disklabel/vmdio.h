/*
 * Volume Metadata I/O Library
 */

#ifndef VMDIO_H
#define VMDIO_H

/* For system independent type. */
#include <stdint.h>

#ifdef VMDIO_LINUX
#include "vmdio-linux.h"
#else
#include "vmdio-windows.h"
#endif

/* These two structure should be provided in vmdio-{platform}.h */
struct _bwvmdio_device;
struct _bwvmdio_aio;

typedef struct _bwvmdio_device bwvmdio_device_t;
typedef struct _bwvmdio_aio bwvmdio_aio_t;
typedef struct _bwvmdio_device_list bwvmdio_device_list_t;
typedef struct _bwvmdio_scsi_info bwvmdio_scsi_info_t;
typedef struct _bwvmdio_device_id  bwvmdio_device_id_t;
typedef struct _bwvmdio_device_info bwvmdio_device_info_t;

#define BWVMDIO_SYNC	0
#define BWVMDIO_ASYNC	1

/*
 * Open device.
 */
bwvmdio_device_t *bwvmdio_open_dev(const char *dev);

/*
 * Close device.
 */
int32_t bwvmdio_close_dev(bwvmdio_device_t *dev);

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
int32_t bwvmdio_read(bwvmdio_device_t *dev, uint64_t offset, uint32_t length, char *buff, int32_t mode, bwvmdio_aio_t **aio);

/*
 * Used to wait for aync io finish. This function only work if bwvmdio_read is
 * work in BWVMDIO_ASYNC mode.
 *  @dev: 	Device describtor.
 *  @aio:	The describtor bwvmdio_read return.
 *  @timeo:	Wait time out, in nanoseconds. -1 means wait forever.
 */
int32_t bwvmdio_wait(bwvmdio_device_t *dev, bwvmdio_aio_t *aio, int64_t timeo);

/*
 * Get device's scsi infomation, include cdb_length & sector_size.
 */
int32_t bwvmdio_query_scsi_info(bwvmdio_device_t *dev, bwvmdio_scsi_info_t *scsi_info);

/*
 * Get device name, to identify unique deivce on this system.
 * On linux, means device file path. eg:/dev/sda
 * On windows, means harddisk name & physical drive name.
 */
int32_t bwvmdio_query_device_id(bwvmdio_device_t *dev, bwvmdio_device_id_t *dev_id);

/*
 * Get device's vendor id, product id & serial num.
 */
int32_t bwvmdio_query_device_info(bwvmdio_device_t *dev, bwvmdio_device_info_t *dev_info);

/*
 * Get all device connected to this system. Each device use type bwvmdio_device_entry
 * to describe.
 */
int32_t bwvmdio_query_device_list(bwvmdio_device_list *head);

/*
 * free device list
 */
void bwvmdio_free_device_list(bwvmdio_device_list *head);

#endif	/* VMDIO_H */
