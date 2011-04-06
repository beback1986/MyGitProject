CFLAGS=-g -O0 -I ./includes/

default: tcp_checksum.o test.o dump.o
	gcc ${CFLAGS} tcp_checksum.o test.o

receiver: rawip_receiver.o uskbuff.o dump.o
	gcc ${CFLAGS} uskbuff.o rawip_receiver.o dump.o

clean:
	rm *.o a.out
