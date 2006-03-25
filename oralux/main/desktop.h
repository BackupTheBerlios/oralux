#ifndef DESKTOP_H
#define DESKTOP_H 1

#include "constants.h"

enum desktopIdentifier
  {
    Emacspeak=1,
    Yasr=2,
    Speakup=4,
  };

enum desktopIdentifier getEnumDesktop(char* theDesktop);
char* desktopGetString( enum desktopIdentifier theValue);
enum sentence desktopGetSentence( enum desktopIdentifier theValue);
int setDesktop(enum desktopIdentifier *theDesktop);

#endif
