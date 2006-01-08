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
  gpointer myNode; /* reserved for DocAPI, logical node associated with this entry(frame, anchor,...) */
};
typedef struct t_terminfoEntry terminfoEntry;

/* > */

void initTerminfo2List( style* theDefaultStyle);
/* GList* convertTerminfo2List( FILE* theStream); */
GList* convertTerminfo2List( char* theBuffer, int theLength);
GByteArray* convertList2Terminfo( GList* theList);
void deleteTermInfoList( GList* theList);
GList* copyTerminfoList( GList* theList);


#define getNodeFromList( theList)   ((theList && theList->data) ? (((terminfoEntry*)(theList->data))->myNode) : NULL)

/* < get_TSAR_Sequence : Create entry for the private terminfo */

/* 
The TSAR (Tengoo: Select Aural Rendition) private terminfo is helpful to enrich the escape sequences with aural informations. For example so that the screen reader does not say a part of the displayed text. 

get_TSAR_Sequence: returns a terminfoEntry containing a TSAR escape sequence.

theVolume: 0..100 (0 = silent)
theVoice: 0..9
the terminfoEntry* returned must be freed by the caller 
*/
terminfoEntry* get_TSAR_Sequence( int theVolume, int theVoice);
/* > */

/* < getting other terminfoEntries */
terminfoEntry* getTextEntry( char* theText);
terminfoEntry* getPositionEntry( int theLine, int theCol);
terminfoEntry* getStyleEntry( style* theStyle);
/* > */


/* 
Local variables:
folded-file: t
folding-internal-margins: nil
*/

#endif
