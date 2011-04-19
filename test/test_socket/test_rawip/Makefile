CFLAGS=-g -O0 -I ./includes/

obj   := task.o mempool.o dump.o \
	 uprotocol.o uskbuff.o ureceiver.o usender.o uip.o utcp.o \
	 test.o

default: ${obj}
	gcc -lpthread ${CFLAGS} ${obj}

clean:
	rm *.o a.out -f
