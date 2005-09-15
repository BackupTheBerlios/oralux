/* 
----------------------------------------------------------------------------
tengoo.c
$Id: tengoo.c,v 1.1 2005/09/15 22:10:18 gcasse Exp $
$Author: gcasse $
Description: Tengoo api.
$Date: 2005/09/15 22:10:18 $ |
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

#include "tengoo.h"
#include "terminfo2list.h"
#include "terminfointerpreter.h"
#include "tifilter2l.h"

#include "debug.h"

struct tengoo {
  termAPI* myTermAPI;
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
  
  return (void*)this;
}
/* > */
/* < deleteTengoo */
void deleteTengoo(void* theHandle)
{
  tengoo* this = (tengoo*)theHandle;
  ENTER("deleteTengoo");

  deleteTermAPI( this->myTermAPI);
}
/* > */
/* < manageInputTengoo */
GByteArray* manageInputTengoo( void* theHandle, char* theInput, int theLength)
{
  tengoo* this;
  GByteArray* aByteArray=NULL;

  ENTER("manageInputTengoo");

  this = (tengoo*)theHandle;
  return aByteArray;
}
/* > */
/* < manageOutputTengoo */
GByteArray* manageOutputTengoo( void* theHandle, char* theOutput, int theLength)
{
  tengoo* this = (tengoo*)theHandle;
  GByteArray* aByteArray=NULL;
  GList* aList = NULL;

  ENTER("manageOutputTengoo");

  aList = convertTerminfo2List( theOutput, theLength);
  
  SHOW_TIME("terminfointerpreter");
  g_list_foreach(aList, (GFunc)terminfointerpreter, NULL);
  
  SHOW_TIME("terminfofilter2lines");
  aList = terminfofilter2lines( aList, this->myTermAPI, 0);
  
  aByteArray = convertList2Terminfo( aList);
  DISPLAY_RAW_BUFFER( aByteArray->data, aByteArray->len);

  SHOW_TIME("deleteTermInfoList");
  deleteTermInfoList( aList);

  return aByteArray;
}
/* > */

