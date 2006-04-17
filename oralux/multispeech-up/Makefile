# Makefile for multispeech-up, multispeechd

#CFLAGS = -ggdb -DPIDPATH=\"@pidpath@\"
CFLAGS = -DPIDPATH=\"\"

all: multispeech-up multispeechd

install: all
	install -m 755 multispeech-up multispeechd multispeech-up-ctl /usr/local/bin

multispeech-up: multispeech-up.o options.o spd.o
multispeechd: multispeechd.o

clean:
	rm -f multispeech-up multispeechd *.o

.o:
	gcc -s -o $@ $^ 
#	gcc -ggdb -o $@ $^


# Dependencies
multispeechd.o: multispeechd.h
spd.o: multispeechd.h
multispeechd.o: multispeechd.h
options.o: options.h