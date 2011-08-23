/*
 * =====================================================================================
 *
 *       Filename:  copy.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/19/2011 07:01:50 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  dingyuan
 *        Company:  bwstor, China
 *
 * =====================================================================================
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <unistd.h>
#include <stdlib.h>
#include <libgen.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#include "fstack.h"
#include "log.h"
#include "copy.h"

static int follow_sym = 0;

int listdir(char *dir_path, struct dstack *ds)
{
	DIR *dir;
	struct dirent *entry;

	dir = opendir(dir_path);
	if (!dir)
		goto failed;

	while ((entry = readdir(dir)) != NULL) {
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
			continue;
		dstack_cflist_add(ds, entry->d_name);
	}

	closedir(dir);

	return 0;

failed:
	return 1;
}

/* Wraper the 2 stat function. If we see a link file in anywhere, the
 * follow symbolic option MUST be turned off! So there is no need to 
 * take any action to follow the symbolic.
 */
int mstat(const char *path, struct stat *buf)
{
	if (follow_sym)
		return stat(path, buf);
	return lstat(path, buf);
}

/* Base attr copy function for all file types.
 */
int __do_battr_copy(char *src, char *dst)
{
	return 0;
}

/* Extend attr copy function for all file types.
 */
int __do_xattr_copy(char *src, char *dst)
{
	return 0;
}

int do_dir_create(struct copy_opts *opts, char *src, char *dst)
{
	struct stat st_buff;

	vprint("create dir:%s\n", dst);

	if (mstat(src, &st_buff)) {
		eprint("File:%s wrong stat.(%s)\n",
							dst, strerror(errno));
		goto failed;
	}

	if (S_ISLNK(st_buff.st_mode) && !opts->opt_follow_sym) {
		/* TODO:create symblic here */
		goto out;
	}

	if (mkdir(dst, st_buff.st_mode)) {
		eprint("Can not create dir:%s.(%s)\n", 
							dst, strerror(errno));
		goto failed;
	}
out:
	chmod(dst, st_buff.st_mode);

	return 0;
failed:
	return 1;
}

int do_dir_revise(struct copy_opts *opts, char *src, char *dst)
{
	vprint("revice dir:%s\n", dst);
	return 0;
}

int __do_data_copy(char *src, char *dst)
{
	struct stat st_src;
	char *buf;
	char *pbuf;
	int fd_src;
	int fd_dst;
	int nread;
	int nwrite;
	int ret = 1;;

	if (mstat(src, &st_src)) {
		eprint("File:%s not exist(%s)\n", dst, strerror(errno));
		goto fail1;
	}

	if ((fd_src=open(src, O_RDONLY)) < 0) {
		eprint("Can not open file:%s(%s)\n", dst, strerror(errno));
		goto fail1;
	}
	if ((fd_dst=open(dst, O_WRONLY|O_CREAT, st_src.st_mode)) < 0) {
		eprint("Can not open file:%s(%s)\n", dst, strerror(errno));
		goto fail2;
	}
	if (!(buf=(char *)calloc(1, BUFFER_SIZE))) {
		eprint("Can not alloc memory!\n");
		goto fail3;
	}

	while (1) {
		nread = read(fd_src, buf, BUFFER_SIZE);
		if (nread == 0) {
			ret = 0;
			break;
		}
		if (nread < 0) 
			goto fail4;
		pbuf = buf;
		while (nread) {
			nwrite = write(fd_dst, pbuf, nread);
			if (nwrite <= 0)
				goto fail4;
			pbuf += nwrite;
			nread -= nwrite;
		}
	}

	chmod(dst, st_src.st_mode);

fail4:
	free(buf);
fail3:
	close(fd_dst);
fail2:
	close(fd_src);
fail1:
	return ret;
}

int __do_battr_copy(char *src, char *dst)
{
	return 0;
}

int __do_xattr_copy()
{
	return 0;
}

int do_file_copy(struct copy_opts *opts, char *src, char *dst)
{
	struct stat st_dst;

	vprint("copy :%s => %s\n", src, dst);

	if (mstat(dst, &st_dst)) {
		if (opts->opt_force) {
			remove(dst);
		} else {
			eprint("File:%s exist. Try --force\n", dst);
			goto failed;
		}
	}

	if (__do_data_copy(src, dst))
		goto failed;

	if (opts->opt_cp_battr)
		if (__do_battr_copy(src, dst))
			goto failed;

	if (opts->opt_cp_xattr)
		if (__do_xattr_copy(src, dst))
			goto failed;

	return 0;
failed:
	return 1;
}

int do_link_create(struct copy_opts *opts, char *src, char *dst)
{
	struct stat st_buff;

	chmod(dst, st_buff.st_mode);

	return 0;
failed:
	return 1;
}

int copy(struct copy_opts *opts, char *src_path, char *dst_path, int flags)
{
	struct dstack *ds;
	struct stat st_buff;
	char *src_name = NULL;
	char *dst_name = NULL;
	char *src_buff = NULL;
	char *dst_buff = NULL;
	char *psrc_buff = NULL;
	char *pdst_buff = NULL;
	int ret;
	int src_base_len;
	int dst_base_len;

	/* See mstat() for more information. */
	follow_sym = opts->opt_follow_sym;

	src_buff = (char *)calloc(1, PATH_MAX);
	dst_buff = (char *)calloc(1, PATH_MAX);
	strcpy(src_buff, dirname(strdup(src_path)));
	src_base_len  = strlen(src_buff);
	psrc_buff = src_buff + src_base_len;
	src_name = basename(strdup(src_path));

	if (IS_MULTI_SRC(flags)) {
		if (IS_DST_EXIST(flags)) {
			eprint("dst not exist!\n");
			return 1;
		} else if (!IS_DST_DIR(flags)) {
			eprint("dst is not dir!\n");
			return 1;
		}
	} else {
		if (IS_DST_EXIST(flags) ) {
			if (!IS_DST_DIR(flags)) {
				/* Dst exist, and is a file, do the copy directly. */
				return do_file_copy(opts, src_path, dst_path);
			}
		} else {
			if (IS_SRC_EXIST(flags)) {
				if (IS_SRC_DIR(flags)) {
					do_dir_create(opts, src_path, dst_path);
				} else {
					return do_file_copy(opts, src_path, dst_path);
				}
			} else {
				eprint("src not exist!\n");
				return 1;
			}
		}
	}
	strcpy(dst_buff, dst_path);
	dst_base_len = strlen(dst_buff);
	pdst_buff = dst_buff + dst_base_len;

	ds = dstack_new();
	dstack_push_dir(ds);
	dstack_cflist_add(ds, src_name);

	while (!dstack_isempty(ds)) {
		if (dstack_cflist_next(ds, psrc_buff)) {
			dstack_pop_dir(ds);
			if (!dstack_cflist_next(ds, psrc_buff)) {
				/* Incase that we have arrive at the bottom of the stack. */
				strcpy(pdst_buff, psrc_buff);
				do_dir_revise(opts, src_buff, dst_buff);
			}
			goto clean;
		}
		strcpy(pdst_buff, psrc_buff);
		ret = mstat(src_buff, &st_buff);

		if (ret) {
			eprint("file:%s not exist!\n", src_buff);
		} else if (S_ISDIR(st_buff.st_mode)) {
			dstack_cflist_prev(ds);
			dstack_push_dir(ds);
			listdir(src_buff, ds);
			if (do_dir_create(opts, src_buff, dst_buff))
				goto failed;
		} else if (S_ISLNK(st_buff.st_mode)) {
			if (do_link_create(opts, src_buff, dst_buff))
				goto failed;
		} else if (S_ISREG(st_buff.st_mode)) {
			if (do_file_copy(opts, src_buff, dst_buff))
				goto failed;
		} else {
			/* If we don't know the file type, do nothing. */
			vprint("Unknown file type!:%s\n", src_buff);
		}

clean:
		memset(psrc_buff, '\0', PATH_MAX-src_base_len);
		memset(pdst_buff, '\0', PATH_MAX-dst_base_len);
	}
	return 0;

failed:
	return 1;
}
