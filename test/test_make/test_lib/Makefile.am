AUTOMAKE_OPTIONS=foreign
sbin_PROGRAMS=libmysolib.so
libmysolib_so_SOURCES=mysolib.C
libmysolib.so:mysolib.o
gcc -o libmysolib.so -shared mysolib.o
bin_PROGRAMS=test
test_SOURCES=test.c
test_LDFLAGS=-L. -lmysolib
include_HEADERS=mysolib.h
