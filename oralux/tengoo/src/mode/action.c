/* 
----------------------------------------------------------------------------
action.c
$Id: action.c,v 1.1 2005/09/16 21:53:36 gcasse Exp $
$Author: gcasse $
Description: execute the required action on the supplied buffer.
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
#include "action.h"
#include "debug.h"
#include "tifilter2l.h"
#include "terminfointerpreter.h"

GByteArray* tagPreviouslyHighlightedArea( char* theBuffer, int theLength, termAPI* theTermAPI)
{
  GByteArray* aByteArray=NULL;
  GList* aList = NULL;

  ENTER("tagPreviouslyHighlightedArea");

  aList = convertTerminfo2List( theBuffer, theLength);
  
  SHOW_TIME("terminfointerpreter");
  g_list_foreach(aList, (GFunc)terminfointerpreter, NULL);
  
  SHOW_TIME("terminfofilter2lines");
  aList = terminfofilter2lines( aList, theTermAPI, 0);
  
  aByteArray = convertList2Terminfo( aList);
  DISPLAY_RAW_BUFFER( aByteArray->data, aByteArray->len);

  SHOW_TIME("deleteTermInfoList");
  deleteTermInfoList( aList);

  return aByteArray;
}

GByteArray* purgeOutput( char* theBuffer, int theLength, termAPI* theTermAPI)
{
  ENTER("purgeOutput");
  return NULL;
}
