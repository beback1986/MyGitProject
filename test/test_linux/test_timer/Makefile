INCLUDES=-I../include/
CFLAGS=-g -O0 -Wall ${INCLUDES}

LD_FLAGS=-lpthread -lrt

all: timer_lin.o test.o
	gcc ${CFLAGS} ${LD_FLAGS} test.o timer_lin.o

clean:
	rm timer_lin.o test.o a.out -f
