/* 
----------------------------------------------------------------------------
termapiSimu.c
$Id: termapiVCSA.c,v 1.4 2005/08/22 22:55:04 gcasse Exp $
$Author: gcasse $
Description: testapi implementation for tests.
$Date: 2005/08/22 22:55:04 $ |
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
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include "termapi.h"
#include "debug.h"

/* < VCSA data */
struct t_VCSA_Header
{
  unsigned char myLines;
  unsigned char myCols;
  unsigned char myCol; 
  unsigned char myLine;
};
typedef struct t_VCSA_Header VCSA_Header;

/* struct t_VCSA_attribute */
/* { */
/*   unsigned int isBlink : 1; */
/*   unsigned int myBackgroundColor : 3; */
/*   unsigned int isBold : 1; */
/*   unsigned int myForegroundColor : 3; */
/*   unsigned int : 0; */
/* }; */
/* typedef struct t_VCSA_attribute VCSA_attribute; */

#define getBlinkField(a) (((a)>>7) & 1)
#define getBackgroundField(a) (((a)>>4) & 7)
#define getBoldField(a) (((a)>>3) & 1)
#define getForegroundField(a) ((a) & 7)

/* #define VCSA_compareAttribute(a, b) ((a.isBlink==b.isBlink)\ */
/* 				     &&(a.myBackgroundColor=b.myBackgroundColor)\ */
/* 				     &&(a.isBold=b.isBold)\ */
/* 				     &&(a.myForegroundColor=b.myForegroundColor)) */

struct t_VCSA_Char
{
  char myChar; 
  char myAttribute;
};
typedef struct t_VCSA_Char VCSA_Char;

static int myDescriptor = 0;
static VCSA_Header myHeader;

/* > */
/* < VCSA_getStyle */
enum terminalColor myAttributeToTermColor[]=
  {
    TERM_COLOR_BLACK,
    TERM_COLOR_BLUE,
    TERM_COLOR_GREEN,
    TERM_COLOR_CYAN,
    TERM_COLOR_RED,
    TERM_COLOR_MAGENTA,
    TERM_COLOR_YELLOW,
    TERM_COLOR_WHITE,
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

  /* < check consistency */
  if (theLine >= myHeader.myLines)
    {
      return NULL;
    }
  if (aFirstCol >= myHeader.myCols)
    {
      aFirstCol = myHeader.myCols - 1;
    }
  if (aLastCol >= myHeader.myCols)
    {
      aLastCol = myHeader.myCols - 1;
    }
  if (aFirstCol > aLastCol)
    {
      int i=aLastCol;
      aLastCol = aFirstCol;
      aFirstCol = i;
    }
  /* > */
  /* < get aBuffer */
  aByteNumber = (aLastCol - aFirstCol + 1) * sizeof(VCSA_Char);
  aBuffer = (VCSA_Char *) malloc( aByteNumber);

  lseek( myDescriptor, 
	 sizeof(VCSA_Header) + sizeof(VCSA_Char) * (theLine * myHeader.myCols + aFirstCol), 
	 0);
  read( myDescriptor, aBuffer, aByteNumber);
  /* > */
  /* < build aList*/
  {
    int i=0;
    char c[2];
    style aStyle;
    VCSA_Char* v = (VCSA_Char*)aBuffer;
    linePortion* p = NULL;

    c[0] = v[0].myChar;
    c[1] = 0;

    SHOW3("char %i=%c\n",i,v[i].myChar);

    VCSA_getStyle( v[0].myAttribute, &aStyle);
    p = createLinePortion (theLine, aFirstCol, &aStyle, (char*)c);
    aList = g_list_append( aList, (gpointer)p);

    for( i=1; i < aLastCol - aFirstCol + 1; i++)
      {
	c[0] = v[i].myChar;
	SHOW3("char %i=%c\n",i,v[i].myChar);
	if (v[i].myAttribute == v[i-1].myAttribute)
	  { 
	    g_string_append( p->myString, c);
	    SHOW3("g_string_append(%x, %s)\n", (unsigned int)(p->myString), c);
	    p->myLastCol = aFirstCol+i;
	  }
	else
	  {
	    VCSA_getStyle( v[i].myAttribute, &aStyle);
	    p = createLinePortion (theLine, aFirstCol+i, &aStyle, c);
	    aList = g_list_append( aList, (gpointer)p);
	  }
      }
  }
  /* > */
  free( aBuffer);

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

  /* < Check consistency */
  if (theLine >= myHeader.myLines)
    {
      return 0;
    }

  if (aFirstCol >= myHeader.myCols)
    {
      aFirstCol = myHeader.myCols - 1;
    }
  if (aLastCol >= myHeader.myCols)
    {
      aLastCol = myHeader.myCols - 1;
    }
  if (aFirstCol > aLastCol)
    {
      i=aLastCol;
      aLastCol = aFirstCol;
      aFirstCol = i;
    }
  /* > */

  /* < Initialize aColorCounter */
  for(i=0; i< COLOR_NB; i++)
    {
      aColorCounter[i]=0;
    }
  /* > */

  /* < Count colors */
  aByteNumber = (aLastCol - aFirstCol + 1) * sizeof(VCSA_Char);
  v = (VCSA_Char *) malloc( aByteNumber);

  SHOW2("\n---- Line:%d\n",j);

  lseek( myDescriptor, 
	 sizeof(VCSA_Header) + sizeof(VCSA_Char) * (i * myHeader.myCols + aFirstCol), 
	 0);
  read( myDescriptor, v, aByteNumber);

  for (j=0; j < aLastCol - aFirstCol + 1; j++)
    {
      /* Count how many times each color is met */
      if (theBackgroundIsChecked)
	{
	  aColorCounter[ getBackgroundField(v[j].myAttribute)]++;
	  SHOW2("%d",getBackgroundField(v[j].myAttribute));
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
  VCSA_Char v;

  ENTER("_getCursor");

  /* Coordinates */
  lseek( myDescriptor, 0, 0);
  read( myDescriptor, &myHeader, sizeof(VCSA_Header));
  theCursor->myLine = myHeader.myLine;
  theCursor->myCol = myHeader.myCol;

  SHOW3("Cursor: line=%d, col=%d\n", theCursor->myLine, theCursor->myCol);

  /* Style */
  lseek( myDescriptor, 
	 sizeof(VCSA_Header) + sizeof(VCSA_Char) * (myHeader.myLine * myHeader.myCols + myHeader.myCol), 
	 0);
  read( myDescriptor, &v, sizeof(VCSA_Char));

  VCSA_getStyle( v.myAttribute, &(theCursor->myStyle));

  return 1;
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

  myDescriptor = open( "/dev/vcsa", O_RDWR);

  if (myDescriptor < 0) 
    {
      perror("/dev/vcsa");
      exit(1);
    }

  (void)read(myDescriptor, &myHeader, 4);

  return aTermAPI;
}
/* > */
/* < deleteTermAPI */
void deleteTermAPI( termAPI* theTermAPI)
{
  ENTER("deleteTermAPI");
  free( theTermAPI);
  close( myDescriptor);
}
/* > */



