#include <sys/types.h>

#define FUSE_KERNEL_VERSION 7

#define FUSE_KERNEL_MINOR_VERSION 13

/**
 *  * INIT request/reply flags
 *   *
 *    * FUSE_EXPORT_SUPPORT: filesystem handles lookups of "." and ".."
 *     * FUSE_DONT_MASK: don't apply umask to file mode on create operations
 *      */
#define FUSE_ASYNC_READ		(1 << 0)
#define FUSE_POSIX_LOCKS	(1 << 1)
#define FUSE_FILE_OPS		(1 << 2)
#define FUSE_ATOMIC_O_TRUNC	(1 << 3)
#define FUSE_EXPORT_SUPPORT	(1 << 4)
#define FUSE_BIG_WRITES		(1 << 5)
#define FUSE_DONT_MASK		(1 << 6)

struct fuse_in_header {
	__uint32_t	len;
	__uint32_t	opcode;
	__uint64_t	unique;
	__uint64_t	nodeid;
	__uint32_t	uid;
	__uint32_t	gid;
	__uint32_t	pid;
	__uint32_t	padding;
};
struct fuse_out_header {
	__uint32_t	len;
	__int32_t	error;
	__uint64_t	unique;
};
struct fuse_write_in {
	__uint64_t	fh;
	__uint64_t	offset;
	__uint32_t	size;
	__uint32_t	write_flags;
	__uint64_t	lock_owner;
	__uint32_t	flags;
	__uint32_t	padding;
};
struct fuse_init_in {
	__uint32_t	major;
	__uint32_t	minor;
	__uint32_t	max_readahead;
	__uint32_t	flags;
};
struct fuse_init_out {
	__uint32_t	major;
	__uint32_t	minor;
	__uint32_t	max_readahead;
	__uint32_t	flags;
	__uint16_t	max_background;
	__uint16_t	congestion_threshold;
	__uint32_t	max_write;
};
