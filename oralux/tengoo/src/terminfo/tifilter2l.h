#ifndef TIFILTER_H
#define TIFILTER_H

#include "terminfo2list.h"

/* 
   Look for two line portions with style changes (previously highlighted area and newly highlighted area)  
theTermAPI supplies several handlers to collect the terminal info.
*/

int terminfofilter2lines(GList* theTerminfoList, termAPI* theTermAPI, GList** thePreviouslyHighlightedElement, GList** theNewlyHighlightedElement);

#endif
