/* 
----------------------------------------------------------------------------
tifilter2l.c
$Id: tifilter2l.c,v 1.2 2005/07/16 17:38:29 gcasse Exp $
$Author: gcasse $
Description: terminfo filter, two lines.
$Date: 2005/07/16 17:38:29 $ |
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
#include <glib.h>
#include "escape2terminfo.h"
/*#include "terminfo2list.h"*/
#include "debug.h"
#include "tifilter.h"

/* < Global definitions */

#define EG_STYLE_C (EG_STYLE+'0')
#define EG_POSITION_C (EG_POSITION+'0')
#define EG_TEXT_C (EG_TEXT+'0')
#define EG_OTHER_C (EG_OTHER+'0')

#define MAX_SUMMARY 100
static char myEntrySummary[MAX_SUMMARY+1];
static int myIndex=0;

/* > */
/* < context */
/* A menu entry can include up to 3 line portions if a car is highlighted */
#define MAX_LINE_PORTION 6 
struct t_context
{
  cursor myCursor;
  cursor mySavedCursor;
  int myNumberOfLine;
  int myNumberOfCol;
  linePortion myLinePortion[ MAX_LINE_PORTION];
  int myLinePortionIndex;
};
typedef struct t_context context;

static context myContext;

void initContext(termapi* theTermAPI)
{
  theTermAPI->getCursor( &(myContext.myCursor));
  theTermAPI->getDim( &(myContext.myNumberOfLine), &(myContext.myNumberOfCol));
  myContext.myLinePortionIndex=0;
}
/* > */
/* < summarizeList */
static void summarizeList(gpointer theEntry, gpointer theResult)
{
  char aGroup;
  enum StringCapacity aCapacity;

  ENTER("summarizeList");

  if ((theEntry == NULL) || (myIndex >= MAX_SUMMARY))
    {
      return;
    }

  aCapacity=((terminfoEntry*)theEntry)->myCapacity;

  DISPLAY_CAPACITY(aCapacity);

  switch( aCapacity)
    {
    case SGR:
      aGroup=EG_STYLE_C;
      break;
      
    case TEXTFIELD:
      {
	GString* aText=((terminfoEntry*)theEntry)->myData1;
	if(aText 
	   && aText->str 
	   && strcspn (aText->str, " \t\n,.;!?"))
	  {
	    aGroup=EG_TEXT_C;
	  }
	else
	  {
	    aGroup=EG_OTHER_C;
	  }
      }
      break;
      
    case CR:
    case CLEAR:
    case HPA:
    case CUP:
    case CUD1:
    case HOME:
    case CUB1:
    case CUF1:
    case NEL:
    case CUU:
    case VPA:
      aGroup=EG_POSITION_C;
      break;
      
    default:
      aGroup=EG_OTHER_C;
      break;
    }

  if ((myIndex==0) || (myEntrySummary[myIndex-1] != aGroup))
    {
      myEntrySummary[myIndex] = aGroup;
      myEntrySummary[++myIndex] = 0;
      SHOW(myEntrySummary);
    }
}
/* > */
/* < processMenuItemsInSingleLine */
GList* processMenuItemsInSingleLine( GList* theList, termapi* theTermAPI)
{
  return theList;
}
/* > */
/* < processMenuItemsInTwoLines */

GList* processMenuItemsInTwoLines( GList* theList, termapi* theTermAPI)
{
  return theList;
}

/* > */
/* < terminfofilter2lines */
GList* terminfofilter2lines(GList* theTerminfoList, termapi* theTermAPI)
{
  char* s=NULL;
  char* s1=NULL;
  ENTER("terminfofilter2lines");

  myIndex=0;
  initContext( theTermAPI);

  g_list_foreach( theTerminfoList, (GFunc)summarizeList, NULL);

  /* Expected pattern: <Text> ... <Style> ... <Text> 
     In this first implementation, no more than two texts are looked for
   */
  if ((s1=strchr (myEntrySummary, EG_TEXT_C))
       && (s=strchr (s1, EG_STYLE_C))
       && (s=strchr (s, EG_TEXT_C))
       && (0==strchr (s, EG_TEXT_C)))
    {
      /* Texts upon one or two lines ? */
      if ((s=strchr (s1, EG_POSITION_C))
	  && (strchr (s, EG_TEXT_C)))
	{
	  theTerminfoList = processMenuItemsInSingleLine( theTerminfoList, theTermAPI);
	}
      else
	{
	  theTerminfoList = processMenuItemsInTwoLines( theTerminfoList, theTermAPI);
	}
    }

  return theTerminfoList;
}
/* > */

