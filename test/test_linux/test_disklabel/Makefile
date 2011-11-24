CFLAGS=-DVMDIO_LINUX -Wall
LD_CFLAGS=-lrt

all: test.o vmdio-linux.o
	gcc $(CFLAGS) test.o vmdio-linux.o -lrt -o getlabel

clean:
	rm *.o getlabel -rf
