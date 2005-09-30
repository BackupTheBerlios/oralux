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

/* addSequence: 
The TSAR (Tengoo: Select Aural Rendition) private terminfo is helpful to enrich the escape sequences with aural informations. For example so that the screen reader does not say a part of the displayed text. 
*/
GList* addSequence( chartyp* theFirstSequence, GList* theFirstElement, GList* theLastElement, chartyp* theLastSequence);

/* 
setRendering: returns the TSAR escape sequence.
theVolume: 0..100 (0 = silent)
theVoice: 0..9
the char* returned must be freed by the caller 
*/
char* setRendering( int theVolume, int theVoice);


GList* sayElement( GList* theFirstElement, GList* theLastElement);
GList* muteElement( GList* theFirstElement, GList* theLastElement);

/* > */

#endif
