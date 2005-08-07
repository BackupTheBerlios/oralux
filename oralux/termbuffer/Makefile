# Makefile for libtermbuffer
#

CC = gcc

# Where "make install" puts libtermbuffer.so* and termbuffer.h
prefix=/usr/local

LEX = flex
#CFLAGS = -pedantic -fPIC -Wall -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include
#CFLAGS = -g -DDEBUG -pedantic -fPIC -Wall -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include
CFLAGS = -g -pedantic -fPIC -Wall -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include

#LDFLAGS=-L. -Wl,-rpath,. -ltermbuffer
LDFLAGS=-lglib-2.0
LDFLAGS3=$(LDFLAGS) -lutil

TBMAJ = 0
TBMIN = 1
TBVER = $(TBMAJ).$(TBMIN)

INCPATH=$(prefix)/include
LIBPATH=$(prefix)/lib

OBJS = termbuffer.o escape2terminfo.o linuxconsole.o debug.o test-tb.o
OBJS2 = tifilter2l.o terminfo2list.o escape2terminfo.o linuxconsole.o lineportion.o terminfointerpreter.o debug.o termapiVCSA.o
OBJS_TI= test-t2l.o $(OBJS2)
OBJS3 = test-tengoo.o $(OBJS2)

all: terminfo2list tengoo
#all: libtermbuffer.so test-tb

terminfo2list: $(OBJS_TI)
	$(CC) -o terminfo2list $(CFLAGS) $(OBJS_TI) $(LDFLAGS)

tengoo: $(OBJS3)
	$(CC)  $(LDFLAGS3) -o tengoo $(CFLAGS) $(OBJS3)

#	$(CC) -o tengoo $(CFLAGS) $(OBJS3) $(LDFLAGS3)

# RAF
# libtermbuffer.so: libtermbuffer.so.$(TBMAJ)
# 	ln -sf libtermbuffer.so.$(TBMAJ) libtermbuffer.so

# libtermbuffer.so.$(TBMAJ): libtermbuffer.so.$(TBVER)
# 	ln -sf libtermbuffer.so.$(TBVER) libtermbuffer.so.$(TBMAJ)

# libtermbuffer.so.$(TBVER): $(OBJS)
# 	$(CC) -g -shared -Wl,-soname,libtermbuffer.so.$(TBMAJ) -o libtermbuffer.so.$(TBVER) $(OBJS) -lc

# test-tb: test-tb.o libtermbuffer.so
# 	$(CC) -o test-tb $(CFLAGS) test-tb.o $(LDFLAGS)

# test: test-tb
# 	./test-tb

# install: libtermbuffer.so.$(TBVER)
# 	-@if [ ! -d $(INCPATH)  ]; then mkdir $(INCPATH); fi
# 	-@if [ ! -d $(LIBPATH) ]; then mkdir $(LIBPATH); fi
# 	cp termbuffer.h $(INCPATH)
# 	chmod 644 $(INCPATH)/termbuffer.h
# 	cp libtermbuffer.so.$(TBVER) $(LIBPATH)
# 	chmod 755 $(LIBPATH)/libtermbuffer.so.$(TBVER)
# 	-@/bin/rm -f $(LIBPATH)/libtermbuffer.so.$(TBMAJ) $(LIBPATH)/libtermbuffer.so
# 	(cd $(LIBPATH); ln -sf libtermbuffer.so.$(TBVER) libtermbuffer.so.$(TBMAJ); \
#  	ln -sf libtermbuffer.so.$(TBMAJ) libtermbuffer.so; ldconfig -n .)

clean:
	/bin/rm -f *.o terminfo2list
#	/bin/rm -f *.o libtermbuffer.so libtermbuffer.so.$(TBMAJ)* test-tb

# dependencies
tifilter2l.o: tifilter2l.h escape2terminfo.h debug.h Makefile
terminfo2list.o: escape2terminfo.h debug.h terminfo2list.h
terminfointerpreter.o: debug.h terminfointerpreter.h
termapi.o: termapi.h escape2terminfo.h lineportion.h debug.h
escape2terminfo.o: escape2terminfo.l escape2terminfo.h debug.h
linuxconsole.o: escape2terminfo.h
lineportion.o: escape2terminfo.h lineportion.h
debug.o: debug.h
termapiVCSA.o: termapi.h debug.h
test-t2l.o: tifilter2l.h escape2terminfo.h debug.h terminfo2list.h

#test-tb.o: termbuffer.h
#termbuffer.o: escape2terminfo.h debug.h termbuffer.h Makefile



