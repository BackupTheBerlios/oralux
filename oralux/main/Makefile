BIN=$(DESTDIR)/usr/bin
SHARE=$(DESTDIR)/usr/share/oralux
DTK=$(DESTDIR)/ramdisk/dtk

#CFLAGS=-g -I $(DTK)/include -DDEBUG -march=i386
CFLAGS=-g -I $(DTK)/include -march=i386
CC=gcc
OBJECTS=main.o constants.o getnchar.o audioUserInterface.o audioFifo.o desktop.o i18n.o menu.o textToSpeech.o braille.o serialPort.o yasr.o speakup.o

OBJECTS_TEST=main.o constants.o getnchar.o audioUserInterface.o audioFifoFoo.o desktop.o i18n.o menu.o textToSpeech.o braille.o serialPort.o

OBJECTS2=getvoice.o audioUserInterface.o audioFifo.o constants.o 
LIBS2=-L $(DTK)/lib -ltts -ltts_us -ltts_fr -ltts_gr -ltts_sp

OBJECTSOG=constants.o getnchar.o audioUserInterface.o audioFifo.o desktop.o i18n.o menu.o textToSpeech.o braille.o serialPort.o yasr.o speakup.o

oralux-target: $(OBJECTS)
	$(CC) $(OBJECTS) -o oralux

oralux_test: $(OBJECTS_TEST)
	$(CC) $(OBJECTS_TEST) -o oralux_test

getvoice: $(OBJECTS2)
	$(CC) $(OBJECTS2) $(LIBS2) -o getvoice

clean:
	rm $(OBJECTS) getvoice $(OBJECTS2)

clean.oralux:
	rm $(OBJECTS)

clean.getvoice:
	rm getvoice $(OBJECTS2)

install:	oralux-target
		install -d $(BIN) $(SHARE)
		install -m 544 oralux *.php *.sh $(SHARE)
		mv $(SHARE)/oralux.sh $(BIN)
		mv $(SHARE)/oralux $(BIN)

audioUserInterface.o: audioUserInterface.h constants.h i18n.h messages_ru.koi8-r
audioFifo.o: audioFifo.h constants.h
getnchar.o: getnchar.h audioUserInterface.h
getvoice.o: audioUserInterface.h constants.h constants.h
braille.o: braille.h constants.h
desktop.o: desktop.h constants.h
i18n.o: i18n.h constants.h
constants.o: constants.h
main.o: audioUserInterface.h constants.h getnchar.h i18n.h menu.h yasr.h speakup.h
menu.o: audioUserInterface.h menu.h constants.h getnchar.h textToSpeech.h menu.h
textToSpeech.o: audioUserInterface.h getnchar.h textToSpeech.h constants.h
yasr.o: yasr.h constants.h textToSpeech.h
speakup.o: speakup.h constants.h textToSpeech.h menu.h
