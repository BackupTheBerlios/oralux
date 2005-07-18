/* 
----------------------------------------------------------------------------
tifilter2l.c
$Id: tifilter2l.c,v 1.4 2005/07/18 22:27:35 gcasse Exp $
$Author: gcasse $
Description: terminfo filter, two lines.
$Date: 2005/07/18 22:27:35 $ |
$Revision: 1.4 $ |
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

#define MAX_LINE_PORTION_ARRAY (MAX_LINE_PORTION * MAX_LINE_PORTION_GROUP)

/* a "LinePortionGroup" gathers the contiguous LinePortions (e.g. a label including a hotkey). It is NULL terminated.
*/

struct t_LinePortionGroup
{
  linePortion* myLinePortion[MAX_LINE_PORTION+1];
  style myStyle;
};

typedef struct t_LinePortionGroup linePortionGroup;


#define sameLine(theGroup) \
  (theGroup \
   && theGroup[0][0] \
   && theGroup[1][0] \
   && (theGroup[0][0]->myLine == theGroup[1][0]->myLine))

static int sameContent( linePortionGroup* theGroup)
{
  ENTER("sameContent");
  return 1;
}

static int sameStyle( linePortionGroup* theGroup )
{
  ENTER("sameStyle");
  return 0;
}

/* > */
/* < context */
struct t_context
{
  cursor myCursor;
  cursor mySavedCursor;
  int myNumberOfLine;
  int myNumberOfCol;
  linePortion myLinePortion[ MAX_LINE_PORTION_ARRAY];
  linePortionGroup myLinePortionGroup[ MAX_LINE_PORTION_GROUP];
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
	  this->myLinePortionGroup[i].myLinePortion[j] = NULL;
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
  int aNumberOfGroup=0;
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
  aNumberOfGroup=1;

  aGroup[ aNumberOfGroup-1].myLinePortion[ j] = &(aLinePortion[i]);
  aGroup[ aNumberOfGroup-1].myLinePortion[ ++j] = NULL;
  SHOW4("aNumberOfGroup=%d, j=%d, \"%s\"\n", 
	aNumberOfGroup, j-1, aLinePortion[i].myString);

  for (i=1; (i < this->myLinePortionIndex) && aNumberOfGroup; i++)
    {
      if (j >= MAX_LINE_PORTION)
	{
	  aNumberOfGroup=0;
	}
      else if ((aLinePortion[ i].myLine == aLinePortion[ i-1].myLine)
	       && (aLinePortion[ i].myFirstCol == aLinePortion[ i-1].myLastCol)
	       )
	{
	  aGroup[ aNumberOfGroup-1].myLinePortion[ j] = &(aLinePortion[i]);
	  aGroup[ aNumberOfGroup-1].myLinePortion[ ++j] = NULL;

	}
      else
	{
	  aNumberOfGroup++;
	  j=0;
	  aGroup[ aNumberOfGroup-1].myLinePortion[ j] = &(aLinePortion[i]);
	  aGroup[ aNumberOfGroup-1].myLinePortion[ ++j] = NULL;

	}
      SHOW4("aNumberOfGroup=%d, j=%d, \"%s\"\n", 
	    aNumberOfGroup, j-1, aLinePortion[i].myString);
    }

  /* determining the major style of each group */
  for (i=0; i<aNumberOfGroup; i++)
    {
      int aWeight[ MAX_LINE_PORTION];
      int k;
      for (j=0; (j < MAX_LINE_PORTION) && aGroup[ i].myLinePortion[ j]; j++)
	{
	  aWeight[ j] = 0;
	  for (k=j+1; (k < MAX_LINE_PORTION) && aGroup[ i].myLinePortion[ k]; k++)
	    {
	      if (compareStyle( &(aGroup[ i].myLinePortion[ j]->myStyle), 
				&(aGroup[ i].myLinePortion[ k]->myStyle)))
		{
		  aWeight[ j] += strlen(aGroup[ i].myLinePortion[ j]->myString);
		}
	    }
	}

      k=0;
      for (j=0; (j < MAX_LINE_PORTION) && aGroup[ i].myLinePortion[ j]; j++)
	{
	  if (aWeight[ j] > aWeight[ k])
	    {
	      k=j;
	    }
	}
      
      copyStyle( &(aGroup[ i].myStyle), &(aGroup[ i].myLinePortion[ k]->myStyle)); 
    }

  return aNumberOfGroup;
}
/* > */
/* < searchUnselectedGroup */
int searchUnselectedGroup( this)
{
/*   int i=0; */

/*   for (i=0; i<MAX_LINE_PORTION_GROUP; i++) */
/*     { */
/*       getBackgroundGroup(i) */
/*     } */
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
      && sameContent(this->myLinePortionGroup) 
      && !sameStyle(this->myLinePortionGroup)
      && sameContent(this->myLinePortionGroup+1) 
      && !sameStyle(this->myLinePortionGroup+1))
    {
      if (isDuplicated)
	{
	  aFilteredList = copyTerminfoList( theTerminfoList);
	}
      else
	{
	  aFilteredList = theTerminfoList;
	}

/*       searchUnselectedGroup( this); */
/*       insertCustomSilenceTerminfo( avant num entry debut, apres num entry fin) */

/*      this->myTermAPI->getBackgroundStyle( linePortion theLinePortion, style* theStyle); */
    }

  deleteContext(this);

  return aFilteredList;
}
/* > */

