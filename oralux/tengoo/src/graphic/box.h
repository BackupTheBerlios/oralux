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

box* createBox( point* theOrigin, int theXLength, int theYLength);
box* copyBox( box* theSource);
#define deleteBox(this) if (this){free(this);}

/* 
Local variables:
folded-file: t
folding-internal-margins: nil
*/

#endif
