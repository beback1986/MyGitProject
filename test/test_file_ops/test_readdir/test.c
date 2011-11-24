#include <limits.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "error_handle.h"

#define DIR_PATH "/mnt/Bonnie.8363/00006/"

int main (int argc, char *argv[])
{
	DIR *dir;
	struct dirent *entry;
	char *dir_p;
	char *file_p;
	int fd;

	dir_p = calloc(1, PATH_MAX);
	if (!dir_p) {
		printf("can not alloc mem.\n");
		goto failed;
	}
	memcpy(dir_p, DIR_PATH, strlen(DIR_PATH));
	file_p = dir_p + strlen(DIR_PATH);

	dir = opendir (DIR_PATH);
	NONE_ZERO_OR_RETURN(dir, "can not open dir.");

	while ((entry = readdir(dir)) != NULL) {
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
			continue;
		strcpy(file_p, entry->d_name);
		printf("open file: d_ino=%ld,\td_off=%ld,\td_reclen=%hd,\td_name=%s\n",
				entry->d_ino,
				entry->d_off,
				entry->d_reclen,
				entry->d_name);
		fd = open(dir_p, O_RDONLY);
		if (fd  < 0) {
			PERROR("can not open file");
			continue;
		}
		close(fd);
	}

	closedir(dir);

	return 0;
failed:
	return 1;
}
