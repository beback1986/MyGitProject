#include "server_header.h"

/***********************************************************************************************/
extern void session(struct sockaddr_sockfd* client_addr_fd);
/***********************************************************************************************/
int main(int argc,char * argv[])
{
	pthread_t thread_fd;

	struct sockaddr_sockfd client_addr_fd;

	int server_fd,client_fd,server_len,client_len;
	struct sockaddr_in server_address;
	struct sockaddr_in client_address;

	struct sockaddr * server_address_ptr;
	struct sockaddr * client_address_ptr;
	server_address_ptr=(struct sockaddr*) &server_address;
	server_len=sizeof(server_address);
	client_address_ptr=(struct sockaddr*) &client_address;
	client_len=sizeof(client_address);

	server_fd = socket(AF_INET,SOCK_STREAM,0);
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(CAMD_CHE_PORT);
//	server_address.sin_addr.s_addr = inet_addr(SERVER_IP);
	server_address.sin_addr.s_addr = INADDR_ANY;

	if(argc>=2){
		if(argv[1][strlen(argv[1])-1] != '/')
			sprintf(root_directory,"%s/",argv[1]);
		else
			strcpy(root_directory,argv[1]);
	}
	else{
		printf("root directory must be inputed as the first argument!\n");
		exit(0);
	}
	if(bind(server_fd,server_address_ptr,server_len) == -1){	//bind the address and fd
		printf("bind error! \n");
		return 0;
	}

	if(listen(server_fd,5)==-1)	//listen to the client
		printf("listen error! \n");
	while(1)
	{
		client_fd=accept(server_fd,client_address_ptr,&client_len);	
		client_addr_fd.socket_addr = client_address;
		client_addr_fd.socket_fd = client_fd;
		if(pthread_create(&thread_fd,NULL,(void *)session,(void*)&client_addr_fd)==-1)
		{
			printf("pthread create error! \n");
		}
		
	}
	return 0;
}
