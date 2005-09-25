/* 
----------------------------------------------------------------------------
action.c
$Id: action.c,v 1.3 2005/09/25 22:17:16 gcasse Exp $
$Author: gcasse $
Description: execute the required action on the supplied buffer.
$Date: 2005/09/25 22:17:16 $ |
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
#include <unistd.h>
#include <stdlib.h>
#include "action.h"
#include "debug.h"
#include "tifilter2l.h"
#include "terminfointerpreter.h"

GByteArray* sayOnlyLinePortionAtCursor( pluginAPI* thePluginAPI, char* theBuffer, int theLength)
{
  GByteArray* aByteArray=NULL;
  termAPI* aTermAPI = NULL;
  cursor aCursor;
  GList* aList = NULL;
  GList* aLinePortionGroup = NULL;
  int aNumberOfLine;
  int aNumberOfCol;

  ENTER("sayOnlyLinePortionAtCursor");

  /* < check consistency */
  if ((thePluginAPI == NULL)
      || (thePluginAPI->myTermAPI == NULL))
    {
      return NULL;
    }
  /* > */
  /* < get cursor from termAPI */
  aTermAPI = thePluginAPI->myTermAPI;

  if( !(aTermAPI->getCursor( &aCursor)))
  {
      return NULL;
  }
  /* > */
  /* < get dim from termapi */
  if ((aTermAPI->getDim == NULL) 
      || !(aTermAPI->getDim( &aNumberOfLine, &aNumberOfCol))
      || (aNumberOfCol == 0)
      || (aCursor.myCol >= aNumberOfCol))
  {
      return NULL;
  }
  /* > */
  /* < get line portion group at cursor from termapi */
  if (aTermAPI->getLinePortionGroup == NULL)
    {
      return NULL;
    }

  aLinePortionGroup = aTermAPI->getLinePortionGroup( aCursor.myLine, 
						     aCursor.myCol, 
						     aNumberOfCol - 1 - aCursor.myCol);
  /* > */
  /* < look for the first non empty lineportion */
  aList = aLinePortionGroup;
  while( aList)
    {
      GString *s = getGStringFromGList( aList);
      
      if (s && 
	  (s->len > strcspn( s->str, " \t\n\r")))
	{
	  break;
	}
      aList = g_list_next( aList);
    }
  /* > */
  /* < mute the buffer and say the lineportion */
  if (aList)
  {
    char* aSequence = NULL;

    aByteArray = muteDisplayedOutput( thePluginAPI, theBuffer, theLength);

    /* the retrieved line portion is added at the end of the buffer.
       Say it but do not display it */

    /* speech yes, screen no */
    aSequence = setRendering( 100, 0);
    g_byte_array_append( (GByteArray*) aByteArray, 
			 (guint8*) aSequence, 
			 (guint) strlen( aSequence));
    free(aSequence);
    
    /* line portion */
    {
      GString *s = getGStringFromGList( aList);
      g_byte_array_append( (GByteArray*) aByteArray, 
			   (guint8*) s->str, 
			   (guint) s->len);
      SHOW2("sayOnlyLinePortionAtCursor: string is \"%s\"\n", s->str);
    }

    /* speech yes, screen yes */
    aSequence = setRendering( 100, 1);
    g_byte_array_append( (GByteArray*) aByteArray, 
			 (guint8*) aSequence, 
			 (guint) strlen( aSequence));
    free(aSequence);
  }
  /* > */

  deleteLinePortionGroup( aLinePortionGroup);

  return aByteArray;
}

GByteArray* tagPreviouslyHighlightedArea( pluginAPI* thePluginAPI, char* theBuffer, int theLength)
{
  GByteArray* aByteArray=NULL;
  GList* aList = NULL;

  ENTER("tagPreviouslyHighlightedArea");

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

GByteArray* updateScreen( pluginAPI* thePluginAPI, char* theBuffer, int theLength)
{
  ENTER("displayOutput");
  (void) write(1, theBuffer, theLength);
  return NULL;
}

/* 
Local variables:
folded-file: t
folding-internal-margins: nil
*/
