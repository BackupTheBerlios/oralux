#ifndef AUDIOFIFO
#define AUDIOFIFO 1

void audioFifoInit();
// theDoubleIsFiltered
// if 1: if the two same files are supplied, just one will be played 
void playOggVorbisFile( char* theFilename, int theDoubleIsFiltered, int theCallIsSynchronous);
void clearAll();
void resayAll();
void audioFifoQuit(int theSoundMustBeFinished);


#endif

