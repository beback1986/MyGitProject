CFLAGS=-g -O0 -Wall

LD_FLAGS=-lpthread

all: dev_event.o test.o
	gcc ${CFLAGS} ${LD_FLAGS} test.o dev_event.o

clean:
	rm dev_event.o test.o a.out -f
