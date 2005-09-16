#ifndef MODE
#define MODE
#include <glib.h>
#include "termapi.h"

void* createMode(char* theName, termAPI* theTermAPI);
GByteArray* manageInputMode( void* theMode, char* theInput, int theLength);
GByteArray* manageOutputMode( void* theMode, char* theOutput, int theLength);
void deleteMode( void* theMode);
#endif
