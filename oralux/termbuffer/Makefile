# Makefile for libtermbuffer
#

CC = gcc

# Where "make install" puts libtermbuffer.so* and termbuffer.h
prefix=/usr/local

LEX = flex
#CFLAGS = -g -DDEBUG -pedantic -fPIC -Wall
CFLAGS = -g -pedantic -fPIC -Wall
LDFLAGS=-L. -Wl,-rpath,. -ltermbuffer

TBMAJ = 0
TBMIN = 1
TBVER = $(TBMAJ).$(TBMIN)

INCPATH=$(prefix)/include
LIBPATH=$(prefix)/lib

OBJS = termbuffer.o escape2terminfo.o linuxconsole.o debug.o test-tb.o

all: libtermbuffer.so test-tb

libtermbuffer.so: libtermbuffer.so.$(TBMAJ)
	ln -sf libtermbuffer.so.$(TBMAJ) libtermbuffer.so

libtermbuffer.so.$(TBMAJ): libtermbuffer.so.$(TBVER)
	ln -sf libtermbuffer.so.$(TBVER) libtermbuffer.so.$(TBMAJ)

libtermbuffer.so.$(TBVER): $(OBJS)
	$(CC) -g -shared -Wl,-soname,libtermbuffer.so.$(TBMAJ) -o libtermbuffer.so.$(TBVER) $(OBJS) -lc

test-tb: test-tb.o libtermbuffer.so
	$(CC) -o test-tb $(CFLAGS) test-tb.o $(LDFLAGS)

test: test-tb
	./test-tb

install: libtermbuffer.so.$(TBVER)
	-@if [ ! -d $(INCPATH)  ]; then mkdir $(INCPATH); fi
	-@if [ ! -d $(LIBPATH) ]; then mkdir $(LIBPATH); fi
	cp termbuffer.h $(INCPATH)
	chmod 644 $(INCPATH)/termbuffer.h
	cp libtermbuffer.so.$(TBVER) $(LIBPATH)
	chmod 755 $(LIBPATH)/libtermbuffer.so.$(TBVER)
	-@/bin/rm -f $(LIBPATH)/libtermbuffer.so.$(TBMAJ) $(LIBPATH)/libtermbuffer.so
	(cd $(LIBPATH); ln -sf libtermbuffer.so.$(TBVER) libtermbuffer.so.$(TBMAJ); \
	ln -sf libtermbuffer.so.$(TBMAJ) libtermbuffer.so)

clean:
	/bin/rm -f *.o libtermbuffer.so libtermbuffer.so.$(TBMAJ)* test-tb

# dependencies
termbuffer.o: escape2terminfo.h debug.h termbuffer.h Makefile
escape2terminfo.o: escape2terminfo.l escape2terminfo.h debug.h
linuxconsole.o: escape2terminfo.h
debug.o: debug.h
test-tb.o: termbuffer.h



