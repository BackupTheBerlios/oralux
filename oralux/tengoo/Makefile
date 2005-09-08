# Makefile for libtengoo
#
AR = ar
CC = gcc

# For "make install" 
prefix=/usr/local

LEX = flex
#CFLAGS = -pedantic -fPIC -Wall -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include
CFLAGS = -g -DDEBUG -fPIC -Wall -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include
#CFLAGS = -g -fPIC -Wall -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include


LDFLAGS=-lglib-2.0 -lutil

INCPATH=$(prefix)/include
LIBPATH=$(prefix)/lib

OBJS = tifilter2l.o terminfo2list.o escape2terminfo.o linuxconsole.o lineportion.o terminfointerpreter.o debug.o termapiVCSA.o linuxscreen.o
OBJS2 = test-tengoo.o
OBJS3 = test-tengoo.o $(OBJS)

all: libtengoo.a tengoo 

tengoo: $(OBJS2) libtengoo.a
	$(CC) $(CFLAGS) test-tengoo.o -ltengoo $(LDFLAGS) -o tengoo  
#	$(CC) -ltengoo -o tengoo $(CFLAGS) $(OBJS2)

#tengoo: $(OBJS3)
#	$(CC)  $(LDFLAGS) -o tengoo $(CFLAGS) $(OBJS3)

libtengoo.a: $(OBJS)
	$(AR) rcs libtengoo.a $(OBJS)

clean:
	/bin/rm -f *.o libtengoo.a tengoo

# dependencies
tifilter2l.o: tifilter2l.h escape2terminfo.h debug.h
terminfo2list.o: escape2terminfo.h debug.h terminfo2list.h
terminfointerpreter.o: debug.h terminfointerpreter.h
termapi.o: termapi.h escape2terminfo.h lineportion.h debug.h
escape2terminfo.o: escape2terminfo.l escape2terminfo.h debug.h
linuxconsole.o: escape2terminfo.h
lineportion.o: lineportion.h
debug.o: debug.h
termapiVCSA.o: termapi.h debug.h




