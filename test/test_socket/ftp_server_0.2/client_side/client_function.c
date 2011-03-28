#include "client_header.h"
/***************************************************************************/
//functions to extern
void init_data_channel();
void close_data_channel();
int scan_command(char* username,char command[],char arguments[][20]);
void QueryDirContent(int server_fd,char directory[]);
int ChangeDirectory(int server_fd,char new_dir[]);
int DownloadFile(int server_fd,char* filename);
int UploadFile(int server_fd,char* filename);
/***************************************************************************/
//functions used within this file
/***************************************************************************/
int get_data_connection();
void close_data_connection(int server_data_channel_fd);
struct file_list * get_files_information(char * buffer);
void free_file_list(struct file_list * file_list_head);
int recv_file(int server_send_fd,FILE* file_to_recv);
int send_file(int server_recv_fd,FILE* file_to_send);
/*************************************************************************
	void init_data_channel()
	func:init the data channel,listen to the server to connect
*************************************************************************/
void init_data_channel()
{
	struct sockaddr_in client_addr;
	
	client_recv_fd = socket(AF_INET,SOCK_STREAM,0);
	client_addr.sin_family = AF_INET;
	client_addr.sin_port = htons(DATA_CHE_PORT);
	client_addr.sin_addr.s_addr = INADDR_ANY;

	if(bind(client_recv_fd,(struct sockaddr*) &client_addr,sizeof(client_addr))==-1){	//bind the address and fd
		printf("bind error! \n");
		exit(0);
	}
	if(listen(client_recv_fd,5)==-1)	//listen to the client
		printf("listen error! \n");

}
/*************************************************************************
	void close_data_channel()
	func:close the data channel
*************************************************************************/
void close_data_channel()
{
	if(close(client_recv_fd) == 0)
		printf("data chenal close success \n");
	else
		printf("data chenal close failed \n");
}
/**************************************************************************
	int get_data_connection()
	func:accept server data chenle connect request
**************************************************************************/
int get_data_connection()
{
	int server_data_channel_fd,server_len;
	
	server_data_channel_fd = -1;
	struct sockaddr_in server_addr;
	server_len = sizeof(server_addr);

	server_data_channel_fd = accept(client_recv_fd,(struct sockaddr*) &server_addr,&server_len);	//accept the client

	return server_data_channel_fd;
}
/**************************************************************************
	int close_data_connection()
	func:accept server data chenle connect request
**************************************************************************/
void close_data_connection(int server_data_channel_fd)
{
//	close(server_data_channel_fd);
	if(close(server_data_channel_fd) != 0)
		printf("data channel connection close failed \n");
	else
		printf("data channel connection close success \n");
}
/***************************************************************************
	int scan_command(char* username,char command[],char arguments[][20])
	func:scan comand from user input
***************************************************************************/
int scan_command(char* username,char command[],char arguments[][20])
{
	printf("command>");
	scanf("%s",command);
	if(strcmp(command,"ls") == 0)    //display directories or files in a directory
	{
		scanf("%s",arguments[0]);
		return 1;
	}
	else if(strcmp(command,"cd") == 0)  //change directory
	{
		scanf("%s",arguments[0]);
		return 1;
	}
	else if(strcmp(command,"rm") == 0) //delete file
	{
		scanf("%s",arguments[0]);
		return 1;
	}
	else if(strcmp(command,"rename") == 0)  //rename a file
	{
		scanf("%s",arguments[0]);
		scanf("%s",arguments[1]);
		return 1;
	}
	else if(strcmp(command,"upload") == 0) //upload a file
	{
		scanf("%s,%s",arguments[0],arguments[1]);
		return 1;
	}
	else if(strcmp(command,"download") == 0) // download a file
	{
		scanf("%s,%s",arguments[0],arguments[1]);
		return 1;
	}
	else if(strcmp(command,"exit") == 0) // exit
	{
		return 0;
	}
	else
	{
		return 2;
	}
}
/*************************************************************************
	void response_analysis(char* buffer,char* response_type,char* response_argu)
	func:get information from server retrieved message
*************************************************************************/
void response_analysis(char* buffer,char* response_type,char* response_argu)
{
	int n = 0;
	int i = 0;
	while(buffer[n]!=';'){
		response_type[i] = buffer[n];
		n++;
		i++;
	}
	response_type[i] = '\0';
	n++;
	i=0;
	while(buffer[n]!='|'){
		response_argu[i] = buffer[n];
		n++;
		i++;
	}
	response_argu[i] = '\0';
}
/**********************************************************************************************
	void QueryDirContent(int server_fd,char directory[])
	func:get contends in current directory
***********************************************************************************************/
void QueryDirContent(int server_fd,char directory[])
{
	char response_type[20];
	char response_argu[20];
	int server_data_channel_fd;
	int nbytes;
	int item_count = 0;
	struct file_list * files_head = NULL;
	struct file_list * pfile = NULL;	

	memset(buffer,'\0',BUF_SIZE);			//clear the buffer before use
	sprintf(buffer,"request;querydircontent;%s%s|",current_directory,directory);
	if(send(server_fd,buffer,strlen(buffer),0)==-1)
	{
		printf("send error\n");
	}
	memset(buffer,'\0',BUF_SIZE);			//clear the buffer before use
	recv(server_fd,buffer,BUF_SIZE,0);
	response_analysis(buffer,response_type,response_argu);

	if(strcmp(response_type,"error") == 0)
	{
		    printf("%s:%s\n",response_type,response_argu);
			return;
	}

	server_data_channel_fd = get_data_connection();

	memset(buffer,BUF_SIZE,'\0');		//clear the buffer before use

	printf("type\tname\t\t\t\tsize\n");

 	while((nbytes = recv(server_data_channel_fd,buffer,BUF_SIZE,0)) > 0)
	{
		buffer[nbytes]='\0';
		files_head = get_files_information(buffer);
		pfile = files_head;
		while(pfile->next_file != NULL){
			item_count++;
			pfile = pfile->next_file;
			printf("%s\t%-20s\t\t%-6d\n",pfile->file_type?"dir":"file",pfile->file_name,pfile->file_size);
		}
		memset(buffer,BUF_SIZE,'\0');		//clear the buffer before use
		free_file_list(files_head);
 	}
	printf("total %d items\n",item_count);

	close_data_connection(server_data_channel_fd);
printf("end of query dir \n");
}
/**********************************************************************************************
	struct file_list * get_files_information(char * buffer)
	func:analysis the data in 'buffer',get the files' information ,stored in a file_list struct list to return
**********************************************************************************************/
struct file_list * get_files_information(char * buffer)
{
	char * tem_str = NULL;
	char * save_ptr_1 = NULL;
	char * save_ptr_2 = NULL;
	struct file_list * files_head = NULL;		//point the head of the list
	struct file_list * files_tail = NULL;		//point the tail of the list
	struct file_list * pfile = NULL;			//point the tempalte file_list struct
	files_head = malloc(sizeof(struct file_list));
	files_head->next_file = NULL;
	files_tail = files_head;
	while((tem_str = strtok_r(buffer,"/",&save_ptr_1)) != NULL){
		buffer = NULL;

		pfile = malloc(sizeof(struct file_list));
		memset(pfile,sizeof(struct file_list),'\0');
		pfile->next_file = NULL;

		tem_str = strtok_r(tem_str,"<",&save_ptr_2);		//get the first part in a data unit
		pfile->file_type = tem_str[0]-'0';
		if(strlen(save_ptr_2) > 0){
			strcpy(pfile->file_name,strtok_r(NULL,"<",&save_ptr_2));		//get the second part in a data unit
			if(strlen(save_ptr_2) > 0){
				pfile->file_size = atoi(strtok_r(NULL,"<",&save_ptr_2));		//get the third part in a data unit
			}else{
				free(pfile);
				continue;
			}
		}else{
			free(pfile);
			continue;
		}

		files_tail->next_file = pfile;
		files_tail = files_tail->next_file;
	}
	return files_head;
}
/**********************************************************************************************

**********************************************************************************************/
void free_file_list(struct file_list * file_list_head)
{
	struct file_list * tem = NULL;
	while(file_list_head != NULL){
		tem = file_list_head;
		file_list_head = file_list_head->next_file;
		free(tem);
	}
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
int ChangeDirectory(int server_fd,char new_dir[])
{
	char response_type[20];
	char response_argu[20];

/*	if(strcmp(new_dir,"..")){
		while(strtok_r)
	}
	else if(strcmp(new_dir,".")){
	}*/
	memset(buffer,BUF_SIZE,'\0');		//clear the buffer before use
	sprintf(buffer,"request;changedirectory;%s;%s|",current_directory,new_dir);
	if(send(server_fd,buffer,strlen(buffer),0)==-1)
	{
		printf("send error\n");
		return -1;
	}

	memset(buffer,BUF_SIZE,'\0');		//clear the buffer before use
	recv(server_fd,buffer,BUF_SIZE,0);
	response_analysis(buffer,response_type,response_argu);

	if(strcmp(response_type,"error") == 0)
	{
		printf("the directory don't exist\n");
		return -1;
	}
	strcat(current_directory,new_dir);
	current_directory[strlen(current_directory)] = '/';
	current_directory[strlen(current_directory)+1] = '\0';
	printf("directory changed\n");
	return 1;
}
/*************************************************************************
	int DownloadFile(int server_fd,char* filename)
	func   :  recieve file from server
	return :
*************************************************************************/
int DownloadFile(int server_fd,char* filename)
{
	int bytes_recv = 0;
	char response_type[20];
	char response_argu[20];
	FILE* file_to_recv;
	int server_send_fd;

	memset(buffer,'\0',BUF_SIZE);			//clear the buffer before use
	sprintf(buffer,"request;downloadfile;%s;%s|",current_directory,filename);
	if(send(server_fd,buffer,strlen(buffer),0)==-1)
		printf("GetConnection send package error! \n");

	memset(buffer,'\0',BUF_SIZE);			//clear the buffer before use
	if(recv(server_fd,buffer,BUF_SIZE,0)>0){	
		response_analysis(buffer,response_type,response_argu);
		if(strncmp(response_type,"ack",3)==0){
			printf("file exist now begin to download\n");
			server_send_fd = get_data_connection();		//get data channel connection to server
			if((file_to_recv=fopen(filename,"w"))==NULL){
				printf("can not open the file to recv! \n");
				return -1;
			}
			if((bytes_recv = recv_file(server_send_fd,file_to_recv)) == -1){
				printf("recieve package error \n");
				return -1;
			}
			printf("%d bytes recieved \n",bytes_recv);
			close_data_connection(server_send_fd);		//close data channel connection to server
			return 1;
		}
		else{
			printf("file not exist \n");
			return 0;
		}
	}
	else{
		printf("server has been shutdown\n");
		return -1;
	}
}
/**************************************************************************
	int recv_file(int server_send_fd,FILE* file_to_recv)
	func  : recieve file from 'server_send_fd'
**************************************************************************/
int recv_file(int server_send_fd,FILE* file_to_recv)
{
	int bytes_recv;//the length of character receved
	int bytes_write;
	int bytes_count = 0;
	char buffer[BUF_SIZE];
	bytes_recv=recv(server_send_fd,buffer,BUF_SIZE,0);
	bytes_count += bytes_recv;
	while(bytes_recv>0)
	{
		bytes_write = fwrite(buffer,1,bytes_recv,file_to_recv);
		bytes_recv = recv(server_send_fd,buffer,BUF_SIZE,0);
		bytes_count += bytes_recv;
	}
	fclose(file_to_recv);
	return bytes_count;
}
/**************************************************************************

**************************************************************************/
int UploadFile(int server_fd,char* filename)
{
	char response_type[20];
	char response_argu[20];
	int server_recv_fd;
	FILE* file_to_send;

	if(((file_to_send = fopen(filename,"r")) == NULL)){
		printf("the file you want to upload can not open \n");
		return -1;
	}

	memset(buffer,'\0',BUF_SIZE);			//clear the buffer before use
	sprintf(buffer,"request;uploadfile;%s;%s|",current_directory,filename);
	if(send(server_fd,buffer,strlen(buffer),0)==-1)
		printf("GetConnection send package error! \n");

	memset(buffer,'\0',BUF_SIZE);			//clear the buffer before use
	if(recv(server_fd,buffer,BUF_SIZE,0) > 0){
		response_analysis(buffer,response_type,response_argu);
		if(strncmp(response_type,"ack",3) == 0){
			printf("now begin to upload\n");
			server_recv_fd = get_data_connection();	//get data channel connection to server
			send_file(server_recv_fd,file_to_send);
			fclose(file_to_send);
			close(server_recv_fd);					//close data channel connecion to server
			return 1;
		}
		else{
			printf("the file existed \n");
			return 0;
		}
	}
	else{
		printf("server has been shutdown\n");
		return -1;
	}
}
/**************************************************************************
	
**************************************************************************/
int send_file(int server_recv_fd,FILE* file_to_send)
{
	int bytes_send;		//the length of sended data
	int bytes_read;
	char buffer[BUF_SIZE];
	while(!feof(file_to_send))
	{
		bytes_read = fread(buffer,1,BUF_SIZE,file_to_send);
		bytes_send += send(server_recv_fd,buffer,bytes_read,0);
	}
	printf("%d bytes sended \n",bytes_send);
	return bytes_send;
}
