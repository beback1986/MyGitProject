#include <unistd.h> 

#include "error_handle.h"

#define FILE_PATH "a.out"

int main (int argc, char *argv[])
{
	int ret;

	ret = unlink (FILE_PATH);
	ZERO_OR_RETURN(ret, "can not unlink");

	return 0;
}
