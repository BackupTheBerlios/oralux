# Makefile for the lliaphon client/dameon

all: lliaphon-client lliaphond

install: all
	install -m 555 lliaphond lliaphon-client lliaphonctl /usr/local/bin

clean:
	rm -f lliaphon-client lliaphond

.o:
	gcc -s -o $@ $^ 

# Dependencies
lliaphon-client.o: lliaphond.h
lliaphond.o: lliaphond.h
