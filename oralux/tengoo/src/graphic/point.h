#ifndef POINT
#define POINT
#include <stdlib.h>
#include <string.h>

struct point 
{
  int x;
  int y;
  int z;
};
typedef struct point point;

point* createPoint( int x, int y, int z);
#define deletePoint(this) if (this){free(this);}
#define copyPoint( theDest, theSource) memcpy(theDest, theSource, sizeof(point))
/* #define isSamePoint( thePoint1, thePoint2) (((thePoint1)->x == (thePoint2)->x) && ((thePoint1)->y == (thePoint2)->y) && ((thePoint1)->z == (thePoint2)->z)) */
int isSamePoint( point* thePoint1, point* thePoint2);

void translatePoint( point* this, int theXLength, int theYLength);



#endif
