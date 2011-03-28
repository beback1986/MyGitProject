#include <selinux/selinux.h>
#include <stdio.h>

void main(void)
{
    security_context_t scon = "unconfined_u:unconfined_r:unconfined_t:s0-s0:c0.c1023";
	char * user = "root";
	security_context_t * con = NULL;
	int ret = -1;
	ret = security_compute_user(scon,user,&con);
	if(!ret){
		security_context_t *ptr;
		for(con;*con;con++){
			printf("%s\n",*con);
		}
/*		for (ptr = *con; *ptr; ptr++) {
			printf("%s\n",ptr);
		}*/
	}
	else{
		printf("can not get the cons\n");
	}
	return ;
}
