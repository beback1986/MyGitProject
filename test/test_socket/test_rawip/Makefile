CFLAGS=-g -O0

default: tcp_checksum.o test.o
	gcc ${CFLAGS} tcp_checksum.o test.o

receiver:
	gcc ${CFLAGS} rawip_receiver.c

clean:
	rm *.o a.out
