/* 
----------------------------------------------------------------------------
tifilter2l.c
$Id: tigetline.c,v 1.7 2006/01/11 22:19:54 gcasse Exp $
$Author: gcasse $
Description: terminfo filter, retreive one line.
$Date: 2006/01/11 22:19:54 $ |
$Revision: 1.7 $ |
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
#include "termapi.h"
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
  int myNumberOfLine;
  int myNumberOfCol;
  termAPI* myTermAPI;
  GList** myTerminfoList;
  int myTextIsFound;
};
typedef struct t_context context;

static context* createContext(termAPI* theTermAPI, GList** theTerminfoList)
{
  context* this = (context*) malloc(sizeof(context));

  ENTER("createContext");

  theTermAPI->getCursor( &(this->myCursor));
  theTermAPI->getDim( &(this->myNumberOfLine), &(this->myNumberOfCol));
  this->myTermAPI = theTermAPI;
  this->myTerminfoList = theTerminfoList;
  this->myTextIsFound = 0;

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
/* < appendList */
static void appendList(gpointer theLinePortion, gpointer theContext)
{
  terminfoEntry* anEntry = NULL;
  context* this = (context*)theContext;
  linePortion* aLinePortion = (linePortion*)theLinePortion;

  ENTER("appendList");

  if (!aLinePortion || !this || !this->myTerminfoList || !*this->myTerminfoList)
    {
      return;
    }

  /* < add terminfoEntry: position */
  anEntry = getPositionEntry( aLinePortion->myLine, aLinePortion->myFirstCol);

  if(!anEntry)
    {
      return;
    }

  *this->myTerminfoList = g_list_append( *this->myTerminfoList, anEntry);
  /* > */
  /* < add terminfoEntry: style */
  anEntry = getStyleEntry( &(aLinePortion->myStyle));

  if(!anEntry)
    {
      return;
    }

  *this->myTerminfoList = g_list_append( *this->myTerminfoList, anEntry);
  /* > */
  /* < add terminfoEntry: text */
  if ((aLinePortion->myString)
      && (aLinePortion->myString->str))
    {
      anEntry = getTextEntry( aLinePortion->myString->str);
      
      if(!anEntry)
	{
	  return;
	}
      *this->myTerminfoList = g_list_append( *this->myTerminfoList, anEntry);
    }
  /* > */
}
/* > */
/* < terminfoExpandText */
/* TBD:  taking in account frame */
int terminfoExpandText( GList** theTerminfoList, termAPI* theTermAPI, cursor* theCursor)
{
  int aResult=0;
  cursor* aPreviousCursor = NULL;
  GList* aLinePortionGroup = NULL;
  context* this = NULL;

  ENTER("terminfoExpandText");

  if (!theTerminfoList || !*theTerminfoList || !theTermAPI || !theCursor)
    {
      goto exitExpand;
    }

  this = createContext( theTermAPI, theTerminfoList);
  if (!this)
    {
      goto exitExpand;
    }

  g_list_foreach( *theTerminfoList, (GFunc)searchText, this);

  if (this->myTextIsFound)
    {
      goto exitExpand;
    }

  aResult = 1;

  /* identifying the type of displacement */
  /* TBD: initial implementation. The command must be taken in account (last char/first char jump)*/

  aPreviousCursor = & (this->myCursor);

  if (aPreviousCursor->myLine == theCursor->myLine)
    {
      int aFirstCol = 0;
      int aLastCol = 0;

      if (aPreviousCursor->myCol < theCursor->myCol)
	{
	  aFirstCol = theCursor->myCol;
	  aLastCol = this->myNumberOfCol - 1;
	}
      else
	{
	  aFirstCol = theCursor->myCol;
	  aLastCol = aPreviousCursor->myCol;
	}

      aLinePortionGroup = this->myTermAPI->getLinePortionGroup( theCursor->myLine, aFirstCol, aLastCol);
      
      if (aFirstCol + 1 < aLastCol)
	{
	  GList* aWord = NULL;
	  aWord = getWordLinePortion( aLinePortionGroup, FIRST_OCCURRENCE);
	  deleteLinePortionGroup( aLinePortionGroup);
	  aLinePortionGroup = aWord;
	}
    }
  else
    {
      /* TBD: command must be taken in account. Next word + next line. */
      aLinePortionGroup = this->myTermAPI->getLinePortionGroup( theCursor->myLine, 0, this->myNumberOfCol - 1);
    }

  if (aLinePortionGroup)
    {
      terminfoEntry* anEntry = NULL;

      g_list_foreach( aLinePortionGroup, (GFunc)appendList, this);
      deleteLinePortionGroup( aLinePortionGroup);

      anEntry = getPositionEntry( theCursor->myLine, theCursor->myCol);

      if(anEntry)
	{
	  *theTerminfoList = g_list_append( *theTerminfoList, anEntry);
	}
    }

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
