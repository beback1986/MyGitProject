CC = gcc
server_function.o:server_header.h
server.o:server_header.h
server:server_function.o server.o
	$(CC) -Wall $^ -lpthread -o $@
clean:
	rm *.o
