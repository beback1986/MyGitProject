#include <sys/types.h>
#include <attr/xattr.h>
#include <string.h>
#include <stdlib.h>
#include <error.h>

#include "error_handle.h"

#define XATTR_NAME "security.NTACL"
//#define XATTR_NAME "user.asdf"

int main (int argc, char *argv[])
{
	int ret;
	char *value;
	char *filepath;

	if (argc < 2) {
		printf("provide file path please.\n");
		goto out;
	}
	filepath = strdup(argv[1]);

	ret = getxattr(filepath, XATTR_NAME, NULL, 0);
	printf("first getxattr return %d\n", ret);
	POSITIVE_OR_RETURN(ret, "first getxattr failed");

	value = calloc(1, ret);
	if (!value) {
		printf("can not alloc mem\n");
		goto out;
	}

	ret = getxattr(filepath, XATTR_NAME, value, ret);
	POSITIVE_OR_RETURN(ret, "second getxattr failed");

	printf("got value of size %d (%s)\n", ret, value);

out:
	return 0;
}
