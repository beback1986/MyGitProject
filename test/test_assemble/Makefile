print: clean_print
	gcc -g -c print.s -o print.o
	ld print.o -o print

clean_print:
	rm -f print.o
	rm -f print

hello: clean_hello
	gcc -g -c hello.s -o hello.o
	ld hello.o -o hello

clean_hello:
	rm -f hello.o
	rm -f hello

all:
	echo "no target"

