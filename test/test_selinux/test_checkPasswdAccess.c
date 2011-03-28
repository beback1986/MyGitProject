#include <selinux/selinux.h>
#include <selinux/av_permissions.h>
#include <stdio.h>

void main(void)
{
	access_vector_t requested = PACKET_SOCKET__IOCTL;
	int ret = -1;

	ret = checkPasswdAccess(requested);
	if(!ret){
		printf("the request access privicy is granted\n");
	}
	return ;
}
