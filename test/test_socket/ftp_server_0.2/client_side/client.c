#include "client_header.h"

/************************************************************************************/
extern void init_data_channel();
extern void close_data_channel();
extern int scan_command(char* username,char command[],char arguments[][20]);
extern void QueryDirContent(int server_fd,char directory[]);
extern int ChangeDirectory(int server_fd,char new_dir[]);
extern int DownloadFile(int server_fd,char* filename);
extern int UploadFile(int server_fd,char* filename);
/************************************************************************************/
int main(int argc,char * argv[])
{

	int server_fd;
	int server_len;
	struct sockaddr_in server_address;
	struct sockaddr * server_address_ptr;
	int connect_count;		//record connect times
	int connect_result;
	server_address_ptr = (struct sockaddr *) &server_address;
	server_len = sizeof(server_address);
	char username[] = "command";

	server_fd = socket(AF_INET,SOCK_STREAM,0);	//initial the socket
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(CAMD_CHE_PORT);
	server_address.sin_addr.s_addr = inet_addr(argv[1]);

	connect_count = 0;
	connect_result = -1;
	while(connect_result == -1 && connect_count <= 10)
	{
		sleep(1);
		connect_result = connect(server_fd,server_address_ptr,server_len);//connect to server
		connect_count++;
		printf("connect %d times \n",connect_count);
		if(connect_count == 10){
//			printf("can not connect to the server!\nerror message:%s\n",strerror(errnum));
			exit(0);
		}
	}

	init_data_channel();

	char command[20];
	char arguments[5][20];
	if(connect_result == 0){
		while(scan_command(username,command,arguments) != 0){
			
//			printf("user input command:%s argument[0]:%s\targument[1]:%s\n",command,arguments[0],arguments[1]);
			if(strcmp(command,"download")==0){
				DownloadFile(server_fd,arguments[0]);
			}
			else if(strcmp(command,"upload")==0){
				UploadFile(server_fd,arguments[0]);
			}
//			else if(strcmp(command,"rename")==0){
//				RenameFile(server_fd,arguments[0],arguments[1]);
//			}
			else if(strcmp(command,"ls")==0){
				QueryDirContent(server_fd,arguments[0]);
			}
//			else if(strcmp(command,"rm")==0){
//				DeleteFile(server_fd,arguments[0]);
//			}
			else if(strcmp(command,"cd")==0){
				ChangeDirectory(server_fd,arguments[0]);
			}
printf("end of the main loop \n");
		}
	}
	close(client_recv_fd);
	close_data_channel();
	return 0;
}
