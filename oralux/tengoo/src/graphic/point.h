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
void translatePoint( point* this, int theXLength, int theYLength);



#endif
