#ifndef DOCAPI_H
#define DOCAPI_H
#include <glib.h>
#include "lineportion.h"
#include "frame.h"

/* < docNode */
enum nodeType 
  {
    rootType,
    textType,
    frameType,
  };

struct docNode
{
  enum nodeType myType;
  style* myRoot;
  linePortion* myLinePortion;
  frame* myFrame;
  int* myVoiceVolume;  
};
typedef struct docNode docNode;

docNode* createRootNode( style* theStyle);
docNode* createTextNode( linePortion* theLinePortion);
docNode* createFrameNode( frame* theFrame);
/* > */

GNode* createDocAPI();
void deleteDocAPI( GNode* this);

/* < setVoiceVolumeDocAPI */
/* theValue: 0..100 (0=silence) */
void setVoiceVolumeDocAPI( GNode* this, int theValue);
/* > */

/* 
Local variables:
folded-file: t
folding-internal-margins: nil
*/

#endif
