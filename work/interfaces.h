
===================================<<event>>===================================

deps:
uint32_t, BOOL

type:
bwoss_event_t

interface:
bwoss_event_t bwoss_event_create(BOOL manual_reset, BOOL initial_state);

interface:
BOOL bwoss_event_set(bwoss_event_t handle);

interface:
BOOL bwoss_event_close(bwoss_event_t handle);

interface:
static uint32_t bwoss_event_wait(bwoss_event_t m, uint32_t timeout);

interface:
static void bwoss_event_reset(bwoss_event_t m);



===================================<<timer>>===================================

deps:

type:
bwoss_timer_t

interface:
bwoss_timer_t bwoss_create_timer();



===================================<<uevent>>===================================



===================================<<vmdio>>===================================

deps:

typedef bwvmdio_device_t
typedef bwvmdio_aio_t
typedef bwvmdio_device_list_t /* The list node is device_id. */
typedef bwvmdio_scsi_info_t
typedef bwvmdio_device_id_t
typedef bwvmdio_device_info_t

#define BWVMDIO_SYNC	0
#define BWVMDIO_ASYNC	1

/*
 * Open device.
 */
int32_t bwvmdio_open_dev(char *dev_name, int32_t flags, struct bwvmdio_device_t **dev);

/*
 * Close device.
 */
int32_t bwvmdio_close_dev(struct bwvmdio_device_t *dev);

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
int32_t bwvmdio_query_device_list(bwvmdio_device_list_t *head);

/*
 * free device list
 */
void bwvmdio_free_device_list(bwvmdio_device_list_t *head);
