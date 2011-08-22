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
