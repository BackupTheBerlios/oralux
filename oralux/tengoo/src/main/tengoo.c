/* 
----------------------------------------------------------------------------
tengoo.c
$Id: tengoo.c,v 1.2 2005/09/16 21:53:36 gcasse Exp $
$Author: gcasse $
Description: Tengoo API.
$Date: 2005/09/16 21:53:36 $ |
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

#include <stdlib.h>

#include "tengoo.h"
#include "termapi.h"
#include "terminfo2list.h"
#include "terminfointerpreter.h"

#include "mode.h"
#include "debug.h"

struct tengoo {
  termAPI* myTermAPI;
  void* myMode;
};
typedef struct tengoo tengoo;

/* < createTengoo */
void* createTengoo( char* theMode)
{
  ENTER("createTengoo");

  tengoo* this = malloc(sizeof(tengoo));
  cursor aCursor;

  this->myTermAPI = createTermAPI();
  this->myTermAPI->getCursor( &aCursor);
  terminfointerpreter_init( &aCursor);
  initTerminfo2List( &(aCursor.myStyle));

  this->myMode = createMode( theMode, this->myTermAPI);
  
  return (void*)this;
}
/* > */
/* < deleteTengoo */
void deleteTengoo(void* theHandle)
{
  tengoo* this = (tengoo*)theHandle;
  ENTER("deleteTengoo");

  deleteMode( this->myMode);
  deleteTermAPI( this->myTermAPI);
}
/* > */
/* < manageInputTengoo */
GByteArray* manageInputTengoo( void* theHandle, char* theInput, int theLength)
{
  tengoo* this = (tengoo*)theHandle;

  ENTER("manageOutputTengoo");

  return manageInputMode( this->myMode, theInput, theLength);
}
/* > */
/* < manageOutputTengoo */
GByteArray* manageOutputTengoo( void* theHandle, char* theOutput, int theLength)
{
  tengoo* this = (tengoo*)theHandle;

  ENTER("manageOutputTengoo");

  return manageOutputMode( this->myMode, theOutput, theLength);
}
/* > */

