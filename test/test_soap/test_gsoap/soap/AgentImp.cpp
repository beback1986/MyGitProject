#include "Agent.h"
#include "Agent.nsmap"

/*
int ns__add(struct soap *add_soap, int num1, int num2, int *sum) 
{ 
	*sum = num1 + num2; 
	return 0; 
} 
*/

int ns__Hello(struct soap *soap,
			  struct ns__Hello_Request *req,
			  struct ns__Hello_Response *res)
{
	return 0;
}
