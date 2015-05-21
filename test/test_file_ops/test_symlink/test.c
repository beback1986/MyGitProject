#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <libgen.h>
#include <errno.h>

#define MAX_PATH 4096

void follow_link(const char *src_path)
{
	struct stat st; 
	int ret;
	ssize_t sym_len;
	char *dev_path = NULL;
	char *sym_path = NULL;


	sym_path = calloc(1, MAX_PATH);
	dev_path = calloc(1, MAX_PATH);

	if (src_path[0] != '/') {
		if (getcwd(dev_path, MAX_PATH-1) == NULL) {
			printf("BW_VMDIO:Get cwd error. %d(%s)\n", errno, strerror(errno));
		}
		strcat(dev_path, "/");
	}
	strncat(dev_path, src_path, MAX_PATH-strlen(dev_path)-1);

	while (1) {
		ret = lstat(dev_path, &st);
		if (ret) {
			printf("BW_VMDIO:stat %s failed.\n", dev_path);
			goto failed;
		}
		if (S_ISLNK(st.st_mode)) {
			sym_len = readlink(dev_path, sym_path, MAX_PATH-1);
			if (sym_len < 0) {
				printf("BW_VMDIO:read link %s failed\n", dev_path);
				goto failed;
			}   
			sym_path[sym_len] = '\0';
			if (sym_path[0] == '/') {
				strcpy(dev_path, sym_path);
			} else {
				dirname(dev_path);
				strcat(dev_path, "/");
				strncat(dev_path, sym_path, MAX_PATH-strlen(dev_path)-1);
			}   
			continue;
		} else if (S_ISBLK(st.st_mode)) {
			break;
		} else {
			printf("BW_VMDIO:%s unkown file mode %o\n", dev_path, st.st_mode);
			goto failed;
		}   
	}

	printf("BW_VMDIO: final path = %s\n", dev_path);

	return ;
failed:
	return ;
}

int main(int argc, char *argv[])
{
	char *file_path;

	if (argc < 2) {
		printf("Provide a path.\n");
		goto out;
	}

	file_path = strdup(argv[1]);

	follow_link(file_path);

out:
	return 0;
}
