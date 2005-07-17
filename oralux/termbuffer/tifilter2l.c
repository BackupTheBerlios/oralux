/* 
----------------------------------------------------------------------------
tifilter2l.c
$Id: tifilter2l.c,v 1.3 2005/07/17 17:06:25 gcasse Exp $
$Author: gcasse $
Description: terminfo filter, two lines.
$Date: 2005/07/17 17:06:25 $ |
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
/*#include "terminfo2list.h"*/
#include "debug.h"
#include "tifilter.h"

/* < linePortionGroup */
/* A menu entry is supposed to include up to 3 line portions (highlighted car) */
#define MAX_LINE_PORTION 3 

/* At the moment, just two style changes are processed (two menu entries) */
#define MAX_LINE_PORTION_GROUP 2 

typedef linePortion LINE_PORTION_ARRAY[ MAX_LINE_PORTION * MAX_LINE_PORTION_GROUP];

/* a "LinePortionGroup" is an array of the LinePortions composing a menu entry.
   It is NULL terminated.
*/
typedef linePortion* LINE_PORTION_GROUP[ MAX_LINE_PORTION_GROUP][MAX_LINE_PORTION+1];

#define sameLine(theGroup) \
  (theGroup \
   && theGroup[0][0] \
   && theGroup[1][0] \
   && (theGroup[0][0]->myLine == theGroup[1][0]->myLine))

/* > */
/* < context */
struct t_context
{
  cursor myCursor;
  cursor mySavedCursor;
  int myNumberOfLine;
  int myNumberOfCol;
  LINE_PORTION_ARRAY myLinePortion;
  LINE_PORTION_GROUP myLinePortionGroup;
/*   linePortion myLinePortion[ MAX_LINE_PORTION * MAX_LINE_PORTION_GROUP]; */
/*   linePortion* myLinePortionGroup[ MAX_LINE_PORTION_GROUP][MAX_LINE_PORTION+1]; */
  int myLinePortionIndex;
  int myLinePortionOverload;
  termapi* myTermAPI;
};
typedef struct t_context context;

static context* createContext(termapi* theTermAPI)
{
  int i,j;
  context* this=(context*) malloc(sizeof(context));

  theTermAPI->getCursor( &(this->myCursor));
  theTermAPI->getDim( &(this->myNumberOfLine), &(this->myNumberOfCol));
  this->myLinePortionIndex=0;
  this->myLinePortionOverload=0;
  this->myTermAPI=theTermAPI;

  for (i=0;i<MAX_LINE_PORTION_GROUP;i++)
    {
      for (j=0;j<MAX_LINE_PORTION;j++)
	{
	  this->myLinePortionGroup[i][j] = NULL;
	}
    }

  return this;
}

#define deleteContext(this) free(this)
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
  if (this->myLinePortionIndex >= MAX_LINE_PORTION)
    {
      this->myLinePortionOverload = 1;
    }
  else if(aText && aText->str && strcspn (aText->str, " \t\n,.;!?"))
    {
      createLinePortion (this->myLinePortion+this->myLinePortionIndex,
			 anEntry->myStartingPosition.myLine,
			 anEntry->myStartingPosition.myCol,
			 anEntry->myStartingPosition.myCol+strlen(aText->str),
			 &(anEntry->myStartingPosition.myStyle),
			 aText->str);
      
      (this->myLinePortionIndex)++;
    }
}
/* > */
/* < groupLinePortion */
static int groupLinePortion( context* this)
{
  int aNumberOfLinePortion=0;
  int i,j;
  #define aLinePortion this->myLinePortion
  #define aGroup this->myLinePortionGroup

  ENTER("groupLinePortion");

  if ((this->myLinePortionIndex == 0) 
      || this->myLinePortionOverload)
    {
      return 0;
    }

  i=j=0;
  aNumberOfLinePortion=1;

  aGroup[ aNumberOfLinePortion-1][ j] = &(aLinePortion[i]);
  aGroup[ aNumberOfLinePortion-1][ ++j] = NULL;
  SHOW4("aNumberOfLinePortion=%d, j=%d, \"%s\"\n", 
	aNumberOfLinePortion, j-1, aLinePortion[i].myString);

  for (i=1; (i < this->myLinePortionIndex) && aNumberOfLinePortion; i++)
    {
      if (j >= MAX_LINE_PORTION)
	{
	  aNumberOfLinePortion=0;
	}
      else if ((aLinePortion[ i].myLine == aLinePortion[ i-1].myLine)
	       && (aLinePortion[ i].myFirstCol == aLinePortion[ i-1].myLastCol)
	       )
	{
	  aGroup[ aNumberOfLinePortion-1][ j] = &(aLinePortion[i]);
	  aGroup[ aNumberOfLinePortion-1][ ++j] = NULL;

	}
      else
	{
	  aNumberOfLinePortion++;
	  j=0;
	  aGroup[ aNumberOfLinePortion-1][ j] = &(aLinePortion[i]);
	  aGroup[ aNumberOfLinePortion-1][ ++j] = NULL;

	}
      SHOW4("aNumberOfLinePortion=%d, j=%d, \"%s\"\n", 
	    aNumberOfLinePortion, j-1, aLinePortion[i].myString);
    }

  return aNumberOfLinePortion;
}
/* > */
/* < sameContent */
static int sameContent( context* this)
{
  ENTER("sameContent");
  return 1;
}
/* > */
/* < sameStyle */
static int sameStyle( context* this)
{
  ENTER("sameStyle");
  return 0;
}
/* > */
/* < terminfofilter2lines */
GList* terminfofilter2lines(GList* theTerminfoList, termapi* theTermAPI, int isDuplicated)
{
  GList* aFilteredList=NULL;
  context* this=NULL;
  
  ENTER("terminfofilter2lines");

  this = createContext( theTermAPI);

  g_list_foreach( theTerminfoList, (GFunc)searchLinePortion, this);

  if ((groupLinePortion(this) == 2)
      && sameContent(this) 
      && !sameStyle(this))
    {
      if (isDuplicated)
	{
	  /* List copy: attention the GString pointers (TEXTFIELD) are just duplicated */
	  aFilteredList = copyTerminfoList( theTerminfoList);
	}
      else
	{
	  aFilteredList = theTerminfoList;
	}

/*       this->myTermAPI->getBackgroundStyle( linePortion theLinePortion, style* theStyle); */
    }

  deleteContext(this);

  return aFilteredList;
}
/* > */

