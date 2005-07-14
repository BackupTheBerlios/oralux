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
};
typedef struct t_terminfoEntry terminfoEntry;

/* > */

GList* convertTerminfo2List( FILE* theStream);
void deleteList(GList* theList);

#endif
