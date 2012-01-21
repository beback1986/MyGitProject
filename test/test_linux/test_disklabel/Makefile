CFLAGS=-g -O0 -DVMDIO_LINUX -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE -D_GNU_SOURCE -Wall
LD_CFLAGS=-lrt

all: test.o vmdio-linux.o
	gcc $(CFLAGS) test.o vmdio-linux.o -lrt -o getlabel

clean:
	rm *.o getlabel -rf
