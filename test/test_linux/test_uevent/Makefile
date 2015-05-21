CFLAGS=-g -O0 -Wall

LD_FLAGS=-lpthread

all: uevent.o test.o
	gcc ${CFLAGS} ${LD_FLAGS} test.o uevent.o

clean:
	rm uevent.o test.o a.out -f
