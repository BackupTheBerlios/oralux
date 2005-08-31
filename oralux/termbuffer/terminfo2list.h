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
  GList* myParent;
};
typedef struct t_terminfoEntry terminfoEntry;

/* > */

void initTerminfo2List( style* theDefaultStyle);
GList* convertTerminfo2List( FILE* theStream);
GByteArray* convertList2Terminfo( GList* theList);
void deleteTermInfoList( GList* theList);
GList* copyTerminfoList( GList* theList);


/* < Create entry for the private terminfo */

/* addPreviouslyHighlithedItem: 
The TPHL private terminfo is helpful to indicate that a sentence was previously highlighted. Its single parameter equals 0 or 1; 0 for the beginning of the sentnece, and 1 for the end. 
The syntax is :
TPHL P=0, sentence, TPHL P=1 

addPreviouslyHighlithedItem adds the two TPHL element before theFirstElement and after theLastElement.
*/
GList* addPreviouslyHighlithedElement( GList* theFirstElement, GList* theLastElement);

/* > */

#endif
