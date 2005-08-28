#ifndef TERMINFO2LIST_H
#define TERMINFO2LIST_H

#include <glib.h>
#include "termapi.h"

/* < terminfoEntry */

struct t_terminfoEntry
{
  enum StringCapacity myCapacity; /* capacity */
  void* myData1; /* data depend on capacity */
  void* myData2; 
  chartype* myEscapeSequence; /* the original escape sequence */
  cursor myStartingPosition;
  style myStyle;
};
typedef struct t_terminfoEntry terminfoEntry;

/* > */

void initTerminfo2List( style* theDefaultStyle);
GList* convertTerminfo2List( FILE* theStream);
GByteArray* convertList2Terminfo( GList* theList);
void deleteTermInfoList( GList* theList);
GList* copyTerminfoList( GList* theList);

#endif
