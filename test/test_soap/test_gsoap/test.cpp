#include "add.h"
#include "add.nsmap"

int main(int argc, char* argv[])
{
	int m, s; /**//* master and slave sockets */
	struct soap sop;
	soap_init(&sop);
	
	if (argc < 2) {
		printf("usage:%s <server_port>\n", argv[0]);
		goto failed;
	}

	m = soap_bind(&sop, NULL, atoi(argv[1]), 100);
	if (m < 0) {
		soap_print_fault(&sop, stderr);
		goto failed;
	}
	fprintf(stderr, "Socket connection successful: master socket = %d\n", m);

	for ( ; ; ) {
		s = soap_accept(&sop);
		if (s < 0) {
			soap_print_fault(&sop, stderr);
			goto failed;
		}
		fprintf(stderr, "Socket connection successful: slave socket = %d\n", s);
		soap_serve(&sop);
		soap_end(&sop);
	}
	return 0;

failed:
	return 1;
}
