#ifndef BOX
#define BOX
#include <stdlib.h>
#include "point.h"
struct box
{
  point myOrigin;
  point myCorner;
};
typedef struct box box;

box* createBox( point* theOrigin, int theXLength, int theYLength);
#define deleteBox(this) if (this){free(this);}


#endif
