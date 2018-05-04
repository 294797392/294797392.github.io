make	: lib
	cc -o make lib.o main.c
lib		: lib.h lib.c
	cc -c lib.h lib.c
clean	:
	rm -rf lib.h.gch lib.o make
