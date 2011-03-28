#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <dirent.h>
#include <sys/stat.h>
#include <netinet/in.h>

#include <sys/timeb.h>

#define BUF_SIZE 4096

#define MAX_DIR_LENGTH 100

#define CAMD_CHE_PORT 30666

#define DATA_CHE_PORT 30667

#define SERVER_IP 192.168.1.102

char root_directory[MAX_DIR_LENGTH];

struct file_list
{
	struct dirent * file;
	struct stat * file_stat;
	struct file_list* next_file;
};

struct sockaddr_sockfd
{
	int socket_fd;
	struct sockaddr_in socket_addr;
};
