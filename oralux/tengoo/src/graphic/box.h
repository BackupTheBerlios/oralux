#ifndef BOX_H
#define BOX_H
#include <stdlib.h>
#include "point.h"
struct box
{
  point myOrigin; /* bottom left (lowest x, highest y) */
  point myCorner; /* top right (highest x, lowest y) */
};
typedef struct box box;

/* the supplied pointers are not stored (the data are copied) */
/* < createBox */
/*
theOrigin: bottom left point
theXlength : horizontal length of the box 
theYlength : verticla length

Length 1= smallest 'box' (just one point).
*/
box* createBox( point* theOrigin, int theXLength, int theYLength);
/* > */
box* copyBox( box* theSource);
void deleteBox(box** theBox);
int isSameBox( box* theBox1, box* theBox2);
/* #define isSameBox( theBox1, theBox2) (isSamePoint(&(theBox1->myOrigin), &(theBox2->myOrigin)) && isSamePoint(&(theBox1->myCorner), &(theBox2->myCorner))) */
enum intersectionType
  {
    includedBox, 
    intersectionBox, 
    noIntersectionBox,
  };

enum intersectionType isIncludedBox( box* this, box* thePossibleContainer, box* theIntersectionBox);

/* 
Local variables:
folded-file: t
folding-internal-margins: nil
*/

#endif
