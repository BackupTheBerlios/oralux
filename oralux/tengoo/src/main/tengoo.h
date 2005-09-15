#ifndef TENGOO
#define TENGOO

#include <glib.h>

/* create the tengoo handle related to the mode (e.g. "links2") */
void* createTengoo( char* theMode);

void deleteTengoo( void* theHandle);

/* return the new input (must be freed by the caller) */
GByteArray* manageInputTengoo( void* theHandle, char* theInput, int theLength);

/* return the new output (must be freed by the caller) */
GByteArray* manageOutputTengoo( void* theHandle, char* theOutput, int theLength);

#endif
