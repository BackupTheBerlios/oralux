/* 
----------------------------------------------------------------------------
frame.c
$Id: frame.c,v 1.2 2005/10/02 20:28:33 gcasse Exp $
$Author: gcasse $
Description: manage frames, screen divisions.
$Date: 2005/10/02 20:28:33 $ |
$Revision: 1.2 $ |
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
void deleteFrame( frame* this)
{
  ENTER("deleteFrame");

  if (this)
    {
      free( this->myName);
      deleteBox( this->myBox);
      free( this);
    }
}
/* > */
/* < createFrame */
frame* createFrame( int theIdentifier, char* theName, point* theOrigin, int theXLength, int theYLength)
{
  frame* this = NULL;

  ENTER("createFrame");

  if (!theName || !theOrigin)
    {
      return NULL;
    }

  this = malloc(sizeof(frame));

  if (this)
    {
      this->myIdentifier = theIdentifier;
      this->myName = strdup(theName);
      this->myBox = createBox( theOrigin, theXLength, theYLength);

      if (!this->myName || !this->myBox)
	{
	  deleteFrame(this);
	  this = NULL;
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
      this->myIdentifier = theSource->myIdentifier;
      this->myName = strdup(theSource->myName);
      this->myBox = copyBox( theSource->myBox);

      if (!this->myName || !this->myBox)
	{
	  deleteFrame(this);
	  this = NULL;
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


