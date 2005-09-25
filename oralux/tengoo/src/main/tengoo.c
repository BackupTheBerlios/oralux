/* 
----------------------------------------------------------------------------
tengoo.c
$Id: tengoo.c,v 1.4 2005/09/25 22:17:16 gcasse Exp $
$Author: gcasse $
Description: Tengoo API.
$Date: 2005/09/25 22:17:16 $ |
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

#include <stdlib.h>

#include "tengoo.h"
#include "termapi.h"
#include "terminfo2list.h"
#include "terminfointerpreter.h"

#include "modeAPI.h"
#include "debug.h"

struct tengoo {
  termAPI* myTermAPI;
  void* myMode;
};
typedef struct tengoo tengoo;

/* < createTengoo */
void* createTengoo( char* theMode, int theInputOutputMaxLength)
{
  ENTER("createTengoo");

  tengoo* this = malloc(sizeof(tengoo));
  cursor aCursor;

  this->myTermAPI = createTermAPI();
  this->myTermAPI->getCursor( &aCursor);
  terminfointerpreter_init( &aCursor);
  initTerminfo2List( &(aCursor.myStyle));

  this->myMode = createModeAPI( theMode, this->myTermAPI, theInputOutputMaxLength);
  
  return (void*)this;
}
/* > */
/* < deleteTengoo */
void deleteTengoo(void* theHandle)
{
  tengoo* this = (tengoo*)theHandle;
  ENTER("deleteTengoo");

  deleteModeAPI( this->myMode);
  deleteTermAPI( this->myTermAPI);
}
/* > */
/* < transcodeInputTengoo */
GByteArray* transcodeInputTengoo( void* theHandle, char* theInput, int theLength)
{
  tengoo* this = (tengoo*)theHandle;
  GByteArray* aByteArray = NULL;

  ENTER("transcodeInputTengoo");

  aByteArray = transcodeInputModeAPI( this->myMode, theInput, theLength);
  if (aByteArray)
    {
      SHOW3("transcodeInputTengoo : aByteArray=%x length=%d\n", (int)aByteArray, ((aByteArray) ? aByteArray->len : 0));
    }
  return aByteArray;
}
/* > */
/* < transcodeOutputTengoo */
GByteArray* transcodeOutputTengoo( void* theHandle, char* theOutput, int theLength)
{
  tengoo* this = (tengoo*)theHandle;
  GByteArray* aByteArray = NULL;

  ENTER("transcodeOutputTengoo");
  SHOW3("**** theLength=%d, theOutput>>\n%s\n", theLength, theOutput);

  aByteArray = transcodeOutputModeAPI( this->myMode, theOutput, theLength);
  
  if (aByteArray)
    {
      SHOW3("transcodeOutputTengoo : aByteArray=%x length=%d\n", (int)aByteArray, ((aByteArray) ? aByteArray->len : 0));
    }
  return aByteArray;
}
/* > */

/* 
Local variables:
folded-file: t
folding-internal-margins: nil
*/
