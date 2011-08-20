/*
 * =====================================================================================
 *
 *       Filename:  main.c
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
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "copy.h"
#include "log.h"

void usage()
{
	printf("usage:\n");
}

struct src_list {
	char **list;
	int len;
};

int get_opts(int argc, char *argv[], struct copy_opts *opts, struct src_list *src_list, char **dst)
{
	int i, j;

	src_list->list = (char **)calloc(argc, sizeof(char *));
	src_list->len = 0;
	for (i=1; i<argc; i++) {
		if (strncmp(argv[i], "--", 2) == 0) {
			if (strcmp(argv[i], "--base-attr") == 0)
				opts->opt_cp_battr = 1;
			else if (strcmp(argv[i], "--ext-attr") == 0)
				opts->opt_cp_xattr = 1;
			else if (strcmp(argv[i], "--force") == 0)
				opts->opt_force = 1;
			else {
				eprint("Unkown option:%s\n", argv[i]);
				return 1;
			}
		} else if (strncmp(argv[i], "-", 1) == 0) {
			j = 1;
			while (argv[i][j] != '\0') {
				if (argv[i][j] == 'r')
					opts->opt_cp_dir = 1;
				else if (argv[i][j] == 's')
					opts->opt_follow_sym = 1;
				else if (argv[i][j] == 'v')
					set_verbose(1);
				else {
					eprint("Unkown option:-%c\n", argv[i][j]);
					return 1;
				}
				j++;
			}
		} else {
			src_list->list[src_list->len++] = strdup(argv[i]);
		}
	}
	if (src_list->len < 2) {
		eprint("Missing operand.\n");
		return 1;
	}
	(*dst) = src_list->list[(src_list->len--) - 1];
	return 0;
}

int get_env(const struct src_list *src_list, const char *dst)
{
	struct stat st_buff;
	int flags = 0;

	if (src_list->len > 1) {
		SET_MULTI_SRC(flags);
	} else {
		if (!stat(src_list->list[0], &st_buff)) {
			SET_SRC_EXIST(flags);
			if (S_ISDIR(st_buff.st_mode))
				SET_SRC_DIR(flags);
		}
	}

	if (!stat(dst, &st_buff)) {
		SET_DST_EXIST(flags);
		if (S_ISDIR(st_buff.st_mode)) {
			SET_DST_DIR(flags);
		} else if (S_ISLNK(st_buff.st_mode)) {
			/* TODO: Follow symbolic to see if it's directory. */
			S_ISLNK(st_buff.st_mode);
		}
	}

	return flags;
}

int main(int argc, char *argv[])
{
	struct copy_opts opts;
	struct src_list src_list;
	char *dst = NULL;
	int flags;
	int i;

	if (get_opts(argc, argv, &opts, &src_list, &dst))
		return 1;

	flags = get_env(&src_list, dst);

	for (i=0; i<src_list.len; i++) {
		vprint("Begin copy, src:%s, dst:%s\n", src_list.list[i], dst);
		if (copy(&opts, src_list.list[i], dst, flags))
			return 1;
	}

	return 0;
}
