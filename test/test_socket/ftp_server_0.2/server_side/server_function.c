#include "server_header.h"
/***************************************************************************/
//functions to extern
void session(struct sockaddr_sockfd* client_addr_fd);
/***************************************************************************/
//functions used within this file
int request_analysis(int client_fd,char* buffer,char func[],char arguments[][100]);
int init_data_connection(struct sockaddr_in client_addr);
int GetDirContent(int client_fd,char* buffer,char* directory,struct sockaddr_in client_addr);
struct file_list * get_file_list(char * abs_dir);
void free_file_list(struct file_list * file_list_head);
int SendFileNames(int client_data_channel_fd,char * buffer,struct file_list * pfile);
int ChangeDirectory(int server_fd, char buffer[],char old_dir[],char new_dir[]);
int DownloadFile(int client_fd,char*buffer,char* directory,char* filename,struct sockaddr_in client_addr);
int send_file(int client_recv_fd,FILE* file_to_send);
int UploadFile(int client_fd,char* buffer,char* directory,char* filename,struct sockaddr_in client_addr);
int recv_file(int client_send_fd,FILE* file_to_recv);
/***************************************************************************/

void session(struct sockaddr_sockfd* client_addr_fd)
{
	int n = 1;
	char func[20];
	int client_fd = 0 ;
	char arguments[5][100];
	char buffer[BUF_SIZE];

	client_fd = client_addr_fd->socket_fd;		//the user's socket handler
	struct sockaddr_in client_addr;					//the user's address structure
	memcpy(&client_addr,&(client_addr_fd->socket_addr),sizeof(client_addr_fd->socket_addr));
	
	printf("get connection with:%s at port: %d\n",inet_ntoa(client_addr.sin_addr.s_addr),client_addr.sin_port);

	while(request_analysis(client_fd,buffer,func,arguments)>0){

		printf("%s execute command:%s\targu[0]:%s\targu[1]:%s\taru[2]:%s \n",inet_ntoa(client_addr.sin_addr.s_addr),func,arguments[0],arguments[1],arguments[2]);
		if(strcmp(func,"downloadfile")==0){
			DownloadFile(client_fd,buffer,arguments[0],arguments[1],client_addr);
		}
		else if(strcmp(func,"uploadfile")==0){
			UploadFile(client_fd,buffer,arguments[0],arguments[1],client_addr);
		}
		else if(strcmp(func,"querydircontent")==0){
			GetDirContent(client_fd,buffer,arguments[0],client_addr);
		}
		else if(strcmp(func,"rename")==0){
//			RenameFile(client_fd,buffer,arguments[0],arguments[1],arguments[2]);
		}
		else if(strcmp(func,"deletefile")==0){
//			DeleteFile(client_fd,buffer,arguments[0],arguments[1]);
		}
		else if(strcmp(func,"changedirectory")==0){
			ChangeDirectory(client_fd,buffer,arguments[0],arguments[1]);
		}
	}
	printf("%s disconnected \n",inet_ntoa(client_addr.sin_addr.s_addr));
	close(client_fd);
	return;
}
/*************************************************************************
	int request_analysis(int client_fd,char* buffer,char func[],char arguments[][100])
	func:analysis user's command package, get user's command and arguments
*************************************************************************/
int request_analysis(int client_fd,char* buffer,char func[],char arguments[][100]){
	int n=0;
	int i=0;
	int j=0;
	int sock_stat = 0;
	char request[10];
	char tm[20];
	
	sock_stat = recv(client_fd,buffer,BUF_SIZE,0);
	if(sock_stat<0){
		printf("request_analysis recv package error! \n");
		return -1;
	}
	
	else if(sock_stat==0){
		return 0;
	}
	//analysis the first part of the package
	while(buffer[n]!=';'){
		request[n] = buffer[n];
		n++;
	}
	request[n+1] = '\0';
	if(strncmp(request,"request",7)==0){
		//analysis the second part of the package
		n++;
		i=0;
		while(buffer[n]!=';'){
			func[i] = buffer[n];
			n++;
			i++;
		}
		func[i] = '\0';
		//analysis the rest part of the package
		n++;
		i=0;
		j=0;
		while(buffer[n]!='|'){
			if(buffer[n]==';'){
				arguments[j][i] = '\0';
				i=0;
				j++;
				n++;
			}
			else{
				arguments[j][i] = buffer[n];
				n++;
				i++;
			}
		}
		arguments[j][i] = '\0';
	}
	return sock_stat;
}
/**************************************************************************
	int init_data_connection(struct sockaddr_in client_addr)
	func:connect to client use data chanel
**************************************************************************/
int init_data_connection(struct sockaddr_in client_addr)
{
	int client_recv_fd;
	client_recv_fd = socket(AF_INET,SOCK_STREAM,0);
	client_addr.sin_port=htons(DATA_CHE_PORT);
	printf("%s require data connection at port: %d\n",inet_ntoa(client_addr.sin_addr.s_addr),client_addr.sin_port);
	sleep(1);
	if(connect(client_recv_fd,(struct sockaddr *) &client_addr,sizeof(client_addr))==-1){
		printf("can not connect to the client \n");
		return -1;
	}
	return client_recv_fd;
}
/**************************************************************************
	int GetDirContent(int client_fd,char* buffer,char* directory,struct sockaddr_in client_addr)
	func:return the files' informations in the dir client queried
**************************************************************************/
int GetDirContent(int client_fd,char* buffer,char* directory,struct sockaddr_in client_addr)
{
	int bytes_send = 0;
	char abs_dir[MAX_DIR_LENGTH];
	int client_data_channel_fd;
	struct file_list * files_head = NULL;

	sprintf(abs_dir,"%s%s",root_directory,directory);
	files_head = get_file_list(abs_dir);
	if(files_head != NULL){
		sprintf(buffer,"ack;rcvdircontent|");
		if(send(client_fd,buffer,BUF_SIZE,0)==-1)
		{
			printf("send error\n");
			exit(1);
		}
	}
	else{
		sprintf(buffer,"error;1001|");
		if(send(client_fd,buffer,BUF_SIZE,0)==-1)
		{
		   printf("send error\n");
		   exit(1);
		}
		return -1;
	}
	client_data_channel_fd = init_data_connection(client_addr);
	if((bytes_send = SendFileNames(client_data_channel_fd,buffer,files_head)) == -1){
		printf("SendFileNames send package failed!\n");
		free_file_list(files_head);
		return -1;
	}
	printf("%d bytes sended \n",bytes_send);
	close(client_data_channel_fd);
 	free_file_list(files_head);
	return 1;
}
/************************************************************************
	int get_file_list(char * abs_dir,struct file_list * file_list_head)
	func:read all files' information in 'abs_dir',stored in 'file_list_head'
	return value:
		-1 	if the dir can not open
		>=0 else
************************************************************************/
struct file_list * get_file_list(char * abs_dir)
{
	DIR	* dir_fd = NULL;
	char file_abs_dir[MAX_DIR_LENGTH+100];
	struct file_list * file_list_head = NULL;
	struct file_list * file_list_tail = NULL;
	struct file_list * tem = NULL;
	struct dirent * file_tem = NULL;
	struct dirent * file = NULL;
	struct stat * file_stat = NULL;

	if((dir_fd = opendir(abs_dir)) == NULL){
		printf("can not open the dir client requaired! \n");
		return NULL;
	}

	file_list_head = malloc(sizeof(struct file_list));
	file_list_head->file_stat = malloc(sizeof(struct stat));
	file_list_head->file = malloc(sizeof(struct dirent));
	file_list_head->next_file = NULL;
	file_list_tail = file_list_head;
	while((file_tem = readdir(dir_fd)) != NULL){
		//get the stat struct of this file
		sprintf(file_abs_dir,"%s/%s",abs_dir,file_tem->d_name);
		file_stat = malloc(sizeof(struct stat));
		stat(file_abs_dir,file_stat);
		//get the dirent struct of this file
		file = malloc(sizeof(struct dirent));
		memcpy(file,file_tem,sizeof(struct dirent));

		tem = malloc(sizeof(struct file_list));
		tem->file = file;
		tem->file_stat = file_stat;
		tem->next_file = NULL;
		file_list_tail->next_file = tem;
		file_list_tail = file_list_tail->next_file;
	}
	if(closedir(dir_fd) == -1){
		printf("dir close failed \n");
	}
	return file_list_head;
}
/************************************************************************
	void free_file_list(struct file_list * file_list_head)
	func:free the memery of the input list 'file_list_head'
************************************************************************/
void free_file_list(struct file_list * file_list_head)
{
	struct file_list * tem = NULL;
	while(file_list_head != NULL){
		tem = file_list_head;
		file_list_head = file_list_head->next_file;
		free(tem->file_stat);
		free(tem->file);
		free(tem);
	}
}
/************************************************************************
	int SendFileNames(int client_data_channel_fd,struct file_list * files_head)
	func:send the files' informations to client
	return : -1  send file failure
			 >=0 send successfully, return the bytes sended
************************************************************************/
int SendFileNames(int client_data_channel_fd,char * buffer,struct file_list * pfile)
{
	int bytes_send = 0;
	char tem_str[200];
	memset(buffer,'\0',strlen(buffer));
	while(pfile->next_file != NULL){
		pfile = pfile->next_file;
		sprintf(tem_str,"%d<%s<%d/",S_ISDIR(pfile->file_stat->st_mode),pfile->file->d_name,pfile->file_stat->st_size);
		strcat(buffer,tem_str);
		if(strlen(buffer) >= 3072){
			if((bytes_send += send(client_data_channel_fd,buffer,strlen(buffer),0)) == -1)
			{
				printf("send error \n");
				return -1;
  		 	}
			memset(buffer,'\0',strlen(buffer));
		}
	}
	if(strlen(buffer)>0){
		if((bytes_send += send(client_data_channel_fd,buffer,strlen(buffer),0)) == -1)
		{
			printf("send error \n");
			return -1;
  	 	}
		memset(buffer,'\0',strlen(buffer));
	}
	return bytes_send;
}
/**********************************************************************
  int ChangeDirectory(int server_fd, char old_dir[],char new_dir[])
  function:change directory 
  parameters: 
	      server_fd: the socket file description of server
	      old_dir: the directory to change
          new_dir: the directory to become
  return : -1: error
            1: directory changed successfully!
************************************************************************/
int ChangeDirectory(int server_fd, char buffer[],char old_dir[],char new_dir[])
{
	char abs_dir[100];
	DIR	*pdir;

	sprintf(abs_dir,"%s%s%s",root_directory,old_dir,new_dir);
	if((pdir=opendir(abs_dir)) == NULL)
	{
		sprintf(buffer,"error;1002|");
		if(send(server_fd,buffer,BUF_SIZE,0)==-1)
		{
		   printf("send error\n");
		   exit(1);
		}
		closedir(pdir);
		return -1;
	}
	sprintf(buffer,"ack;directorychanged|");
	if(send(server_fd,buffer,BUF_SIZE,0)==-1)
	{
		printf("send error\n");
		exit(1);
	}
	return 1;
}
/*************************************************************************
	int DownloadFile(int client_fd,char*buffer,char* directory,char* filename,struct sockaddr_in client_addr)
	func:send the file client required to 'client_fd'
*************************************************************************/
int DownloadFile(int client_fd,char*buffer,char* directory,char* filename,struct sockaddr_in client_addr)
{
	int bytes_send = 0;
	char abs_directory[100];
	int client_recv_fd;
	FILE* file_to_send;
	sprintf(abs_directory,"%s%s%s",root_directory,directory,filename);
	printf("URL:%s\n",abs_directory);

	if((file_to_send=fopen(abs_directory,"r"))==NULL){
		buffer = "error;1003|";
		if(send(client_fd,buffer,BUF_SIZE,0)<=0)
			printf("DownloadFile send package error \n");
		return -1;
	}
	else{
		buffer = "ack;filedownloaded|";
		if(send(client_fd,buffer,BUF_SIZE,0)<=0){
			printf("DownloadFile send package error \n");
			return -1;
		}
		client_recv_fd = init_data_connection(client_addr);
		if(client_recv_fd<=0)
			return -1;
		if((bytes_send = send_file(client_recv_fd,file_to_send)) == -1){
			printf("send file failed\n");
			return -1;
		}
	}
	printf("%d bytes sended \n",bytes_send);
	fclose(file_to_send);
	close(client_recv_fd);
	return 1;
}
/**************************************************************************
	int send_file(int client_recv_fd,FILE* file_to_send)
	func   : send file to 'client_recv_fd'
	return : -1  send package failed
			 >=0 send file successfully, return the amount of bytes sended
**************************************************************************/
int send_file(int client_recv_fd,FILE* file_to_send)
{
	int bytes_send;		//the length of sended data
	int bytes_read;
	char buffer[BUF_SIZE];

	bytes_send = 0;
	while(!feof(file_to_send))
	{
		bytes_read = fread(buffer,1,BUF_SIZE,file_to_send);
		bytes_send += send(client_recv_fd,buffer,bytes_read,0);
	}
	return bytes_send;
}
/*********************************************************************************
*********************************************************************************/
int UploadFile(int client_fd,char* buffer,char* directory,char* filename,struct sockaddr_in client_addr)
{
	char abs_directory[100];
	int client_send_fd;
	FILE* file_to_recv;
	sprintf(abs_directory,"%s%s%s",root_directory,directory,filename);
	printf("URL:%s\n",abs_directory);
	if((file_to_recv=fopen(abs_directory,"w"))==NULL){
		buffer = "error;1004|";
		if(send(client_fd,buffer,BUF_SIZE,0)<=0)
			printf("DownloadFile send package error \n");
		close(abs_directory);
		return -1;
	}
	else{
		buffer = "ack;fileuploaded|";
		if(send(client_fd,buffer,BUF_SIZE,0)<=0){
			printf("DownloadFile send package error \n");
			return -1;
		}
		client_send_fd = init_data_connection(client_addr);
		if(client_send_fd<=0)
			return -1;
		recv_file(client_send_fd,file_to_recv);
		close(client_send_fd);
		fclose(file_to_recv);
	}
	return 1;
}
/*********************************************************************************

*********************************************************************************/
int recv_file(int client_send_fd,FILE* file_to_recv)
{
	int bytes_count = 0;
	int bytes_recv;//the length of character receved
	int bytes_write;
	char buffer[BUF_SIZE];
	bytes_recv = recv(client_send_fd,buffer,BUF_SIZE,0);
	bytes_count += bytes_recv;
	while(bytes_recv>0)
	{
		bytes_write=fwrite(buffer,1,bytes_recv,file_to_recv);
		bytes_recv=recv(client_send_fd,buffer,BUF_SIZE,0);
		bytes_count += bytes_recv;
	}
	printf("%d bytes recieved \n",bytes_count);
	return bytes_count;
}
