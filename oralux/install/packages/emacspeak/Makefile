# ATTENTION
# This is not the original emacspeak file. Changed for Oralux. 
# Id: Makefile,v 18.0 2003/04/29 21:21:24 raman Exp
# 
LABEL=1.0
DIST=software-dtk-$(LABEL)
CFLAGS=-g -fpic -I /usr/include/tcl8.3 -I /ramdisk/dtk/include -march=i386
INSTALL=install
CC=gcc
LIBS=-L /ramdisk/dtk/lib -ltts -ltts_us -ltcl8.3
SERVER_DIR=/usr/share/emacs/site-lisp/emacspeak/servers/software-dtk
FILES=tcldtk.c tcldtk.so tcldtk.o Makefile

tcldtk.so: tcldtk.o
	$(CC) $< $(LIBS)  -shared -o $@

dist: software-dtk.tar.gz

software-dtk.tar.gz:
	mkdir $(DIST) ;
	cp $(FILES) $(DIST) ;
	tar cfz software-dtk.tar.gz $(DIST)
	/bin/rm -rf $(DIST)

clean:
	rm tcldtk.so tcldtk.o

install:
	$(INSTALL) -d $(SERVER_DIR)
	cp  $(FILES) $(SERVER_DIR)

rpm: software-dtk.spec software-dtk.tar.gz
	@cp software-dtk.tar.gz /usr/src/redhat/SOURCES/
	@cp software-dtk.spec /usr/src/redhat/SPECS/
	rpm  -bb --sign --clean   /usr/src/redhat/SPECS/software-dtk.spec

