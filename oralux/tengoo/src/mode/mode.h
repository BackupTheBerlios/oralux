#ifndef MODE
#define MODE
#include <glib.h>
#include "termapi.h"

typedef void* (CREATE_MODE_HANDLER)( char* theName, termAPI* theTermAPI);
typedef void (DELETE_MODE_HANDLER)( void* theMode);
typedef GByteArray* (MANAGE_BUFFER_MODE_HANDLER)( void* theMode, char* theInput, int theLength);

CREATE_MODE_HANDLER createMode;
DELETE_MODE_HANDLER deleteMode;
MANAGE_BUFFER_MODE_HANDLER manageInputMode;
MANAGE_BUFFER_MODE_HANDLER manageOutputMode;

#endif
