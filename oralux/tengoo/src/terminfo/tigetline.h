#ifndef TIGETLINE_H
#define TIGETLINE_H

#include "terminfo2list.h"

/* 
Search the not blanked line portion after the cursor.
The related list element is returned or NULL otherwise.
theTermAPI supplies several handlers to collect terminal info.
*/

GList* terminfoGetLineAtCursor(GList* theTerminfoList, cursor* theCursor);

#endif
