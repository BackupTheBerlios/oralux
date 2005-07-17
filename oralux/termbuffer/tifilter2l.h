#ifndef TIFILTER_H
#define TIFILTER_H

#include "terminfo2list.h"

/* 
The input terminfo list is filtered, and the new list is returned.
theTermAPI supplies several handlers to collect terminal info.
*/

GList* terminfofilter2lines(GList* theTerminfoList, termapi* theTermAPI, int isDuplicated);

#endif
