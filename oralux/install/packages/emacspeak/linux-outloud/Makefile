#$Id: Makefile,v 1.1 2004/12/16 23:14:43 gcasse Exp $
CC=g++
INSTALL = install
PREFIX = /usr
libparentdir = ${PREFIX}/share/emacs/site-lisp
libdir =$(libparentdir)/emacspeak/linux-outloud
CFLAGS=-g -fpic  -pedantic -ansi -Wall -I /opt/IBM/ibmtts/inc 
TTS=tcleci.so eci.ini VIAVOICE

all: tcleci.so 

CPPFLAGS=  -DUNIX -fpic  -g -I/usr/include/tcl8.3 -I/usr/include/tclX8.3

tcleci.so: tcleci.o
	$(CC) $(CFLAGS) -g -o $@ tcleci.o -ltclx8.3 -ldl -shared

clean:
	rm -f tcleci.so tcleci.o

install: tcleci.so
	if test ! -d $(libdir) ; then mkdir $(libdir); fi ; \
	chmod 755 $(libdir); \
	$(INSTALL) -m 0644 $(TTS) $(libdir)
