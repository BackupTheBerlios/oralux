#CFLAGS=-g -I /ramdisk/dtk/include -DDEBUG -march=i386
CFLAGS=-g -I /ramdisk/dtk/include -march=i386
CC=gcc
OBJECTS=main.o constants.o getnchar.o audioUserInterface.o audioFifo.o desktop.o i18n.o menu.o textToSpeech.o braille.o serialPort.o yasr.o

OBJECTS_TEST=main.o constants.o getnchar.o audioUserInterface.o audioFifoFoo.o desktop.o i18n.o menu.o textToSpeech.o braille.o serialPort.o

OBJECTS2=getvoice.o audioUserInterface.o audioFifo.o constants.o 
LIBS2=-L /ramdisk/dtk/lib -ltts -ltts_us -ltts_fr -ltts_gr -ltts_sp

OBJECTS3=minimenu.o audioUserInterfaceFoo.o getnchar.o constants.o 

oralux: $(OBJECTS)
	$(CC) $(OBJECTS) -o oralux

oralux_test: $(OBJECTS_TEST)
	$(CC) $(OBJECTS_TEST) -o oralux_test

getvoice: $(OBJECTS2)
	$(CC) $(OBJECTS2) $(LIBS2) -o getvoice

minimenu: $(OBJECTS3)
	$(CC) $(OBJECTS3) -o minimenu

clean:
	rm oralux $(OBJECTS) getvoice $(OBJECTS2) $(OBJECTS3) $(OBJECTS4)

clean.oralux:
	rm oralux $(OBJECTS)

clean.getvoice:
	rm getvoice $(OBJECTS2)

clean.minimenu:
	rm minimenu $(OBJECTS3)

minimenu.o: constants.h getnchar.h
audioUserInterface.o: audioUserInterface.h constants.h i18n.h messages_ru.koi8-r
audioFifo.o: audioFifo.h constants.h
getnchar.o: getnchar.h audioUserInterface.h
getvoice.o: audioUserInterface.h constants.h constants.h
braille.o: braille.h constants.h
desktop.o: desktop.h constants.h
i18n.o: i18n.h constants.h
constants.o: constants.h
main.o: audioUserInterface.h constants.h getnchar.h i18n.h menu.h yasr.h
menu.o: audioUserInterface.h menu.h constants.h getnchar.h textToSpeech.h menu.h
textToSpeech.o: audioUserInterface.h getnchar.h textToSpeech.h constants.h
yasr.o: yasr.h constants.h textToSpeech.h
