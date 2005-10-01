/* 
----------------------------------------------------------------------------
tifilter2l.c
$Id: tigetline.c,v 1.2 2005/10/01 14:41:15 gcasse Exp $
$Author: gcasse $
Description: terminfo filter, retreive one line.
$Date: 2005/10/01 14:41:15 $ |
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
#include <glib.h>
#include "escape2terminfo.h"
#include "debug.h"
#include "tigetline.h"

/* < terminfoGetLineAtCursor */

GList* terminfoGetLinePortionAtCursor(GList* theTerminfoList, cursor* theCursor)
{
  terminfoEntry* aSelectedElement = NULL;
  GList* anElement = theTerminfoList;

  ENTER("terminfoGetLinePortionAtCursor");

  if ((anElement == NULL)
      || (theCursor == NULL))
    {
      return NULL;
    }

  while( anElement)
    {
      terminfoEntry* anEntry = NULL;
      GString* aString;
      cursor* aCursor;
      anEntry = (terminfoEntry*)(anElement->data);

      if ((anEntry == NULL) 
	  || (anEntry->myCapacity != TEXTFIELD))
	{
	  anElement = g_list_next( anElement);
	  continue;
	}
      
      aCursor = &(anEntry->myStartingPosition);
      aString = (GString*)(anEntry->myData1);

      if((aCursor->myLine == theCursor->myLine)
	 && (aCursor->myCol >= theCursor->myCol)
	 && aString
	 && aString->str
	 && (aString->len > strspn( aString->str, " \t\n\r"))
	 && ((aSelectedElement == NULL)
	     || (aSelectedElement->myStartingPosition.myCol > anEntry->myStartingPosition.myCol)))
	{
	  SHOW4("Last selected string='%s', line=%d, col=%d\n", aString->str, aCursor->myLine, aCursor->myCol);
	  aSelectedElement = anEntry;	     
	}
      anElement = g_list_next( anElement);
    }

  anElement = (aSelectedElement) ? aSelectedElement->myParent : NULL;
  
  SHOW2("selected element=%x\n", (int)anElement);
  
  return anElement;
}

/* > */

/* 
Local variables:
folded-file: t
folding-internal-margins: nil
*/
