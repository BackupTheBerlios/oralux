#ifndef DOCAPI_H
#define DOCAPI_H
#include <glib.h>
#include "lineportion.h"
#include "frame.h"

enum elementType 
  {
    anyType = 0,
    rootType=1,
    textType=2,
    linkType=4,
    frameType=8,
  };

void* createDocAPI( int theVoiceVolume);
void deleteDocAPI( void* theDocAPI);

/* < addFrameDocAPI */
/* add a Frame style to the document  */
void addFrameStyleDocAPI( void* theDocAPI, GList* theFrame);
/* > */

/* < putListEntryDocAPI */
/* put a list of terminfoEntry in the document.
Each text entry will be associated to a text element (linked to the containing frame). 
If an entry belongs to two or more frames, it will be splitted so that each resulting entry is included in one frame.

The supplied list will be deleted by docAPI.
*/
void putListEntryDocAPI( void* theDocAPI, GList* theList);
/* > */

/* < setElementTypeDocAPI */
/*
  Must be called after putListEntryDocAPI to enrich the type of an entry 
*/
void setElementTypeDocAPI( void* theDocAPI, void* theNode, enum elementType theType);
/* > */

/* < setFocusDocAPI... */
/*
  Must be called after putListEntryDocAPI to enrich the type of an entry 
  setFocusDocAPI: 
*/
void hoverNodeDocAPI( void* theNode);
void activeNodeDocAPI( void* theNode);
/* > */

/* < getListEntryDocAPI */
/*
returned the list in taking in account the styles.
*/
GList* getStyledListEntryDocAPI( void* theDocAPI);
/* > */

/* < loadStyle */
void loadStyle( void* theDocAPI, char* theFilename);
/* > */

/* < clearContent */
/*
clear any textual info (text and link nodes, entry list,...).
The layout is not concerned (frame nodes,...).   
*/
void clearContent( void* theDocAPI);
/* > */


/* 
Local variables:
folded-file: t
folding-internal-margins: nil
*/

#endif
