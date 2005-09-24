#ifndef FRAME
#define FRAME
#include "box.h"
#include "auralStyle.h"
#include "escape2terminfo.h"

struct frame
{
  char* myName;
  box myBox;
  style myVisualStyle;
  auralStyle myAuralStyle;
};



#endif
