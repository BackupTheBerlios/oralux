CC=gcc 
CFLAGS=-Wall -g -fpic -I /ramdisk/dtk/include -march=i386 -DDEBUG
LIBS=-ltts -ltts_us 
OBJECTS=eterpret.o eparser.o command.o fifo.o dectalkAPI.o helpEmul.o

all: $(OBJECTS)
	$(CC) $(OBJECTS) $(LIBS) -o eterpret

clean:
	$(RM) $(OBJECTS)

eterpret.o: eparser.h helpEmul.h
eparser.o: eparser.h command.h fifo.h synthAPI.h helpEmul.h
command.o: command.h helpEmul.h
fifo.o: fifo.h helpEmul.h
dectalkAPI.o: synthAPI.h helpEmul.h
helpEmul.o: helpEmul.h

