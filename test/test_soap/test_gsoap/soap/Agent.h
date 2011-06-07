//gsoap ns service name: Agent
//gsoap ns service namespace: http://localhost/Agent.wsdl 
//gsoap ns service location: http://localhost/Agent.cgi
//gsoap ns service executable: Agent.cgi 
//gsoap ns service encoding: encoded 
//gsoap ns schema namespace: urn:Agent

typedef char * xsd__string;

struct ns__Hello_Request {
	xsd__string word;
};

struct ns__Hello_Response {
	xsd__string word;
};

int ns__Hello(struct ns__Hello_Request *req,
			  struct ns__Hello_Response *res);
