#ifndef TENGOO_H
#define TENGOO_H

#include <glib.h>

/* create the tengoo handle related to the mode (e.g. "links2") */
void* createTengoo( char* theMode, int theInputOutputMaxLength);

void deleteTengoo( void* theHandle);

/* return the new input (must be freed by the caller) */
GByteArray* transcodeInputTengoo( void* theHandle, char* theInput, int theLength);

/* return the new output (must be freed by the caller) */
GByteArray* transcodeOutputTengoo( void* theHandle, char* theOutput, int theLength);

#endif
