/* < license */
/* 
----------------------------------------------------------------------------
action.c
$Id: action.c,v 1.11 2006/01/14 01:10:09 gcasse Exp $
$Author: gcasse $
Description: execute the required action on the supplied buffer.
$Date: 2006/01/14 01:10:09 $ |
$Revision: 1.11 $ |
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
/* > */
/* < include */
#include <unistd.h>
#include <stdlib.h>
#include "action.h"
#include "debug.h"
#include "docAPI.h"
#include "tifilter2l.h"
#include "tigetline.h"
#include "terminfointerpreter.h"
/* > */
/* < sayOnlyLinePortionAtCursor */
GByteArray* sayOnlyLinePortionAtCursor( pluginAPI* thePluginAPI, char* theBuffer, int theLength)
{
  GByteArray* aByteArray=NULL;
  GList* aList = NULL;

  ENTER("sayOnlyLinePortionAtCursor");

  aList = convertTerminfo2List( theBuffer, theLength);
  if (aList == NULL)
    {
      return NULL;
    }
  g_list_foreach( aList, (GFunc)terminfointerpreter, NULL);

  appendListEntryDocAPI( thePluginAPI->myDocument, aList);
  
  /* < get the last cursor position */
  {
    GList* aSelectedElement = NULL;
    cursor* aCursor = terminfointerpreter_getCursor();

    if (aCursor)
      {
	/* get the selected line portion */
	aSelectedElement = terminfoGetLinePortionAtCursor( aList, aCursor);
	if (aSelectedElement && aSelectedElement->data)
	  {
	    setElementTypeDocAPI( thePluginAPI->myDocument, aSelectedElement, linkType);
	    setFocusStateDocAPI( aSelectedElement, hoveredElement);
	  }
      }
  }
  /* > */

  aList = getStyledListEntryDocAPI( thePluginAPI->myDocument);
  aByteArray = convertList2Terminfo( aList);
  DISPLAY_RAW_BUFFER( aByteArray->data, aByteArray->len);

  SHOW_TIME("deleteTermInfoList");
  deleteTermInfoList( aList);
  clearContentDocAPI( thePluginAPI->myDocument);

  return aByteArray;
}
/* > */
/* < mutePreviouslyHighlightedArea */
GByteArray* mutePreviouslyHighlightedArea( pluginAPI* thePluginAPI, char* theBuffer, int theLength)
{
  GByteArray* aByteArray=NULL;
  GList* aList = NULL;
  cursor aFirstCursor;
  cursor aLastCursor;

  ENTER("mutePreviouslyHighlightedArea");

  aList = convertTerminfo2List( theBuffer, theLength);
  if (aList == NULL)
    {
      return NULL;
    }
  copyCursor(&aFirstCursor, terminfointerpreter_getCursor()); /* TBD: malloc, no static cursor */
  g_list_foreach(aList, (GFunc)terminfointerpreter, NULL);
  
  copyCursor(&aLastCursor, terminfointerpreter_getCursor()); /* TBD: malloc, no static cursor */

  if(terminfoExpandText( &aList, thePluginAPI->myTermAPI, &aFirstCursor, &aLastCursor))
    {
      appendListEntryDocAPI( thePluginAPI->myDocument, aList);
    }
  else 
    {
      GList* aPreviouslyHighlightedElement  = NULL; 
      GList* aNewlyHighlightedElement = NULL;

      appendListEntryDocAPI( thePluginAPI->myDocument, aList);

      if (terminfofilter2lines( aList, thePluginAPI->myTermAPI, &aPreviouslyHighlightedElement, &aNewlyHighlightedElement))
	{
	  setElementTypeDocAPI( thePluginAPI->myDocument, 
				aPreviouslyHighlightedElement, 
				linkType);
	  setElementTypeDocAPI( thePluginAPI->myDocument, 
				aNewlyHighlightedElement, 
				linkType);
	  setFocusStateDocAPI( aNewlyHighlightedElement, hoveredElement);
	  aList = getStyledListEntryDocAPI( thePluginAPI->myDocument);
	}
    }

  aByteArray = convertList2Terminfo( aList);
  DISPLAY_RAW_BUFFER( aByteArray->data, aByteArray->len);

  deleteTermInfoList( aList);
  clearContentDocAPI( thePluginAPI->myDocument);

  return aByteArray;
}
/* > */
/* < muteDisplayedOutput */
GByteArray* muteDisplayedOutput( pluginAPI* thePluginAPI, char* theBuffer, int theLength)
{
/*   char* aSequence = NULL; */
/*   GByteArray* aByteArray=NULL; */
/*   ENTER("muteDisplayedOutput"); */

/*   aSequence = setRendering( 0, 1); */
/*   aByteArray = g_byte_array_new(); */

/*   g_byte_array_append( (GByteArray*) aByteArray,  */
/* 		       (guint8*) aSequence,  */
/* 		       (guint) strlen(aSequence)); */

/*   g_byte_array_append( (GByteArray*) aByteArray,  */
/* 		       (guint8*) theBuffer,  */
/* 		       (guint) theLength); */

/*   free( aSequence); */

/*  return aByteArray; */

  return NULL;
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
