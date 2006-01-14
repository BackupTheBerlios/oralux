/* 
----------------------------------------------------------------------------
tifilter2l.c
$Id: tigetline.c,v 1.9 2006/01/14 08:34:51 gcasse Exp $
$Author: gcasse $
Description: terminfo filter, retreive one line.
$Date: 2006/01/14 08:34:51 $ |
$Revision: 1.9 $ |
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
  GList* myExpansionList;
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
  this->myExpansionList = NULL;
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

  DISPLAY_CAPACITY(anEntry->myCapacity);
  /* In case of cursor jump, the line, word,.. will be expanded (said). 
     If some text is found the line will not be expanded. */

  if (anEntry 
      && (anEntry->myCapacity == TEXTFIELD))
    {
      GString* aString = anEntry->myData1;

      /* strcspn: in some cases, the cursor jumps to a new line and 
	 the ">" char prepends the line. This symbol is understood 
	 as a prompt not as text: the whole line must be expanded (said).
      */
      if (aString
	  && aString->str)
	{
	  char* s = strdup( aString->str);

	  SHOW2("s=%s\n",s);

	  if( s && strtok( s, " \n\r\t*-+=/\\<>#$.,;:!?§"))
	    {
	      this->myTextIsFound = 1;
	      SHOW("myTextIsFound\n");
	    }
	  free( s);
	}
    }
}
/* > */
/* < addList */
static void addList(gpointer theLinePortion, gpointer theContext)
{
  terminfoEntry* anEntry = NULL;
  context* this = (context*)theContext;
  linePortion* aLinePortion = (linePortion*)theLinePortion;

  ENTER("addList");

  //  if (!aLinePortion || !this || !this->myTerminfoList || !*this->myTerminfoList)
  if (!aLinePortion || !this)
    {
      return;
    }

  /* < add terminfoEntry: position */
  anEntry = getPositionEntry( aLinePortion->myLine, aLinePortion->myFirstCol);

  if(!anEntry)
    {
      return;
    }

  this->myExpansionList = g_list_append( this->myExpansionList, anEntry);
  //  *this->myTerminfoList = g_list_append( *this->myTerminfoList, anEntry);
  /* > */
  /* < add terminfoEntry: style */
  anEntry = getStyleEntry( &(aLinePortion->myStyle));

  if(!anEntry)
    {
      return;
    }

  this->myExpansionList = g_list_append( this->myExpansionList, anEntry);
  //  *this->myTerminfoList = g_list_append( *this->myTerminfoList, anEntry);
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
      this->myExpansionList = g_list_append( this->myExpansionList, anEntry);
      //      *this->myTerminfoList = g_list_append( *this->myTerminfoList, anEntry);
    }
  /* > */
}
/* > */
/* < terminfoExpandText */
/* TBD:  taking in account frame (docAPI.c) */
int terminfoExpandText( GList** theTerminfoList, termAPI* theTermAPI, cursor* theFirstCursor, cursor* theLastCursor)
{
  int aResult=0;
  GList* aLinePortionGroup = NULL;
  context* this = NULL;

  ENTER("terminfoExpandText");

  DISPLAY_STYLE( &(theFirstCursor->myStyle));

  if (!theTerminfoList || !*theTerminfoList || !theTermAPI || !theFirstCursor)
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
  /* TBD: initial implementation. The command must be taken in account (last char/first char jump; jump to next word on next line)*/

  if (theFirstCursor->myLine == theLastCursor->myLine)
    {
      int aFirstCol = 0;
      int aLastCol = 0;

      if (theFirstCursor->myCol < theLastCursor->myCol)
	{
	  aFirstCol = theLastCursor->myCol;
	  aLastCol = this->myNumberOfCol - 1;
	}
      else
	{
	  aFirstCol = theLastCursor->myCol;
	  aLastCol = theFirstCursor->myCol;
	}

      aLinePortionGroup = this->myTermAPI->getLinePortionGroup( theLastCursor->myLine, aFirstCol, aLastCol);
      
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
      aLinePortionGroup = this->myTermAPI->getLinePortionGroup( theLastCursor->myLine, 0, this->myNumberOfCol - 1);
    }

  if (aLinePortionGroup)
    {
      terminfoEntry* anEntry = NULL;

      g_list_foreach( aLinePortionGroup, (GFunc)addList, this);
      deleteLinePortionGroup( aLinePortionGroup);

      anEntry = getPositionEntry( theFirstCursor->myLine, theFirstCursor->myCol);
      if(anEntry)
	{
	  // TBD: the expanded text must be merged to the possible prompt.
	  //
	  // For example, if prompt = ">" and expansion = "  hello" 
	  // it implies "> hello".
	  // And the terminfo = ">" + "  hello" + ">".
	  // The second muted ">" is added to be sure that the expansion 
	  // does not overwrite the original prompt.
	  GList* aTerminfoList2 = copyTerminfoList( *theTerminfoList);
	  GList* aList = g_list_concat( *theTerminfoList, this->myExpansionList);
	  /* < the original terminfo must not be overwritten by the expansion */
	  {
	  aList = g_list_append( aList, anEntry); /* back to first cursor */
	  aList = g_list_append( aList, get_TSAR_Sequence( 0, 1)); /* muted */
	  aList = g_list_concat( aList, aTerminfoList2);
	  /* TBD: volume must be related to the computed value (docAPI.c) */
	  aList = g_list_append( aList, get_TSAR_Sequence( 100, 1));
	  }
	  /* > */

	  *theTerminfoList = aList;
	}
    }

 exitExpand:
  deleteContext( this);

  SHOW2("Exit=%d\n", aResult);

  return aResult;
}

/* > */
/* 
Local variables:
folded-file: t
folding-internal-margins: nil
*/
