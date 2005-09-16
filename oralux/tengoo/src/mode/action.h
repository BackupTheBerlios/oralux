#ifndef ACTION
#define ACTION

#include <glib.h>
#include "termapi.h"

typedef GByteArray* (actionCallBack)( char* theBuffer, int theLength, termAPI* theTermAPI);

actionCallBack tagPreviouslyHighlightedArea;
actionCallBack purgeOutput;

#endif
