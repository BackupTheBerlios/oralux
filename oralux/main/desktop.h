#ifndef DESKTOP_H
#define DESKTOP_H 1

#include "constants.h"

enum desktopIdentifier
  {
    Emacspeak,
    Yasr,
    MaxDesktop,
  };

enum desktopIdentifier getEnumDesktop(char* theDesktop);
char* desktopGetString( enum desktopIdentifier theValue);
enum sentence desktopGetSentence( enum desktopIdentifier theValue);
void setDesktop(enum desktopIdentifier *theDesktop);

#endif
