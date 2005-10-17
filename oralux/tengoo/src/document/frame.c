/* 
----------------------------------------------------------------------------
frame.c
$Id: frame.c,v 1.4 2005/10/17 14:12:25 gcasse Exp $
$Author: gcasse $
Description: manage frames, screen divisions.
$Date: 2005/10/17 14:12:25 $ |
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

/* < include */
#include "frame.h"
#include "debug.h"
/* > */
/* < deleteFrame */
void deleteFrame( frame** theFrame)
{
  ENTER("deleteFrame");

  if (theFrame && *theFrame)
    {
      frame* this = *theFrame;
      SHOW2("Old frame=%x\n", (int)this);
      free( this->myName);
      deleteBox( &(this->myBox));
      free( this);
      *theFrame = NULL;
    }
}
/* > */
/* < createFrame */
frame* createFrame( int theIdentifier, char* theName, point* theOrigin, int theXLength, int theYLength)
{
  frame* this = NULL;

  ENTER("createFrame");

  if (!theName || !theOrigin || (theXLength <= 0) || (theYLength <= 0))
    {
      return NULL;
    }

  this = malloc(sizeof(frame));

  if (this)
    {
      this->myIdentifier = theIdentifier;
      this->myName = strdup(theName);
      this->myBox = createBox( theOrigin, theXLength, theYLength);
      SHOW5("New frame %x, identifier=%d, name=%s, box=%x\n",
	    (int)this,
	    this->myIdentifier, this->myName, (int)(this->myBox));
      if (!this->myName || !this->myBox)
	{
	  deleteFrame( &this);
	}
    }

  return this;
}
/* > */
/* < copyFrame */
frame* copyFrame( frame* theSource)
{
  frame* this = NULL;

  ENTER("copyFrame");

  if (!theSource)
    {
      return NULL;
    }

  this = malloc(sizeof(frame));

  if (this)
    {
      SHOW2("New frame=%x\n", (int)this);
      this->myIdentifier = theSource->myIdentifier;
      this->myName = strdup(theSource->myName);
      this->myBox = copyBox( theSource->myBox);

      if (!this->myName || !this->myBox)
	{
	  deleteFrame( &this);
	}
    }

  return this;
}
/* > */


/* 
Local variables:
folded-file: t
folding-internal-margins: nil
*/


