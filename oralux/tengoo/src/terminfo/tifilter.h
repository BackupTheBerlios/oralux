#ifndef TIFILTER_H
#define TIFILTER_H

#include "terminfo2list.h"

/* terminfofilter
This is the generic api.
The input terminfo list is filtered, and the new list is returned.
theTermAPI supplies several handlers to collect terminal info.
*/

GList* terminfofilter(GList* theTerminfoList, termAPI* theTermAPI); 

#endif
