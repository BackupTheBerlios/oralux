#ifndef BOX_H
#define BOX_H
#include <stdlib.h>
#include "point.h"
struct box
{
  point myOrigin;
  point myCorner;
};
typedef struct box box;

/* the supplied pointers are not stored (the data are copied) */
box* createBox( point* theOrigin, int theXLength, int theYLength);
box* copyBox( box* theSource);
#define deleteBox(this) if (this){free(this);}
int isSameBox( box* theBox1, box* theBox2);
/* #define isSameBox( theBox1, theBox2) (isSamePoint(&(theBox1->myOrigin), &(theBox2->myOrigin)) && isSamePoint(&(theBox1->myCorner), &(theBox2->myCorner))) */
enum intersectionType
  {
    includedBox, 
    intersectionBox, 
    noIntersectionBox,
  };

enum intersectionType isIncluded( box* this, box* thePossibleContainer, box* theIntersectionBox);

/* 
Local variables:
folded-file: t
folding-internal-margins: nil
*/

#endif
