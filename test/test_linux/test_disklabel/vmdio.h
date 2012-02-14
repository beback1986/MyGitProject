/*
 * Volume Metadata I/O Library
 */

#ifndef __BW_VMDIO_H__
#define __BW_VMDIO_H__

/* For struct timeval. */
#if defined BWOSL_LIN_USER
#include <time.h>
#include <stdint.h>
#elif defined BWOSL_WIN_USER
#else
#error "Unsupported platform!"
#endif


//#include "types.h"
#include "list.h"

typedef enum _bw_vmdio_error {
	BW_VMDIO_SUCCESS = 0,
	BW_VMDIO_ERROR_NOSYS,
	BW_VMDIO_ERROR_BADFD,
	BW_VMDIO_ERROR_NOMEM,
	BW_VMDIO_ERROR_INVAL,
	BW_VMDIO_ERROR_READ,
	BW_VMDIO_ERROR_AIO,
	BW_VMDIO_ERROR_WAIT,
	BW_VMDIO_ERROR_TIMEO,
	BW_VMDIO_ERROR_QUERY_SCSI,
	BW_VMDIO_ERROR_QUERY_STORAGE,
	BW_VMDIO_ERROR_QUERY_DEVICE,
	BW_VMDIO_ERROR_QUERY_DEVLIST,
} bw_vmdio_error_t;

typedef enum _bw_vmdio_iomode {
	BW_VMDIO_SYNC,
	BW_VMDIO_ASYNC_WAIT,
	BW_VMDIO_ASYNC_CALLBACK,
} bw_vmdio_iomode_t;

/* ASYNC_CALLBACK mode function type. */
typedef void (*bw_vmdio_aio_callback_t)(bw_vmdio_error_t err, uint32_t len, void *args);

#if defined BWOSL_LIN_USER
#include "vmdio-linux.h"
#elif defined BWOSL_WIN_USER
#include "vmdio-windows.h"
#else
#error "Unsupported platform!"
#endif

/* These two structure should be provided in vmdio-{platform}.h */

struct _bw_vmdio_device; /* Platform provide. */
typedef struct _bw_vmdio_device bw_vmdio_device_t;

struct _bw_vmdio_aio;	 /* Platform provide. */
typedef struct _bw_vmdio_aio bw_vmdio_aio_t;

typedef struct bw_list_head bw_vmdio_device_list_t;/* List head. */

typedef struct _bw_vmdio_device_id {
	struct bw_list_head	list;		  /* List node. */
	uint64_t		device_number;	  /* Unique number to represent device in system. */
	char *			kernel_space_name;/* Device name in kernel space. */
	char *			user_space_name;  /* Device name in user space. */
} bw_vmdio_device_id_t;

typedef struct _bw_vmdio_scsi_info {
	uint32_t 		sector_size;	/* Scsi device io block size. */
	uint64_t		capacity;	/* Scsi device capacity. */
	unsigned char	cdb_length;		/* Scsi device cdb length. */
} bw_vmdio_scsi_info_t;

typedef struct _bw_vmdio_device_info {
	char 		vendor_id[32];	/* Device vendor id. */
	char		product_id[32];	/* Device product id. */
	char		serial_num[64];	/* Device serial number. */
} bw_vmdio_device_info_t;

/*
 * Open device.
 */
extern bw_vmdio_device_t *bw_vmdio_open_dev(const char *dev);

/*
 * Close device.
 */
extern bw_vmdio_error_t bw_vmdio_close_dev(bw_vmdio_device_t *dev);

/*
 * Create bw_vmdio_aio_t.
 */
extern bw_vmdio_aio_t *bw_vmdio_aio_create(bw_vmdio_iomode_t mode, bw_vmdio_aio_callback_t callback, void *args);

/*
 * Delete bw_vmdio_aio_t, release its memory.
 */
extern void bw_vmdio_aio_delete(bw_vmdio_aio_t *aio);

/*
 * Used in device io. Provide both sync & async mode.
 *  @dev: 	Device describtor.
 *  @off:	Offset in device, where read begin.
 *  @len:	Read length.
 *  @buf:	Read buffer. Invoker provide buffer.
 *  @aio:	Use bw_vmdio_aio_create to create aio. 
 *  		If aio is NULL, SYNC mode is supposed.
 */
extern bw_vmdio_error_t bw_vmdio_read(bw_vmdio_device_t *dev, uint64_t off, uint32_t len, char *buf, bw_vmdio_aio_t *aio);

/*
 * Used to wait for aync io finish. This function only work if bw_vmdio_read is
 * work in BW_VMDIO_ASYNC mode.
 *  @dev: 	Device describtor.
 *  @aio:	The describtor bw_vmdio_read return.
 *  @timeo:	NULL means wait forever. If not NULL, given time will be wait before IO is done.
 */
extern bw_vmdio_error_t bw_vmdio_aio_wait(bw_vmdio_device_t *dev, bw_vmdio_aio_t *aio, struct timeval *timeo);

/*
 * Get device's scsi infomation, include cdb_length & sector_size.
 */
extern bw_vmdio_error_t bw_vmdio_query_scsi_info(bw_vmdio_device_t *dev, bw_vmdio_scsi_info_t *scsi_info);

/*
 * Get device name, to identify unique deivce on this system.
 * On linux, means device name. eg:sda
 * On windows, means harddisk name & physical drive name.
 */
extern bw_vmdio_error_t bw_vmdio_query_device_id(bw_vmdio_device_t *dev, bw_vmdio_device_id_t *dev_id);

/*
 * Get device's vendor id, product id & serial num.
 */
extern bw_vmdio_error_t bw_vmdio_query_device_info(bw_vmdio_device_t *dev, bw_vmdio_device_info_t *dev_info);

/*
 * Get all device connected to this system. Each device use type bw_vmdio_device_entry
 * to describe.
 */
extern bw_vmdio_error_t bw_vmdio_query_device_list(bw_vmdio_device_list_t *head);

/*
 * free device list
 */
extern void bw_vmdio_free_device_list(bw_vmdio_device_list_t *head);

#endif	/* __BW_VMDIO_H__ */
