LD_CFLAGS=-lrt
TMP_CFLAGS=-DVMDIO_LINUX
INCLUDES=-I../include/
CFLAGS=-g -O0 -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE -D_GNU_SOURCE -Wall $(INCLUDES) ${TMP_CFLAGS}

all: test.o vmdio-linux.o
	gcc $(CFLAGS) $(LD_CFLAGS) test.o vmdio-linux.o -o getlabel

clean:
	rm *.o getlabel -rf
