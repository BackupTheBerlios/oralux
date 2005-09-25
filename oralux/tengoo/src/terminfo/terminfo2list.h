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
  chartyp* myEscapeSequence; /* the original escape sequence */
  cursor myStartingPosition;
  style myStyle;
  GList* myParent;
};
typedef struct t_terminfoEntry terminfoEntry;

/* > */

void initTerminfo2List( style* theDefaultStyle);
/* GList* convertTerminfo2List( FILE* theStream); */
GList* convertTerminfo2List( char* theBuffer, int theLength);
GByteArray* convertList2Terminfo( GList* theList);
void deleteTermInfoList( GList* theList);
GList* copyTerminfoList( GList* theList);


/* < Create entry for the private terminfo */

/* muteDisplayedElement: 
The TSAR (Tengoo: Select Aural Rendition) private terminfo is helpful to enrich the escape sequences with aural informations. For example so that the screen reader does not say a part of the displayed text. 

muteDisplayedElement adds a TSAR element (volume=0) before theFirstElement and after theLastElement (volume=100).
*/
GList* muteDisplayedElement( GList* theFirstElement, GList* theLastElement);

/* 
setRendering: returns the escape sequence for rendering a text via speech or screen.
theVolumeProsody: 0..100 (0 = silent)
theTextIsDisplayed: 0 or 1 (true)
the char* returned must be freed by the caller 
*/
char* setRendering( int theVolumeProsody, int theTextIsDisplayed );

/* > */

#endif
