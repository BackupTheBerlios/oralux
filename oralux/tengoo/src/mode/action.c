/* 
----------------------------------------------------------------------------
action.c
$Id: action.c,v 1.5 2005/10/01 14:41:15 gcasse Exp $
$Author: gcasse $
Description: execute the required action on the supplied buffer.
$Date: 2005/10/01 14:41:15 $ |
$Revision: 1.5 $ |
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
#include <unistd.h>
#include <stdlib.h>
#include "action.h"
#include "debug.h"
#include "tifilter2l.h"
#include "tigetline.h"
#include "terminfointerpreter.h"
/* > */
/* < sayOnlyLinePortionAtCursor */
GByteArray* sayOnlyLinePortionAtCursor( pluginAPI* thePluginAPI, char* theBuffer, int theLength)
{
  GByteArray* aByteArray=NULL;
  GList* aFirstElement = NULL;
  GList* aLastElement = NULL;
  GList* aSelectedElement = NULL;

  ENTER("sayOnlyLinePortionAtCursor");

  aFirstElement = convertTerminfo2List( theBuffer, theLength);

  if (aFirstElement == NULL)
    {
      return NULL;
    }

  aLastElement = g_list_last( aFirstElement);
  
  g_list_foreach( aFirstElement, (GFunc)terminfointerpreter, NULL);
  
  /* < get the last cursor position */
  {
    cursor* aCursor = terminfointerpreter_getCursor();

    if (aCursor)
      {
	/* get the selected line portion */
	aSelectedElement = terminfoGetLinePortionAtCursor( aFirstElement, aCursor);
      }
  }
  /* > */

  /* Firstly, the whole list is mute */
  aFirstElement = muteElement( aFirstElement, aLastElement);

  /* then the selected element can be said */
  aFirstElement = sayElement( aSelectedElement, aSelectedElement);

  aByteArray = convertList2Terminfo( aFirstElement);
  DISPLAY_RAW_BUFFER( aByteArray->data, aByteArray->len);

  SHOW_TIME("deleteTermInfoList");
  deleteTermInfoList( aFirstElement);

  return aByteArray;
}
/* > */
/* < mutePreviouslyHighlightedArea */
GByteArray* mutePreviouslyHighlightedArea( pluginAPI* thePluginAPI, char* theBuffer, int theLength)
{
  GByteArray* aByteArray=NULL;
  GList* aList = NULL;

  ENTER("mutePreviouslyHighlightedArea");

  aList = convertTerminfo2List( theBuffer, theLength);
  
  SHOW_TIME("terminfointerpreter");
  g_list_foreach(aList, (GFunc)terminfointerpreter, NULL);
  
  SHOW_TIME("terminfofilter2lines");
  {
    aList = terminfofilter2lines( aList, thePluginAPI->myTermAPI, 0);
  }

  aByteArray = convertList2Terminfo( aList);
  DISPLAY_RAW_BUFFER( aByteArray->data, aByteArray->len);

  SHOW_TIME("deleteTermInfoList");
  deleteTermInfoList( aList);

  return aByteArray;
}
/* > */
/* < muteDisplayedOutput */
GByteArray* muteDisplayedOutput( pluginAPI* thePluginAPI, char* theBuffer, int theLength)
{
  char* aSequence = NULL;
  GByteArray* aByteArray=NULL;
  ENTER("muteDisplayedOutput");

  aSequence = setRendering( 0, 1);
  aByteArray = g_byte_array_new();

  g_byte_array_append( (GByteArray*) aByteArray, 
		       (guint8*) aSequence, 
		       (guint) strlen(aSequence));

  g_byte_array_append( (GByteArray*) aByteArray, 
		       (guint8*) theBuffer, 
		       (guint) theLength);

  free( aSequence);

  return aByteArray;
}
/* > */
/* < appendBuffer */
GByteArray* appendBuffer( pluginAPI* thePluginAPI, char* theOutput, int theLength)
{
  ENTER("appendBuffer");

  if ((theOutput == NULL) || (theLength == 0))
    {
      return NULL;
    }

  if (thePluginAPI->myBuffer == NULL)
    {
      thePluginAPI->myBuffer = g_byte_array_new();
    }

  g_byte_array_append( thePluginAPI->myBuffer, 
		       (guint8*) theOutput, 
		       (guint) theLength);

  return NULL;
}
/* > */
/* < updateScreen */
GByteArray* updateScreen( pluginAPI* thePluginAPI, char* theBuffer, int theLength)
{
  ENTER("displayOutput");
  (void) write(1, theBuffer, theLength);
  return NULL;
}
/* > */

/* 
Local variables:
folded-file: t
folding-internal-margins: nil
*/
