/* 
----------------------------------------------------------------------------
termapiSimu.c
$Id: termapiVCSA.c,v 1.7 2005/08/31 23:19:55 gcasse Exp $
$Author: gcasse $
Description: testapi implementation for tests.
$Date: 2005/08/31 23:19:55 $ |
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
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include "termapi.h"
#include "linuxscreen.h"
#include "debug.h"

/* < VCSA_getStyle */
enum terminalColor myAttributeToTermColor[]=
  {
    TERM_COLOR_BLACK, /* 0 */
    TERM_COLOR_BLUE, /* 4 */
    TERM_COLOR_GREEN, /* 2 */
    TERM_COLOR_CYAN, /* 6 */
    TERM_COLOR_RED, /* 1 */
    TERM_COLOR_MAGENTA, /* 5 */
    TERM_COLOR_YELLOW, /* 3 */
    TERM_COLOR_WHITE, /* 7 */
  };

#define COLOR_NB sizeof(myAttributeToTermColor)/sizeof(myAttributeToTermColor[0])
static void VCSA_getStyle( char theAttribute, style* theStyle)
{
  ENTER("VCSA_getStyle");

  clearStyle( theStyle);

  theStyle->myForegroundColor = myAttributeToTermColor[ getForegroundField( theAttribute)];
  theStyle->isBold = getBoldField( theAttribute);
  theStyle->myBackgroundColor = myAttributeToTermColor[ getBackgroundField( theAttribute)];
  theStyle->isBlink = getBlinkField( theAttribute);

  DISPLAY_STYLE(theStyle);
}

/* > */
/* < _getLinePortionGroup */

static GList* _getLinePortionGroup( int theLine, int theFirstCol, int theLastCol)
{
  int aFirstCol=theFirstCol;
  int aLastCol=theLastCol;
  VCSA_Char* aBuffer=NULL;  
  int aByteNumber=0;
  GList* aList=NULL;

  ENTER("_getLinePortionGroup");

  SHOW4("theLine=%d, theFirstCol=%d, theLastCol=%d\n", theLine, theFirstCol, theLastCol);

  /* < build aList*/

  if (readLinuxScreen( theLine, aFirstCol, aLastCol, &aBuffer, &aByteNumber))
  {
    int i=0;
    char c[2];
    style aStyle;
    VCSA_Char* v = (VCSA_Char*)aBuffer;
    linePortion* p = NULL;

    c[0] = v[0].myChar;
    c[1] = 0;

    SHOW4("char %i=%c (0x%x)\n",i,v[i].myChar,v[i].myChar);

    VCSA_getStyle( v[0].myAttribute, &aStyle);
    p = createLinePortion (theLine, aFirstCol, &aStyle, (char*)c, NULL);
    aList = g_list_append( aList, (gpointer)p);

    for( i=1; i < aLastCol - aFirstCol + 1; i++)
      {
	c[0] = v[i].myChar;
	SHOW4("char %i=%c (0x%x)\n",i,v[i].myChar,v[i].myChar);
	if (v[i].myAttribute == v[i-1].myAttribute)
	  { 
	    g_string_append( p->myString, c);
	    SHOW3("g_string_append(%x, %s)\n", (unsigned int)(p->myString), c);
	    p->myLastCol = aFirstCol+i;
	  }
	else
	  {
	    VCSA_getStyle( v[i].myAttribute, &aStyle);
	    p = createLinePortion (theLine, aFirstCol+i, &aStyle, c, NULL);
	    aList = g_list_append( aList, (gpointer)p);
	  }
      }
    free( aBuffer);
  }
  /* > */

  return aList;
}

/* > */

/* < _getColor */
static int _getColor( int theLine, int theFirstCol, int theLastCol, enum terminalColor* theColor, int theBackgroundIsChecked)
{
  int aFirstCol=theFirstCol;
  int aLastCol=theLastCol;
  int aByteNumber=0;
  int i=0;
  int j=0;
  int aColorCounter[COLOR_NB];
  int aColorIsFound=0;
  VCSA_Char* v = NULL;

  ENTER("_getColor");

  SHOW5("theLine=%d, theFirstCol=%d, theLastCol=%d, theBackgroundIsChecked=%d\n", theLine, theFirstCol, theLastCol, theBackgroundIsChecked);

  if (!readLinuxScreen( theLine, aFirstCol, aLastCol, &v, &aByteNumber))
  {
    return 0;
  }

  /* < Initialize aColorCounter */
  for(i=0; i< COLOR_NB; i++)
    {
      aColorCounter[i]=0;
    }
  /* > */

  /* < Count colors */
  SHOW("Attributes:\n");
  for (j=0; j < aLastCol - aFirstCol + 1; j++)
    {
      /* Count how many times each color is met */
      if (theBackgroundIsChecked)
	{
	  aColorCounter[ getBackgroundField(v[j].myAttribute)]++;
	  SHOW2("%d ",getBackgroundField(v[j].myAttribute));
	}
      else
	{
	  aColorCounter[ getForegroundField(v[j].myAttribute)]++;
	  SHOW2("%d ",getForegroundField(v[j].myAttribute));
	}
    }
  /* > */

  /* < Retrieve the color with higher score */
  j=0;
  aColorIsFound=1;
  for( i=1; i< COLOR_NB; i++)
    {
      if (aColorCounter[i] > aColorCounter[j])
	{
	  j=i;
	  aColorIsFound=1;
	}
      else if (aColorCounter[i] == aColorCounter[j])
	{
	  aColorIsFound=0;
	}
    }

  /* > */

  *theColor = myAttributeToTermColor[j]; /* convert color */

  SHOW5("\naColorIsFound=%d, VCSA_color=%d, score=%d, theColor=%d\n", aColorIsFound, j, aColorCounter[j], *theColor);

  free( v);

  return aColorIsFound;
}
/* > */

/* < _getBackground */
static int _getBackground( int theLine, int theFirstCol, int theLastCol, enum terminalColor* theColor)

{
  ENTER("_getBackground");
  return _getColor( theLine, theFirstCol, theLastCol, theColor, 1);
}

/* > */

/* < _getForeground */
static int _getForeground( int theLine, int theFirstCol, int theLastCol, enum terminalColor* theColor)
{
  ENTER("_getForeground");
  return _getColor( theLine, theFirstCol, theLastCol, theColor, 0);
}

/* > */
/* < _getCursor */
static int _getCursor( cursor* theCursor)
{
  VCSA_Char* v=NULL;
  int aByteNumber;
  int aStatus=0;

  ENTER("_getCursor");

  /* Coordinates */

  getCursorLinuxScreen( &(theCursor->myLine), &(theCursor->myCol));

  SHOW3("Cursor: line=%d, col=%d\n", theCursor->myLine, theCursor->myCol);

  /* Style */
  if (readLinuxScreen( theCursor->myLine, theCursor->myCol, theCursor->myCol, &v, &aByteNumber))
  {
    aStatus=1;
    VCSA_getStyle( v->myAttribute, &(theCursor->myStyle));
    free(v);
  }

  return aStatus;
}
/* > */
/* < _getDim */
static int _getDim( int* theNumberOfLine, int* theNumberOfCol)
{
  ENTER("_getDim");
  *theNumberOfLine=25;
  *theNumberOfCol=80;
  return 0;
}
/* > */
/* < createTermAPI */
termAPI* createTermAPI()
{
  termAPI* aTermAPI = (termAPI*)malloc(sizeof(termAPI));

  ENTER("createTermAPI");

  aTermAPI->getCursor=_getCursor;
  aTermAPI->getDim=_getDim;
  aTermAPI->getLinePortionGroup=_getLinePortionGroup;
  aTermAPI->getBackground=_getBackground;
  aTermAPI->getForeground=_getForeground;

  if (!openLinuxScreen()) 
    {
      perror("initLinuxScreen");
      exit(1);
    }

  return aTermAPI;
}
/* > */
/* < deleteTermAPI */
void deleteTermAPI( termAPI* theTermAPI)
{
  ENTER("deleteTermAPI");
  free( theTermAPI);
  closeLinuxScreen ();
}
/* > */



