# Basic Makefile -- relies on implicit rules
# Creates terminfo from "escape2terminfo.l" and "terminfo.c"
#
LEX=flex
CFLAGS=-g -DDEBUG -O3 -fpic -pedantic -Wall
terminfo: escape2terminfo.o terminfo.o
escape2terminfo.o: escape2terminfo.l escape2terminfo.h
terminfo.o:  escape2terminfo.h
