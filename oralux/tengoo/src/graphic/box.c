/* 
----------------------------------------------------------------------------
box.c
$Id: box.c,v 1.7 2005/10/17 14:12:25 gcasse Exp $
$Author: gcasse $
Description: box.
$Date: 2005/10/17 14:12:25 $ |
$Revision: 1.7 $ |
Copyright (C) 2005 Gilles Casse (gcasse@oralux.org)

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
----------------------------------------------------------------------------
*/
#include "box.h"
#include "debug.h"

#define MIN(a,b) ((a<b)?a:b)
#define MAX(a,b) ((a>b)?a:b)

/* < debug */
#ifdef DEBUG
#define DISPLAY_BOX(b) if(b){SHOW4("Origin: x=%d, y=%d, z=%d\n",b->myOrigin.x,b->myOrigin.y,b->myOrigin.z); SHOW4("Corner: x=%d, y=%d, z=%d\n",b->myCorner.x,b->myCorner.y,b->myCorner.z);}


void displayIntersection( enum intersectionType x)
{
  char* aResult=NULL;
  switch(x)
    {
    case includedBox:
      aResult="includedBox";
      break;
    case intersectionBox:
      aResult="intersectionBox";
      break;
    case noIntersectionBox:
      aResult="noIntersectionBox";
      break;
    default:
      aResult="undefined...";
      break;
    }
  SHOW2("intersection=%s\n", aResult);
}

#define DISPLAY_INTERSECTION(x) displayIntersection(x)
#else
#define DISPLAY_BOX(b)
#define DISPLAY_INTERSECTION(x)
#endif
/* > */
/* < deleteBox */
void deleteBox(box** theBox)
{
  ENTER("deleteBox");

  if (theBox && *theBox)
    {
      SHOW2("Old box:%x\n",(int)*theBox);
      free(*theBox);
      *theBox=NULL;
    }
}
/* > */
/* < createBox*/
box* createBox( point* theOrigin, int theXLength, int theYLength)
{
  box* this = malloc(sizeof(box));

  ENTER("createBox");

  if (this && theOrigin && (theXLength > 0) && (theYLength > 0))
    {
      copyPoint(& this->myOrigin, theOrigin);
      copyPoint(& this->myCorner, theOrigin);
      translatePoint( &(this->myCorner), 
		     theXLength - 1, 
		     1 - theYLength);
      SHOW2("New box:%x\n",(int)this);
      DISPLAY_BOX(this);
    }
  return this;
}
/* > */
/* < copyBox*/
box* copyBox( box* theSource)
{
  box* this = NULL;

  ENTER("copyBox");

  if (!theSource)
    {
      return NULL;
    }

  this = malloc(sizeof(box));

  if (this)
    {
      memcpy( this, theSource, sizeof(box));
      SHOW2("New box:%x\n",(int)this);
      DISPLAY_BOX(this);
    }

  return this;
}
/* > */
/* < isIncluded */
enum intersectionType isIncludedBox( box* this, box* thePossibleContainer, box* theIntersectionBox)
{
  enum intersectionType aResult=noIntersectionBox;

  ENTER("isIncludedBox");

  SHOW2("*** this=%x\n", (int)this);
  DISPLAY_BOX(this);
  SHOW2("*** thePossibleContainer=%x\n", (int)thePossibleContainer);
  DISPLAY_BOX(thePossibleContainer);

  if (!this || !thePossibleContainer || !theIntersectionBox)
    {
      DISPLAY_INTERSECTION(aResult);
      return aResult;
    }

  {
    int xa1=this->myOrigin.x;
    int ya1=this->myOrigin.y;
    int xa2=this->myCorner.x;
    int ya2=this->myCorner.y;

    int xb1=thePossibleContainer->myOrigin.x;
    int yb1=thePossibleContainer->myOrigin.y;
    int xb2=thePossibleContainer->myCorner.x;
    int yb2=thePossibleContainer->myCorner.y;

    int* xi1=&(theIntersectionBox->myOrigin.x);
    int* yi1=&(theIntersectionBox->myOrigin.y);
    int* xi2=&(theIntersectionBox->myCorner.x);
    int* yi2=&(theIntersectionBox->myCorner.y);

    if ((ya1 < yb2)
	|| (ya2 > yb1)
	|| (xa2 < xb1)
	|| (xa1 > xb2))
      {
	DISPLAY_INTERSECTION(aResult);
	return noIntersectionBox;
      }

    *xi1 = MAX( xa1, xb1);
    *yi1 = MAX( ya2, yb2);
    *xi2 = MIN( xa2, xb2);
    *yi2 = MIN( ya1, yb1);

    DISPLAY_BOX( this);
  }
  aResult = isSameBox( this, theIntersectionBox) ? includedBox : intersectionBox;
  DISPLAY_INTERSECTION(aResult);
  return aResult;
}
/* > */

int isSameBox( box* theBox1, box* theBox2) 
{
  return (isSamePoint(&(theBox1->myOrigin), &(theBox2->myOrigin)) 
    && isSamePoint(&(theBox1->myCorner), &(theBox2->myCorner)));
}

/* 
Local variables:
folded-file: t
folding-internal-margins: nil
*/
