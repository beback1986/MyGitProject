/*
 * =====================================================================================
 *
 *       Filename:  copy.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/19/2011 07:08:28 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  dingyuan
 *        Company:  bwstor, China
 *
 * =====================================================================================
 */

#ifndef __COPY_H__
#define __COPY_H__

#define MULTI_SRC 0x0001
#define DST_EXIST 0x0002
#define DST_DIR   0x0004
#define SRC_EXIST 0x0008  /* Works only when MULTI_SRC is not set. */
#define SRC_DIR   0x0010  /* Works only when MULTI_SRC is not set. */

#define IS_MULTI_SRC(flags) (flags & MULTI_SRC)
#define IS_DST_EXIST(flags) (flags & DST_EXIST)
#define IS_DST_DIR(flags)   (flags & DST_DIR)
#define IS_SRC_EXIST(flags) (flags & SRC_EXIST)
#define IS_SRC_DIR(flags)   (flags & SRC_DIR)

#define SET_MULTI_SRC(flags) {flags |= MULTI_SRC;}
#define SET_DST_EXIST(flags) {flags |= DST_EXIST;}
#define SET_DST_DIR(flags)   {flags |= DST_DIR;}
#define SET_SRC_EXIST(flags) {flags |= SRC_EXIST;}
#define SET_SRC_DIR(flags)   {flags |= SRC_DIR;}

/* Options for copy. */
struct copy_opts {
	/* If true, copy directories recursive. */
	int	opt_cp_dir;
	/* If true, copy the file that symbolic points to. */
	int opt_follow_sym;
	/* If true, copy base attr to the destnation file.
	 * Base attr includes access time and modify time. */
	int opt_cp_battr;
	/* If true, copy all extend attrs to the destnation file. */
	int opt_cp_xattr;
	/* If true, force to replace a file that has a duplication name. */
	int opt_force;
};

int copy(struct copy_opts *opts, char *src_path, char *dst_path, int is_dst_dir);

#endif /* __COPY_H__ */
