/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */

/* Use our own fusermount */
#define GF_FUSERMOUNT 1

/* Use syslog for logging */
#define GF_USE_SYSLOG 1

/* have argp */
#define HAVE_ARGP 1

/* define if found backtrace */
#define HAVE_BACKTRACE 1

/* define if lvm2app library found and bd xlator enabled */
#define HAVE_BD_XLATOR 1

/* enable building crypt encryption xlator */
#define HAVE_CRYPT_XLATOR 1

/* Define to 1 if you have the declaration of `lvm_lv_from_name', and to 0 if
   you don't. */
#define HAVE_DECL_LVM_LV_FROM_NAME 1

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define to 1 if you have the <execinfo.h> header file. */
#define HAVE_EXECINFO_H 1

/* define if fallocate exists */
#define HAVE_FALLOCATE 1

/* define if fdatasync exists */
/* #undef HAVE_FDATASYNC */

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* have sys/ioctl.h */
#define HAVE_IOCTL_IN_SYS_IOCTL_H 1

/* libaio based POSIX enabled */
#define HAVE_LIBAIO 1

/* Define to 1 if you have the `crypto' library (-lcrypto). */
#define HAVE_LIBCRYPTO 1

/* Define to 1 if you have the `dl' library (-ldl). */
/* #undef HAVE_LIBDL */

/* Define to 1 if you have the <libintl.h> header file. */
#define HAVE_LIBINTL_H 1

/* Define to 1 if you have the `pthread' library (-lpthread). */
#define HAVE_LIBPTHREAD 1

/* Define to 1 if you have the `rt' library (-lrt). */
#define HAVE_LIBRT 1

/* Define to 1 if using libxml2. */
#define HAVE_LIB_XML 1

/* define if zlib is present */
#define HAVE_LIB_Z 1

/* define if found linkat */
#define HAVE_LINKAT 1

/* Define to 1 if you have the <linux/falloc.h> header file. */
#define HAVE_LINUX_FALLOC_H 1

/* define if llistxattr exists */
#define HAVE_LLISTXATTR 1

/* have malloc.h */
#define HAVE_MALLOC_H 1

/* define if found malloc_stats */
#define HAVE_MALLOC_STATS 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the <openssl/cmac.h> header file. */
#define HAVE_OPENSSL_CMAC_H 1

/* Define to 1 if you have the <openssl/md5.h> header file. */
#define HAVE_OPENSSL_MD5_H 1

/* define if posix_fallocate exists */
#define HAVE_POSIX_FALLOCATE 1

/* define if libglib-2.0 library found and QEMU Block translator enabled */
#define HAVE_QEMU_BLOCK 1

/* readline enabled CLI */
#define HAVE_READLINE 1

/* define if found setfsuid setfsgid */
#define HAVE_SET_FSID 1

/* define if found spinlock */
#define HAVE_SPINLOCK 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* define if found strnlen */
#define HAVE_STRNLEN 1

/* Define to 1 if `st_atimespec.tv_nsec' is member of `struct stat'. */
/* #undef HAVE_STRUCT_STAT_ST_ATIMESPEC_TV_NSEC */

/* Define to 1 if `st_atim.tv_nsec' is member of `struct stat'. */
#define HAVE_STRUCT_STAT_ST_ATIM_TV_NSEC 1

/* Define to 1 if using probes. */
/* #undef HAVE_SYSTEMTAP */

/* Define to 1 if you have the <sys/epoll.h> header file. */
#define HAVE_SYS_EPOLL_H 1

/* Define to 1 if you have the <sys/extattr.h> header file. */
/* #undef HAVE_SYS_EXTATTR_H */

/* Define to 1 if you have the <sys/ioctl.h> header file. */
#define HAVE_SYS_IOCTL_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <sys/xattr.h> header file. */
#define HAVE_SYS_XATTR_H 1

/* define if found umount2 */
#define HAVE_UMOUNT2 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#define LT_OBJDIR ".libs/"

/* defined if lvm_lv_from_name() was not found in the lvm2app.h header, but
   can be linked */
/* #undef NEED_LVM_LV_FROM_NAME_DECL */

/* Name of package */
#define PACKAGE "glusterfs"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "gluster-users@gluster.org"

/* Define to the full name of this package. */
#define PACKAGE_NAME "glusterfs"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "glusterfs 3.5qa2"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "glusterfs"

/* Define to the version of this package. */
#define PACKAGE_VERSION "3.5qa2"

/* The size of `int', as computed by sizeof. */
#define SIZEOF_INT 4

/* The size of `long', as computed by sizeof. */
#define SIZEOF_LONG 8

/* The size of `long long', as computed by sizeof. */
#define SIZEOF_LONG_LONG 8

/* The size of `short', as computed by sizeof. */
#define SIZEOF_SHORT 2

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Version number of package */
#define VERSION "3.5qa2"

/* Define to 1 if `lex' declares `yytext' as a `char *' by default, not a
   `char[]'. */
#define YYTEXT_POINTER 1
