#ifndef FRAME_H
#define FRAME_H
#include "box.h"
#include "escape2terminfo.h"

/* < struct frame */
struct frame
{
  int myIdentifier;
  char* myName;
  box* myBox;
};
typedef struct frame frame;
/* > */

/* < createFrame */
frame* createFrame( int theIdentifier, char* theName, point* theOrigin, int theXLength, int theYLength);
/* > */
/* < copyFrame */
frame* copyFrame( frame* theSource);
/* > */
/* < deleteFrame */
void deleteFrame( frame** theFrame);
/* > */

/* 
Local variables:
folded-file: t
folding-internal-margins: nil
*/

#endif
