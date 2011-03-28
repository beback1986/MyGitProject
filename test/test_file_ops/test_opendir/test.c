#include <dirent.h>
#include <string.h>

#include "error_handle.h"

#define DIR_TO_OPEN "/mnt/glusterfs-mnt/first/second"

int main (int argc, char *argv[])
{
	DIR *dir;
	struct dirent *entry;

	dir = opendir (DIR_TO_OPEN);
	NONE_ZERO_OR_RETURN(dir, "can not open dir");
/*
	while ((entry = readdir(dir)) != NULL) {
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
			continue;
		printf("read entry: %s\n", entry->d_name);
	}
*/
	closedir(dir);

	return 0;
}
