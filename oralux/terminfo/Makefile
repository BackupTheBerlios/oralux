# Basic Makefile -- relies on implicit rules
# Creates termBuffer for test purposes
#
LEX=flex
CFLAGS=-g -DDEBUG -pedantic -Wall
#CFLAGS=-O3 -pedantic -Wall
termBuffer: escape2terminfo.o termBuffer.o debug.o test.o
escape2terminfo.o: escape2terminfo.l escape2terminfo.h debug.h
termBuffer.o:  escape2terminfo.h debug.h termBuffer.h Makefile
debug.o: debug.h
test.o: termBuffer.h

clean: 
	rm *o termBuffer

