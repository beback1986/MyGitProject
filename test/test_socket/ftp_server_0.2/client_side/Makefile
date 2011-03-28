CC = gcc
client_function.o:client_header.h
client.o:client_header.h
client:client_function.o client.o
	$(CC) -Wall $^ -o $@
clean:
	rm *.o

client_function_g.o:client_header.h
	$(CC) -g -c -Wall -rdynamic client_function.c -o client_function_g.o
client_g.o:client_header.h
	$(CC) -g -c -Wall -rdynamic client.c -o client_g.o
client_g:client_g.o client_function_g.o
	$(CC) -g -Wall -rdynamic client_g.o client_function_g.o -o client_g
