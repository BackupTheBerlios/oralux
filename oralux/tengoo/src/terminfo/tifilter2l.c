/* 
----------------------------------------------------------------------------
tifilter2l.c
$Id: tifilter2l.c,v 1.6 2005/10/17 22:55:14 gcasse Exp $
$Author: gcasse $
Description: terminfo filter, two lines.
$Date: 2005/10/17 22:55:14 $ |
$Revision: 1.6 $ |
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
/*#include "terminfo2list.h"*/
#include "debug.h"
#include "tifilter.h"

/* < context */

/* A menu entry is supposed to include up to 3 line portions (highlighted car) 
*/
#define MAX_LINE_PORTION 3 

/* At the moment, just two style changes are processed (two menu entries) plus one for the remaining text */
#define NB_STYLE_CHANGE 2
#define MAX_LINE_PORTION_GROUP 4

struct t_context
{
  cursor myCursor;
  cursor mySavedCursor;
  int myNumberOfLine;
  int myNumberOfCol;
  GList* myLinePortionGroup[ MAX_LINE_PORTION_GROUP ];
  int myLinePortionCount;
  termAPI* myTermAPI;
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
  this->myLinePortionCount = 0;

  for (i=0; i<MAX_LINE_PORTION_GROUP; i++)
    {
      this->myLinePortionGroup[i] = NULL;
    }

  return this;
}

static void deleteContext(context* this)
{ 
  int i;

  ENTER("deleteContext");

  for (i=0; i<MAX_LINE_PORTION_GROUP; i++)
    {
      SHOW2("Line Portion: %d\n",i);
      if(this->myLinePortionGroup[i]);
      {
	deleteLinePortionGroup(this->myLinePortionGroup[i]);
      }
    }
  free(this);
}
/* > */
/* < searchLinePortion */
static void searchLinePortion(gpointer theEntry, gpointer theContext)
{
  terminfoEntry* anEntry = (terminfoEntry*)theEntry;
  context* this = (context*)theContext;
  GString* aText = NULL;

  ENTER("searchLinePortion");

  if ((anEntry == NULL) 
      || (anEntry->myCapacity != TEXTFIELD))
    {
      return;
    }

  aText = anEntry->myData1;
  if(aText && aText->str)     
    {
      cursor* c =  &(anEntry->myStartingPosition);
      linePortion* p= createLinePortion (c->myLine, c->myCol, &(c->myStyle), aText->str, anEntry->myParent);
      this->myLinePortionGroup[ 0] = g_list_append( this->myLinePortionGroup[ 0], p);
    }
}
/* > */
/* < groupLinePortion */
static int groupLinePortion( context* this)
{
  int i=0;
  GList* aList=NULL;

  ENTER("groupLinePortion");

  if (this->myLinePortionGroup[0] == NULL)
    {
      return 0;
    }

  aList = this->myLinePortionGroup[i];
  i++;
  SHOW3("Group=%d: \"%s\"\n", i-1, getStringFromGList( aList));

  while(( aList = g_list_next( aList)) && (i < MAX_LINE_PORTION_GROUP))
    {
      /* New group ? */
      if ((getLineFromGList( aList) != getLineFromGList( aList->prev))
	  || (getFirstColFromGList( aList) != getLastColFromGList( aList->prev)+1)
	  || (
	      (compareStyle( getStyleAddressFromGList( aList), getStyleAddressFromGList( aList->prev))!=0)
	      && (getStringLengthFromGList(aList) != 1)
	      && (getStringLengthFromGList(aList->prev) != 1)
	      ))
	{
	  aList->prev->next = NULL;
	  aList->prev = NULL;
	  this->myLinePortionGroup[i] = aList;
	  i++;
	} 
      SHOW3("Group=%d: \"%s\"\n", i-1, getStringFromGList( aList));
    }
  SHOW2("groupLinePortion returns: %d\n", i);

  return i;
}
/* > */
/* < findLineForBackgroundTest */
static int findLineTest(  context* this, linePortion* p1, linePortion* p2, int* theLine, int* theFirstCol, int* theLastCol)
{
  int aLine1;
  int aLine2;
  int aLineIsFound=0;

  ENTER("findLineTest");

  if (!p1 || !p2 || !theLine || !theFirstCol || !theLastCol)
    {
      return 0;
    }

  aLineIsFound = 1;
  
  /* search which is the currently selected line */
  if (p1->myLine <= p2->myLine)
    {
      aLine1 = p1->myLine;
      aLine2 = p2->myLine;
    }
  else
    {
      aLine1 = p2->myLine;
      aLine2 = p1->myLine;
    }	  
  
  *theFirstCol = MIN( p1->myFirstCol, p2->myFirstCol);
  *theLastCol = MAX( p1->myLastCol, p2->myLastCol);

  SHOW5("theFirstCol=%d, theLastCol=%d, aLine1=%d, aLine2=%d\n",
       *theFirstCol,
       *theLastCol,
       aLine1,
       aLine2);
  
  if (aLine1 != aLine2)
    {
      if (aLine1 + 1 == aLine2)
	{
	  if (aLine1 <= 1) 
	    { /* 1 to avoid a possible menu bar */
	      *theLine = aLine2 + 1;
	    }
	  else
	    {
	      *theLine = aLine1 - 1;
	    }
	}
      else
	{
	  *theLine = aLine1 + 1;
	}
    }
  else
    {
      int aCol1=0;
      int aCol2=0;
      
      if (p1->myLastCol < p2->myFirstCol)
	{
	  aCol1 = p1->myLastCol;
	  aCol2 = p2->myFirstCol;
	}
      else
	{
	  aCol1 = p2->myLastCol;
	  aCol2 = p1->myFirstCol;
	}	  

      /* At least 4 chars between the two line portions */ 
      if (aCol1 + 4 < aCol2)
	{
	  *theLine = aLine1;
	  *theFirstCol = aCol1 + 1; 
	  *theLastCol = aCol2 - 1; 
	}
      else if (*theFirstCol > 4)
	{
	  *theLine = aLine1;
	  *theLastCol = *theFirstCol -1; 
	  *theFirstCol = *theLastCol -4;
	}
      else if (*theLastCol + 4 < this->myNumberOfCol)
	{
	  *theLine = aLine1;
	  *theFirstCol = *theLastCol + 1;
	  *theLastCol = *theFirstCol + 4;
	}
      else if (aLine1 == 0)
	{
	  *theLine = aLine1 + 1;
	}
      else
	{
	  *theLine = aLine1 -1;
	}
    }
  SHOW5("aLineIsFound=%d, theLine=%d, theFirstCol=%d, theLastCol=%d\n",aLineIsFound, *theLine, *theFirstCol, *theLastCol);

return aLineIsFound;
}
/* > */
/* < getHighlightedLine */
/* return the highlighted line portion or NULL */
static linePortion* getHighlightedLine( context* this, linePortion* p1, linePortion* p2)
{
  enum terminalColor aColor;
  int aLineTest1=0;
  int aFirstColTest1=0;
  int aLastColTest1=0;
  unsigned int aBackgroundColor[2];
  unsigned int aForegroundColor[2];
  linePortion* aHighlightedLinePortion=NULL;

  ENTER("getHighlightedLine");

  if (!findLineTest( this, p1, p2, &aLineTest1, &aFirstColTest1, &aLastColTest1))
    {
      return NULL;
    }

  if (!this->myTermAPI->getBackground( aLineTest1, aFirstColTest1, aLastColTest1, &aColor))
    {
      return NULL;
    }

  SHOW("highlighted Line?\n");
  
  getEquivalentStyle( &(p1->myStyle), aBackgroundColor, aForegroundColor);
  getEquivalentStyle( &(p2->myStyle), aBackgroundColor+1, aForegroundColor+1);

  if (aBackgroundColor[0] == aColor)
    {
      SHOW2("BG1: no (%s)\n", p1->myString->str);
      if (aBackgroundColor[1] == aColor)
	{
	  SHOW2("BG2: no (%s)\n", p2->myString->str);

	  if (this->myTermAPI->getForeground(aLineTest1, aFirstColTest1, aLastColTest1, &aColor))
	    {
	      if (aForegroundColor[0] == aColor)
		{
		  SHOW2("FG1: no (%s)\n", p1->myString->str);
		  if (aForegroundColor[1] == aColor)
		    {
		      SHOW2("FG2: no (%s)\n", p2->myString->str);
		    }
		  else
		    {
		      SHOW2("FG2: yes (%s)\n", p2->myString->str);
		      aHighlightedLinePortion = p2;
		    }
		}
	      else
		{
		  SHOW2("FG1: yes (%s)\n", p1->myString->str);

		  if (aForegroundColor[1] == aColor)
		    {
		      SHOW2("FG2: no (%s)\n", p2->myString->str);
		      aHighlightedLinePortion = p1;
		    }
		  else
		    {
		      SHOW2("FG2: yes (%s)\n", p2->myString->str);
		    }
		}
	    }
	  else
	    {
	      return NULL;
	    }
	}
      else
	{
	  SHOW2("BG2: yes (%s)\n", p2->myString->str);
	  aHighlightedLinePortion = p2;			      
	}
    }
  else
    {
      SHOW2("BG1: yes (%s)\n", p1->myString->str);
      aHighlightedLinePortion = p1;
    }

  return aHighlightedLinePortion;
}
/* > */
/* < terminfofilter2lines */

int terminfofilter2lines(GList* theTerminfoList, termAPI* theTermAPI, GList** thePreviouslyHighlightedElement, GList** theNewlyHighlightedElement)
{
  context* this=NULL;
  int aNumberOfLinePortionGroup=0;
  
  ENTER("terminfofilter2lines");

  if (!theTerminfoList || !theTermAPI || !thePreviouslyHighlightedElement || !theNewlyHighlightedElement)
    {
      return 0;
    }

  *thePreviouslyHighlightedElement = NULL;
  *theNewlyHighlightedElement = NULL;

  this = createContext( theTermAPI);
  if (!this)
    {
      return 0;
    }

  g_list_foreach( theTerminfoList, (GFunc)searchLinePortion, this);

  aNumberOfLinePortionGroup = groupLinePortion(this);

  if (aNumberOfLinePortionGroup >= NB_STYLE_CHANGE)
    { /* looking for the style changes */
      GList* new_g[ NB_STYLE_CHANGE+1];
      GList* old_g[ NB_STYLE_CHANGE+1];
      linePortion new_p[ NB_STYLE_CHANGE+1];
      linePortion old_p[ NB_STYLE_CHANGE+1];
      int i=0; /* myLinePortionGroup index */
      int j=0; /* index and number of style changes */

      for(i=0; i<aNumberOfLinePortionGroup && j<NB_STYLE_CHANGE+1; i++)
	{
	  SHOW2("* linePortionGroup %d\n",i);

	  /* get info ('features') about the new group of line portions */ 
	  new_g[j]=this->myLinePortionGroup[i];
	  getFeaturesLinePortionGroup( new_g[j], &(new_p[j]));

	  /* get info about the old group of line portions (currently displayed) */ 
	  old_g[j] = this->myTermAPI->getLinePortionGroup( new_p[j].myLine,
							   new_p[j].myFirstCol,
							   new_p[j].myLastCol);
	  getFeaturesLinePortionGroup( old_g[j], &(old_p[j]));

	  if (new_p[j].myString && new_p[j].myString->str
	      && old_p[j].myString && old_p[j].myString->str)
	    {
	      SHOW3("new string %d: \"%s\"\n",j,new_p[j].myString->str);
	      SHOW3("old string %d: \"%s\"\n",j,old_p[j].myString->str);

	      SHOW2("** New style %d:\n",j);
	      DISPLAY_STYLE(&(new_p[j].myStyle));
	      
	      SHOW2("** Old style %d:\n",j);
	      DISPLAY_STYLE(&(old_p[j].myStyle));

	      /* interesting if same contents and distinct styles */
	      if ((strcmp( new_p[j].myString->str, old_p[j].myString->str) == 0)
		  && !equivalentStyle( &(new_p[j].myStyle), &(old_p[j].myStyle)))
		{
		  j++;
		}
	    }
	}

      SHOW2("Number of style changes = %d\n",j);

      if( j==NB_STYLE_CHANGE)
	{
	  linePortion* aOldHighlightedLinePortion=NULL;
	  SHOW("The old highlighted line was:\n");
	  aOldHighlightedLinePortion = getHighlightedLine( this, old_p, old_p+1);
	  if (aOldHighlightedLinePortion)
	    {
	      if (old_p == aOldHighlightedLinePortion)
		{
		  i=0; /* index of line portion group, no more highlighted */
		  j=1;
		}
	      else
		{
		  i=1;
		  j=0;
		}

	      *thePreviouslyHighlightedElement = getTerminfoElementLinePortionGroup( new_g[i]);
	      *theNewlyHighlightedElement = getTerminfoElementLinePortionGroup( new_g[j]);
	    }
	}
    }
  deleteContext(this);

  SHOW3("PreviouslyHighlightedElement:%x, NewlyHighlightedElement:%x\n",
	(int)*thePreviouslyHighlightedElement, 
	(int)*theNewlyHighlightedElement);
 
 
  return (*thePreviouslyHighlightedElement && *theNewlyHighlightedElement);
}

/* > */

/* 
Local variables:
folded-file: t
folding-internal-margins: nil
*/
