#tcldtk updated for cicero
#
#CFLAGS=-O3 -fpic -pedantic -ansi -Wall -I/usr/include/tcl8.3 -I/usr/include/tclX8.3 -march=i386
CFLAGS=-g -fpic -pedantic -ansi -Wall -I/usr/include/tcl8.3 -I/usr/include/tclX8.3 -march=i386
CPPFLAGS=-DSTDC_HEADERS=1 -Dclock_t=long -DRETSIGTYPE=void 

INSTALL=install
CC=gcc
LIBS=-ltclx8.3
SERVER_DIR=/usr/share/emacs/site-lisp/emacspeak/servers/cicero-api
FILES=tclcicero.c tclcicero.so tclcicero.o Makefile

tclcicero.so: tclcicero.o
	$(CC) $< $(LIBS)  -shared -o $@

clean:
	rm tclcicero.so tclcicero.o

install:
	$(INSTALL) -d $(SERVER_DIR)
	cp  $(FILES) $(SERVER_DIR)
