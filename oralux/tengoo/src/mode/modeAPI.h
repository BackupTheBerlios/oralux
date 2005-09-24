#ifndef MODEAPI
#define MODEAPI
#include <glib.h>
#include "termapi.h"

void* createModeAPI( char* theName, termAPI* theTermAPI, int theInputOutputMaxLength);
void deleteModeAPI( void* theMode);
GByteArray* transcodeInputModeAPI( void* theMode, char* theBuffer, int theLength);
GByteArray* transcodeOutputModeAPI( void* theMode, char* theBuffer, int theLength);

#endif
