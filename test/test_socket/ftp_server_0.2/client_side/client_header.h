#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <sys/stat.h>

#define BUF_SIZE 1048576

#define CAMD_CHE_PORT 30666

#define DATA_CHE_PORT 30667

char buffer[BUF_SIZE];

char current_directory[1024];

int client_recv_fd;

struct file_list
{
	int file_type;
	char file_name[100];
	int file_size;
	struct file_list* next_file;
};

