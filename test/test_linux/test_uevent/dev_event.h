/*
 * Deivce event handle library.
 * Support all platform.
 */

#ifndef __DEV_EVENT_H_
#define __DEV_EVENT_H_

/*
 * These should move to bwlblchk.h
 */
typedef enum _bwlc_devent_type {
	BWLC_DEV_ARRIVE = 1,		/* Device arrive event type. */
	BWLC_DEV_REMOVE,		/* Device remove event type. */
} bwlc_devent_type_t;

typedef struct _bwlc_devent_msg {
	bwlc_devent_type_t	event_type; /* Event type. */
	char*			dev_path;   /* Device path, platform depends. */
} bwlc_devent_msg_t;

extern int32_t bwlc_handle_dev_msg(bwlc_devent_msg_t*);

/*
 * These should be moved to platfome related initial file.
 */
typedef enum _bwlc_scan_set {
	BWLC_SCAN_NORMAL = 0,
	BWLC_SCAN_MULTIPATH,
	BWLC_SCAN_MAX,
} bwlc_scan_set_t;

extern int32_t bwlc_uevent_init(bwlc_scan_set_t scan_set);


#endif /* __DEV_EVENT_H_ */
