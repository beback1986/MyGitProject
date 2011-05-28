OBJECTS=soapC.o AgentImp.o

default:${OBJECTS}
	ar rcs soap.a soapC.o AgentImp.o

soap:
	soapcpp2 -c -S -L Agent.h

soapC.o: soap
	g++ ${CPPFLAGS} -c soapC.c -o soapC.o

AgentImp.o: soap
	g++ ${CPPFLAGS} -c AgentImp.cpp -o AgentImp.o

clean:
	rm -f *.nsmap *.xsd *.xml *.wsdl *.a *.o soapH.h soapStub.h soapC.c  soapServer.c
