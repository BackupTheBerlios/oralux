/* 
----------------------------------------------------------------------------
point.c
$Id: point.c,v 1.3 2005/10/09 22:43:12 gcasse Exp $
$Author: gcasse $
Description: point.
$Date: 2005/10/09 22:43:12 $ |
$Revision: 1.3 $ |
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
#include "point.h"
#include "debug.h"

point* createPoint( int x, int y, int z)
{
  point* this = NULL;
  
  ENTER("createPoint");

  this = malloc(sizeof(point));
  if (this)
    {
      this->x=x;
      this->y=y;
      this->z=z;
    }
  return this;
}

void translatePoint( point* this, int theXLength, int theYLength)
{
  ENTER("translatePoint");
  this->x += theXLength;
  this->y += theYLength;
}


int isSamePoint( point* thePoint1, point* thePoint2)
{
  return ((thePoint1->x == thePoint2->x) && (thePoint1->y == thePoint2->y) && (thePoint1->z == thePoint2->z));
}
/* 
Local variables:
folded-file: t
folding-internal-margins: nil
*/
