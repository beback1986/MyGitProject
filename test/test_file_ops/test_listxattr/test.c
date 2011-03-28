#include <stdlib.h>
#include <sys/types.h>
#include <attr/xattr.h>

#include "error_handle.h"

#define BUF_SIZE 500

#define FILE_PATH "Makefile"

int main (int argc, char *argv[])
{
	int ret		= -1;
	char *value	= NULL;
	char *value_p	= NULL;
	char *file_path	= NULL;

	if(argc > 1)
		file_path = argv[1];
	else
		file_path = FILE_PATH;

        value = malloc(BUF_SIZE);
	memset(value, '\0', 500);

	ret = listxattr(file_path, value, 500);
	POSITIVE_OR_RETURN(ret, "can not get xattr");
	value_p = value;
	printf("xattr total length:%d\n", ret);
	while (ret>0) {
		printf("ret=%d,value_p=%s\n", ret, value_p);
		ret -= strlen(value_p) + 1;
		value_p += strlen(value_p) + 1;
	}

/*	memcpy (value, "nihao", 5);
	ret = setxattr(FILE_PATH, name, value, 5, XATTR_CREATE);
	ZERO_OR_RETURN(ret, "can not set xattr");*/

	return 0;
}
