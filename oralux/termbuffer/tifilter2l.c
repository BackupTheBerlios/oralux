/* 
----------------------------------------------------------------------------
tifilter2l.c
$Id: tifilter2l.c,v 1.6 2005/08/07 19:43:54 gcasse Exp $
$Author: gcasse $
Description: terminfo filter, two lines.
$Date: 2005/08/07 19:43:54 $ |
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
#define MAX_LINE_PORTION_GROUP 3

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
  if(aText && aText->str && strcspn (aText->str, " \t"))     
    {
      cursor* c =  &(anEntry->myStartingPosition);
      linePortion* p= createLinePortion (c->myLine, c->myCol, &(c->myStyle), aText->str);
      this->myLinePortionGroup[ 0] = g_list_append( this->myLinePortionGroup[ 0], (gpointer)p);
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
	  || (getFirstColFromGList( aList) != getLastColFromGList( aList->prev)+1))
	{ 
	  aList->prev->next = NULL;
	  aList->prev = NULL;
	  this->myLinePortionGroup[i] = aList;
	  i++;
	} 
      SHOW3("Group=%d: \"%s\"\n", i-1, getStringFromGList( aList));
    }

  return i;
}
/* > */
/* < searchUnselectedGroup */
int searchUnselectedGroup( this)
{
/*   int i=0; */
  ENTER("searchUnselectedGroup");

/*   for (i=0; i<MAX_LINE_PORTION_GROUP; i++) */
/*     { */
/*       getBackgroundGroup(i) */
/*     } */
  return 0;
}
/* > */
/* < terminfofilter2lines */

GList* terminfofilter2lines(GList* theTerminfoList, termAPI* theTermAPI, int isDuplicated)
{
  GList* aFilteredList=theTerminfoList;
  context* this=NULL;
  
  ENTER("terminfofilter2lines");

  this = createContext( theTermAPI);

  g_list_foreach( theTerminfoList, (GFunc)searchLinePortion, this);

  if (groupLinePortion(this) == 2)
    {
      GList* new_g[2];
      GList* old_g[2];
      linePortion new_p[2];
      linePortion old_p[2];
      int i;
      int isInteresting=1;

      for(i=0; i<2 && isInteresting; i++)
	{
	  new_g[i]=this->myLinePortionGroup[i];
	  getFeaturesLinePortionGroup( new_g[i], &(new_p[i]));

	  old_g[i] = this->myTermAPI->getLinePortionGroup( new_p[i].myLine,
							   new_p[i].myFirstCol,
							   new_p[i].myLastCol);
	  getFeaturesLinePortionGroup( old_g[i], &(old_p[i]));

	  /* interesting if same contents and distinct styles */
	  isInteresting = ( (strcmp( new_p[0].myString->str, 
				     old_p[0].myString->str) == 0)
			    && !compareStyle( &(new_p[0].myStyle), 
					      &(old_p[0].myStyle)));
	}

      if( isInteresting)
	{
	  if (isDuplicated)
	    {
	      aFilteredList = copyTerminfoList( theTerminfoList);
	    }
	  /*       searchUnselectedGroup( this); */
	  /*       insertCustomSilenceTerminfo( avant num entry debut, apres num entry fin) */
	}
    }
  deleteContext(this);
  
  return aFilteredList;
}

/* > */

