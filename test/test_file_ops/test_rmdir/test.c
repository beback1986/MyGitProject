#include <dirent.h>
#include <string.h>

#include "error_handle.h"

#define DIR_TO_OPEN "/mnt/glusterfs-mnt/test/aaaa/bbbb/cccc/cccc/11111"

int main (int argc, char *argv[])
{
	int dir;
	struct dirent *entry;

	dir = rmdir (DIR_TO_OPEN);
	ZERO_OR_RETURN(dir, "can not open dir");

	return 0;
}
