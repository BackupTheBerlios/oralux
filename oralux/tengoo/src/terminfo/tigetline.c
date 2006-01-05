/* 
----------------------------------------------------------------------------
tifilter2l.c
$Id: tigetline.c,v 1.3 2006/01/05 23:30:46 gcasse Exp $
$Author: gcasse $
Description: terminfo filter, retreive one line.
$Date: 2006/01/05 23:30:46 $ |
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
#include <stdlib.h>
#include <glib.h>
#include "escape2terminfo.h"
#include "debug.h"
#include "tigetline.h"

/* < terminfoGetLinePortionAtCursor */

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
/* < context */
struct t_context
{
  cursor myCursor;
  cursor mySavedCursor;
  int myNumberOfLine;
  int myNumberOfCol;
  termAPI* myTermAPI;
  int myTextIsFound;
};
typedef struct t_context context;

static context* createContext(termAPI* theTermAPI)
{
  int i;
  context* this = (context*) malloc(sizeof(context));

  ENTER("createContext");

  theTermAPI->getCursor( &(this->myCursor));
  theTermAPI->getDim( &(this->myNumberOfLine), &(this->myNumberOfCol));
  this->myTermAPI = theTermAPI;
  this->myTextIsFound = 0;

cursor* terminfointerpreter_getCursor()

  return this;
}

static void deleteContext(context* this)
{ 
  ENTER("deleteContext");
  free(this);
}
/* > */
/* < searchText */
static void searchText(gpointer theEntry, gpointer theContext)
{
  terminfoEntry* anEntry = (terminfoEntry*)theEntry;
  context* this = (context*)theContext;
  GString* aText = NULL;

  ENTER("searchText");

  if ((anEntry == NULL) 
      || (anEntry->myCapacity != TEXTFIELD))
    {
      return;
    }

  this->myTextIsFound = 1;

/*   aText = anEntry->myData1; */
/*   if(aText && aText->str)      */
/*     { */
/*       cursor* c =  &(anEntry->myStartingPosition); */
/*     } */

}
/* > */

/* < terminfoExpandText */
/* TBD:  taking in account frame */
GList* terminfoExpandText(GList* theTerminfoList, termAPI* theTermAPI, cursor* theCursor)
{
  int aResult=0;
  cursor* aPreviousCursor = NULL;
  int aPreviousCell;
  int aNextCell;

  ENTER("terminfoExpandText");

  if (!theTerminfoList || !*theTerminfoList || !theTermAPI)
    {
      return 0;
    }

  this = createContext( theTermAPI);
  if (!this)
    {
      return 0;
    }

  g_list_foreach( theTerminfoList, (GFunc)searchText, this);

  if (!this->myTextIsFound)
    {
      goto exitExpand;
    }

  aResult = 1;

  /* identifying the type of displacement */
  /* TBD: initial implementation. The command must be taken in account */

  aPreviousCursor = & (this->myCursor);

  aPreviousCell = aPreviousCursor->myCol + this->myNumberOfCol * aPreviousCursor->myLine;

  aCell = theCursor->myCol + myNumberOfCol * theCursor->myLine;

  /* Current word must be announced */

  if (aPreviousCursor->myLine == theCursor->myLine)
    {
      if (theCursor->myCol == 0)
	{

	  goto  exitExpand;
	}
      else if (isEOL( myTermAPI, theCursor->myCol)
	{

	  goto  exitExpand;
	}
    }

  if (countChar( myTermAPI, aPreviousCursor, theCursor) == 1)
    {
      getChar( myTermAPI, theCursor);
    }
  else if (countWord( myTermAPI, aPreviousCursor, theCursor)==1)
    {
      getWord( myTermAPI, theCursor);
    }
  else
    {
      getLine
    }


     
myCursor.myCol
  



 exitExpand:
  deleteContext( this);
  return aResult;
}

/* > */

/* 
Local variables:
folded-file: t
folding-internal-margins: nil
*/
