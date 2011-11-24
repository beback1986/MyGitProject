#include <sys/types.h>
#include <attr/xattr.h>
#include <string.h>

#include "error_handle.h"

#define BUF_SIZE 300

#define FILE_PATH "Makefile"

int main (int argc, char *argv[])
{
	int ret;
	char name[BUF_SIZE] = "hello";
	char val[BUF_SIZE] = "123123";
	int  val_size = strlen(val);
	char *filepath;

	if (argc < 2) {
		printf("provide file path please.\n");
		goto out;
	}
	filepath = strdup(argv[1]);

	ret = setxattr(filepath, name, val, val_size, 0);
	ZERO_OR_RETURN(ret, "can not get xattr");

out:
	return 0;
}
