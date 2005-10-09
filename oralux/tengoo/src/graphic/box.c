/* 
----------------------------------------------------------------------------
box.c
$Id: box.c,v 1.4 2005/10/09 22:43:12 gcasse Exp $
$Author: gcasse $
Description: box.
$Date: 2005/10/09 22:43:12 $ |
$Revision: 1.4 $ |
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

/* < createBox*/
box* createBox( point* theOrigin, int theXLength, int theYLength)
{
  box* this = malloc(sizeof(box));

  ENTER("createBox");

  if (this)
    {
      copyPoint(& this->myOrigin, theOrigin);
      copyPoint(& this->myCorner, theOrigin);
      translatePoint(& this->myCorner, theXLength, theYLength);
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
    }

  return this;
}
/* > */
/* < isIncluded */
enum intersectionType isIncluded( box* this, box* thePossibleContainer, box* theIntersectionBox)
{
  ENTER("isIncluded");

  if (!this || !thePossibleContainer || !theIntersectionBox)
    {
      return noIntersectionBox;
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

    int xi1=theIntersectionBox->myOrigin.x;
    int yi1=theIntersectionBox->myOrigin.y;
    int xi2=theIntersectionBox->myCorner.x;
    int yi2=theIntersectionBox->myCorner.y;

    if ((ya1 < yb2)
	|| (ya2 > yb1)
	|| (xa2 < xb1)
	|| (xa1 > xb2))
      {
	return noIntersectionBox;
      }

    xi1 = MAX( xa1, xb1);
    yi1 = MAX( ya2, yb2);
    xi2 = MIN( xa2, xb2);
    yi2 = MIN( ya1, yb1);
  }
  return isSameBox( this, theIntersectionBox) ? includedBox : intersectionBox;
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
