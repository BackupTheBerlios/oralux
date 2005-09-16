/* 
----------------------------------------------------------------------------
mode.c
$Id: mode.c,v 1.1 2005/09/16 21:53:36 gcasse Exp $
$Author: gcasse $
Description: Mode API.
$Date: 2005/09/16 21:53:36 $ |
$Revision: 1.1 $ |
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

#include <stdlib.h>
#include <string.h>
#include "action.h"
#include "debug.h"
#include "mode.h"

/* < type */
struct mode
{
  int myState;
  actionCallBack* myOutputIntermediaryBlockCallback;
  actionCallBack* myOutputLastBlockCallback;
  termAPI* myTermAPI; 
};
typedef struct mode mode;
/* > */


/* < createMode */
void* createMode( char* theName, termAPI* theTermAPI)
{
  mode* this=NULL;

  ENTER("createMode");

  this = malloc(sizeof(mode));
  memset(this, 0, sizeof(mode));
  this->myTermAPI = theTermAPI;

  return (void*)this;
}
/* > */
/* < deleteMode */
void deleteMode( void* theMode)
{
  ENTER("deleteMode");
  free((mode*)theMode);
}
/* > */
/* <manageInputMode */
GByteArray* manageInputMode( void* theMode, char* theInput, int theLength)
{
  GByteArray* aByteArray = NULL;
  ENTER("manageInputMode");
  return aByteArray;
}
/* > */
/* < manageOutputMode */
GByteArray* manageOutputMode( void* theMode, char* theOutput, int theLength)
{
  GByteArray* aByteArray = NULL;
  mode* this = (mode*)theMode;

  ENTER("manageOutputMode");

  aByteArray = tagPreviouslyHighlightedArea( theOutput, theLength, this->myTermAPI); 

  return aByteArray;
}
/* > */
/* < convertInputToCommandMode */
void convertInputToCommandMode( void* theMode, const char* theInput, char* theCommand)
{
}

/* > */
