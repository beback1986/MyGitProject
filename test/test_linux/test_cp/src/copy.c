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

#include <sys/stat.h>
#include <unistd.h>


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

int do_dir_create()
{
	return 0;
}

int do_dir_revise()
{
	return 0;
}

int do_file_copy()
{
	return 0;
}

int copy(struct copy_opts *opts, char *src_path, char *dst_path, int is_dst_dir)
{
	struct dstack *ds;
	struct stat src_st;
	struct stat dst_st;
	struct stat st_buff;
	char *src_base_path;
	char *dst_base_path;
	char *filename;
	char pa_buff[PATH_MAX];
	char *pbuff = NULL;
	int ret;
	int src_base_len;

	ret = stat(dst_path, &dst_st);
	if (ret < 0) {
		if (is_dst_dir) {
			eprint("cp: target dir %s not exist!\n", dst_path);
			return 1;
		}
	}
	else {
		if (S_ISDIR(dst_st.st_mode))
			dst_base_path = strdup(dst_path);
		else
	}

	ds = dstack_new();
	dstack_push_flist(ds);

	src_base_path = dirname(src_path);
	filename = basename(src_path);
	dstack_cflist_add(ds, filename);

	strcpy(pa_buff, src_base_path);
	src_base_len = strlen(src_base_path);
	pbuff = pa_buff + src_base_len;

	while (!dstack_empty(ds)) {
		if (!dstack_cflist_next(ds, pbuff)) {
			dstack_pop_dir(ds);
			do_dir_revise();
			continue;
		}
		ret = stat(pa_buff, &st_buff);
		if (S_ISDIR(st_buff.st_mode)) {
			listdir(pa_buff, ds);  //Sort all files by name
			do_dir_create();
		}
		else if (is_file ¦¦ is_symbol) {
			do_file_copy();
		}
		else {
			/* If we don't know the file type, do nothing. */
			print("Unknown file type!");
		}
		memset(pbuff, '\0', MAX_PATH-src_base_len);
	}
	return 0;
}
