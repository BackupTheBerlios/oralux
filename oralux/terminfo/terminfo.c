/* 
----------------------------------------------------------------------------
terminfo.c
$Id: terminfo.c,v 1.5 2004/12/27 22:19:59 gcasse Exp $
$Author: gcasse $
Description: store the layout using the supplied terminfo commands. 
$Date: 2004/12/27 22:19:59 $ |
$Revision: 1.5 $ |
Copyright (C) 2003, 2004 Gilles Casse (gcasse@oralux.org)

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
/* < include */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "escape2terminfo.h"
#include "debug.h"

/* > */
/* < flex definitions */
int yywrap ()
{  
   return 1; 
}
/* > */
/* < cursor */

struct t_cursor
{
  int myLine;
  int myCol;
  struct t_style myStyle;
};

#define copyCursor( theDestination, theSource) memcpy( theDestination, theSource, sizeof(struct t_cursor))

/* > */
/* < linePortion */

/* 
A line portion describes an horizontal area in the screen.
When a menu is browsed, the selected item and the previously ones are displayed with distinct styles (for example distinct backgrounds).
These two line portions must be particularly managed to distinguish the selected one.
*/
struct t_linePortion
{
  int myLine; /* Line number */
  int myFirstCol; /* First column of the portion */
  int myLastCol; /* Last column of the portion */
  int myContentIsModified; /* equals 1 if at least one char has been modified */
  int myStyleIsModified; /* equals 1 if one of the style has been modified */
};

enum {MAX_LINE_PORTION=2}; /* 2 line portions are expected to distinguish the selected item */

/* > */
/* < createBuffer */

void createBuffer( char** theDataBuffer, struct t_style** theStyleBuffer, int theNumberOfCell)
{
  ENTER("createBuffer");
  *theDataBuffer=(char*)malloc( theNumberOfCell * sizeof(char));
  *theStyleBuffer=(struct t_style*)malloc( theNumberOfCell * sizeof(struct t_style));
}

/* > */
/* < clearBuffer */

void clearBuffer( char* theDataBuffer, struct t_style* theStyleBuffer, int theNumberOfCell)
{
  ENTER("clearBuffer");
  memset(theDataBuffer, 0x20, theNumberOfCell*sizeof(char));
  memset(theStyleBuffer, 0, theNumberOfCell*sizeof(struct t_style));
}

/* > */
/* < initCursor */

/*
theFirstCell indicates the value of the first line and the first column.
This can be 0 or 1 according to the terminal.
*/
void initCursor(struct t_cursor* theCursor, int theFirstCell)
{
  ENTER("initCursor");
  theCursor->myLine=theFirstCell;
  theCursor->myCol=theFirstCell;
  memset (& (theCursor->myStyle), 0, sizeof(struct t_style));
  theCursor->myStyle.myBackgroundColor=TERM_COLOR_BLACK;
  theCursor->myStyle.myForegroundColor=TERM_COLOR_WHITE;
}

/* > */
/* < getCell*/
#define getCell( theLine, theCol, theNumberOfCol) (theLine * theNumberOfCol + theCol)
/* > */
/* < compareAndSetStyle */
/* 
search if the style of the cell equals one of the two supplied styles (in the ListOfStyle).
If yes, this style is copied in theFoundStyle and 1 is returned. 
Otherwise 0 is returned
*/
int compareAndSetStyle( struct t_style* theListOfStyle, struct t_style* theStyleBuffer, int theLine, int theCol, int theNumberOfCol, struct t_style* theFoundStyle)
{
  int aCell=getCell( theLine, theCol, theNumberOfCol);
  int aStatus=0;
  if ((compareStyle (theListOfStyle, theStyleBuffer+aCell) == 0)
      || (compareStyle (theListOfStyle+1, theStyleBuffer+aCell) == 0))
    {
      copyStyle( theFoundStyle, theStyleBuffer+aCell);
      DISPLAY_STYLE(theFoundStyle);
      aStatus=1;
    }
  return aStatus;
}

/* > */
/* < getBackgroundStyle */

/* retrieve the background style of a line portion. 
If the background style has been found, theStyle is updated, and the number of distinct styles found in the line portion is returned (so 1, 2 or 3). 
Otherwise 0 is returned. 

Three cases are expected:
- 1 style in the line portion: each character has the same style which is also considered as the background style.
- 2 styles: then the surroundings of the line portion is checked to determine the background.
- 3 styles: if two of them are equal, this the background.
- other cases: not processed.
*/ 

int getBackgroundStyle( int theLine, int theCol, struct t_style* theStyleBuffer, int theLength, int theNumberOfLine, int theNumberOfCol, struct t_style* theStyle)
{
  enum {MAX_DISTINCT_STYLE=3};
  struct t_style aStyle[MAX_DISTINCT_STYLE];
  int aStyleWeight[MAX_DISTINCT_STYLE];
  int aIndex=0;
  int i=0;
  int aCell=getCell( theLine, theCol, theNumberOfCol);

  ENTER("getBackgroundStyle");

  copyStyle( aStyle+aIndex, theStyleBuffer+aCell+i);
  aStyleWeight[ aIndex]=1;

  for(i=1; i<theLength; i++)
    {
      if (compareStyle (aStyle+aIndex, theStyleBuffer+aCell+i) == 0)
	{
	  ++aStyleWeight[ aIndex];
	}
      else if (aStyleWeight[ aIndex - 1]==1)
	{ /* the previous style concerns just one char */ 
	  --aIndex; /* it is not taken in account (it can be a shortcut). */
	  if ((aIndex>0) 
	      && (compareStyle (aStyle+aIndex-1, theStyleBuffer+aCell+i) == 0))
	    { /* the current style is the same than the previous one */
	      --aIndex;
	      ++aStyleWeight[ aIndex];
	    }
	  else
	    { /* the current style is distinct */
	      copyStyle( aStyle+aIndex, theStyleBuffer+aCell+i);
	      aStyleWeight[ aIndex]=1;	      
	    }
	}
      else if (++aIndex < MAX_DISTINCT_STYLE)
	{
	  copyStyle( aStyle+aIndex, theStyleBuffer+aCell+i);
	  aStyleWeight[ aIndex]=1;
	}
      else
	{ /* too many styles: abandon... */
	  aIndex=-1;
	}
    }

#ifdef DEBUG
  for (i=0; i<=aIndex; i++)
    {
      SHOW3("* style #%d used by %d chars\n", i, aStyleWeight[i]);
      DISPLAY_STYLE(aStyle+i);
    }
#endif

  switch(aIndex)
    {
    case 0: /* just one style */
      copyStyle( theStyle, aStyle);
      break;
    case 1: /* two styles: look around the line portion */
      aIndex=-1; /* no result at the moment */

      /* Check the top cells */
      if (theLine > 0)
	{ 
	  if (theCol > 0)
	    {
	      SHOW("Test Top Left Cell");
	      aIndex=compareAndSetStyle( aStyle, theStyleBuffer, theLine-1, theCol-1, theNumberOfCol, theStyle);
	    }
	  if((theCol+theLength < theNumberOfCol) && (aIndex != 1))
	    {
	      SHOW("Test Top Right Cell");
	      aIndex=compareAndSetStyle( aStyle, theStyleBuffer, theLine-1, theCol+theLength, theNumberOfCol, theStyle);
	    }
	}
      /* otherwise check the bottom cells */
      if ((theLine+1 < theNumberOfLine) && (aIndex != 1))
	{
	  if (theCol > 0)
	    {
	      SHOW("Test Bottom Left Cell");
	      aIndex=compareAndSetStyle( aStyle, theStyleBuffer, theLine+1, theCol-1, theNumberOfCol, theStyle);
	    }
	  if((theCol+theLength < theNumberOfCol) && (aIndex != 1))
	    {
	      SHOW("Test Bottom Right Cell");
	      aIndex=compareAndSetStyle( aStyle, theStyleBuffer, theLine+1, theCol+theLength, theNumberOfCol, theStyle);
	    }
	}
      if (aIndex != 1)
	{
	  SHOW("Can't decide...");
	  aIndex=-1;
	}
      break;
    case 2: /* 3 styles: 2 of them are equal */
      if (compareStyle (aStyle, aStyle+1) == 0)
	{
	  copyStyle( theStyle, aStyle);
	}
      else if(compareStyle (aStyle, aStyle+2) == 0)
	{
	  copyStyle( theStyle, aStyle);
	}
      else if(compareStyle (aStyle+1, aStyle+2) == 0)
	{
	  copyStyle( theStyle, aStyle+1);
	}
      else
	{
	  aIndex=-1;
	}
      break;
    case -1:
    default:
      aIndex=-1;
      break;
    }
  return aIndex + 1;
}

/* > */
/* < eraseCharWithThisStyle */
void eraseCharWithThisStyle( char* theDataBuffer, struct t_style* theStyleBuffer, int theLength, struct t_style* theStyle)
{
  int i=0;

  ENTER("eraseCharWithThisStyle");
  for(i=0; i<theLength; i++)
    {
      if (compareStyle( theStyle, theStyleBuffer+i)==0)
	{
	  theDataBuffer[i]=0x20;
	}
    }
}
/* > */
/* < flushText */

void flushText( char* theDataBuffer, struct t_style* theStyleBuffer, int theLength)
{
  char c=theDataBuffer[ theLength];
  
  ENTER("flushText");

#ifdef DEBUG
  theDataBuffer[ theLength]=0;
  SHOW2("Text=>>>%s<<<\n",theDataBuffer);
  theDataBuffer[ theLength]=c;
#endif

}

/* > */
/* < testIfPortionsAreMenuItems */
int testIfPortionsAreMenuItems( struct t_linePortion* theLinePortion1, struct t_linePortion* theLinePortion2)
{
  ENTER("testIfPortionsAreMenuItems");
  SHOW3("* theLinePortion1: myFirstCol=%d, myLastCol=%d\n",theLinePortion1->myFirstCol, theLinePortion1->myLastCol);
  SHOW3(" Content modified=%d, style modified=%d\n",theLinePortion1->myContentIsModified, theLinePortion1->myStyleIsModified);
  SHOW3("* theLinePortion2: myFirstCol=%d, myLastCol=%d\n",theLinePortion2->myFirstCol, theLinePortion2->myLastCol);
  SHOW3(" Content modified=%d, style modified=%d\n",theLinePortion2->myContentIsModified, theLinePortion2->myStyleIsModified);
  return !theLinePortion1->myContentIsModified 
    && !theLinePortion2->myContentIsModified
    && theLinePortion1->myStyleIsModified
    && theLinePortion2->myStyleIsModified
    && ((theLinePortion1->myLastCol >= theLinePortion2->myFirstCol)
	|| (theLinePortion2->myLastCol >= theLinePortion1->myFirstCol));
}
/* > */
/* < lookForHighlightedMenuItem */
/* Among theLinePortion - array of two menu items - retreives the index of the highlighted one.
It guessed the background style and comapres it to the menu item style.

The returned value is:
* 0: theLinePortion[0] is the highlighted style.
* 1: theLinePortion[1] is the highlighted style.
* -1: no highlighted item found.
*/
int lookForHighlightedMenuItem( struct t_linePortion* theLinePortion, struct t_style* theStyleBuffer, int theNumberOfLine, int theNumberOfCol)
{
  /* Distinguish the top and bottom portions */
  struct t_linePortion* aTopPortion=theLinePortion;
  struct t_linePortion* aBottomPortion=theLinePortion;
  int aStatus=0;
  struct t_style aSurroundingBackgroundStyle;
  struct t_style aTopPortionBackgroundStyle;
  struct t_style aBottomPortionBackgroundStyle;
  int aLength=theLinePortion->myLastCol + 1 - theLinePortion->myFirstCol;

  ENTER("lookForHighlightedMenuItem");

  if (theLinePortion[0].myLine < theLinePortion[1].myLine)
    {
      aBottomPortion++; /* points to theLinePortion+1 */
    }
  else
    {
      aTopPortion++;  /* points to theLinePortion+1 */
    }

  /* Look for the background style of the top and bottom line portions */
  if(!getBackgroundStyle( aTopPortion->myLine, aTopPortion->myFirstCol, theStyleBuffer, aLength, theNumberOfLine, theNumberOfCol, &aTopPortionBackgroundStyle))
    {
      SHOW("Top portion: no background found");
      return -1;
    }

  if(!getBackgroundStyle( aBottomPortion->myLine, aBottomPortion->myFirstCol, theStyleBuffer, aLength, theNumberOfLine, theNumberOfCol, &aBottomPortionBackgroundStyle))
    {
      SHOW("Bottom portion: no background found");
      return -1;
    }

  /* Look for the background style of the surrounding line portions
     Two possible cases:
     - Contiguous line portions: we check if possible the line upper than aTopPortion or the line lower than aBottomPortion
     - Not contiguous: check the line lower than aTopPortion
  */
  /* Check if the lines are contiguous */
  if ( abs( aTopPortion->myLine - aTopPortion->myLine)==1)
    { /* yes, contiguous */
      if (aTopPortion->myLine > 0)
	{ /* Check the upper line */
	  aStatus=getBackgroundStyle( aTopPortion->myLine - 1, aTopPortion->myFirstCol, theStyleBuffer, aLength, theNumberOfLine, theNumberOfCol, &aSurroundingBackgroundStyle);
	}
      if (!aStatus && (aBottomPortion->myLine + 1 < theNumberOfLine))
	{ /* Check the lower line */
	  aStatus=getBackgroundStyle( aBottomPortion->myLine - 1, aBottomPortion->myFirstCol, theStyleBuffer, aLength, theNumberOfLine, theNumberOfCol, &aSurroundingBackgroundStyle);
	}
    }
  else
    { /* Not contiguous: check the line lower than aTopPortion */
      aStatus=getBackgroundStyle( aTopPortion->myLine + 1, aTopPortion->myFirstCol, theStyleBuffer, aLength, theNumberOfLine, theNumberOfCol, &aSurroundingBackgroundStyle);
    }

  /* If the surrounding background has been found, compare it to the style of the bottom and top line portion */
  if (aStatus)
    {
      aStatus=-1;
      if (0==compareStyle( &aTopPortionBackgroundStyle, &aSurroundingBackgroundStyle))
	{
	  if (0!=compareStyle( &aBottomPortionBackgroundStyle, &aSurroundingBackgroundStyle))
	    { /* Return the index of the bottom portion */
	      aStatus=(aBottomPortion==theLinePortion) ? 0 : 1;
	      SHOW("Bottom portion is highlighted");
	    }
	}
      else if (0==compareStyle( &aBottomPortionBackgroundStyle, &aSurroundingBackgroundStyle))
	{ /* return the index of the top portion */
	  aStatus=(aTopPortion==theLinePortion) ? 0 : 1;
	  SHOW("Top portion is highlighted");
	}
    }
  else
    {
      aStatus=-1;
      SHOW("No highlighted portion found");
    }
  return aStatus;
}

/* > */
/* < initPortion */
void initPortion( struct t_linePortion* thePortion, struct t_cursor* theCursor)
{
  ENTER("initPortion");
  SHOW3("myLine=%d, myCol=%d\n", theCursor->myLine, theCursor->myCol);
  thePortion->myLine=theCursor->myLine;
  thePortion->myFirstCol=theCursor->myCol;
  thePortion->myLastCol=0;
  thePortion->myContentIsModified=0;
  thePortion->myStyleIsModified=0;
}

/* > */
/* < flushPortion */

/* Process and display the stored line portions according to the style.
This function can:
- determine the possible selected item in a menu and displays it.
- take in account the style (single letter as shortcut, distinct voices)

* selected item
If there are 1 or 2 line portions without any content modification but with style change, it can be the new selected item and the previous one which are displayed. 
The current background style around the line portion is determined.

*/
void flushPortion( struct t_linePortion* theLinePortion, int* theLinePortionIndex, char* theDataBuffer, int theNumberOfLine, int theNumberOfCol, struct t_style* theStyleBuffer)
{
  ENTER("flushPortion");

  /* Firstly, look for menu items: 2 lines without any content modification but style change */ 
  if ((*theLinePortionIndex==1)
      && testIfPortionsAreMenuItems(theLinePortion, theLinePortion+1))
    {
      int i;
      int aCell;
      switch( i=lookForHighlightedMenuItem( theLinePortion, theStyleBuffer, theNumberOfLine, theNumberOfCol))
	{
	case 0:
	case 1:
	  aCell=getCell( theLinePortion[i].myLine, theLinePortion[i].myFirstCol, theNumberOfCol);	    
	  flushText( theDataBuffer + aCell, NULL, theLinePortion[i].myLastCol - theLinePortion[i].myFirstCol);
	  break;
	case -1:
	default:
	  for (i=0; i<2; i++)
	    {
	      aCell=getCell( theLinePortion[i].myLine, theLinePortion[i].myFirstCol, theNumberOfCol);	    
	      flushText( theDataBuffer + aCell, NULL, theLinePortion[i].myLastCol - theLinePortion[i].myFirstCol);
	    }
	  break;
	}
    }
  else
    { /* process each line portion */
      struct t_linePortion* aPortion=NULL;
      int aFirstCell=0;
      int aLastCell=0;
      int i=0;

      for (i=0; i<=*theLinePortionIndex; i++)
	{
	  struct t_style aStyle;
	  int aLength=0;
	  aPortion=theLinePortion+i;
	  aFirstCell=getCell(aPortion->myLine, aPortion->myFirstCol, theNumberOfCol);
	  aLastCell=getCell(aPortion->myLine, aPortion->myLastCol, theNumberOfCol);

	  aLength=aLastCell + 1 - aFirstCell;

	  if (aPortion->myContentIsModified)
	    {
	      flushText( theDataBuffer + aFirstCell, NULL, aLength);
	    }
	  else if (aPortion->myStyleIsModified)
	    {
	      int aStatus=getBackgroundStyle( aPortion->myLine, aPortion->myFirstCol, theStyleBuffer, aLength, theNumberOfLine, theNumberOfCol, &aStyle);

	      /* The characters associated with the background style are not displayed */
	      if (aStatus)
		{
		  eraseCharWithThisStyle( theDataBuffer + aFirstCell, theStyleBuffer + aFirstCell, aLength, &aStyle);
		}
	      flushText( theDataBuffer + aFirstCell, NULL, aLength);
	    }
	}
    }

/*   for (i=0; i<=*theLinePortionIndex; i++) */
/*     { */
/*       char c; */

/*       aPortion=theLinePortion + i; */
/*       aFirstCell=aPortion->myLine * theNumberOfCol + aPortion->myFirstCol; */
/*       aLastCell=aPortion->myLine * theNumberOfCol + aPortion->myLastCol; */
/*       c=theDataBuffer[ aLastCell+1]; */
/*       theDataBuffer[ aLastCell+1]=0; */
/*       SHOW3("portion %d=>>>%s<<<\n",i,theDataBuffer+aFirstCell); */
/*       SHOW2("myLine=%d\n",aPortion->myLine); */
/*       SHOW3("myFirstCol=%d, myLastCol=%d\n",aPortion->myFirstCol, aPortion->myLastCol); */
/*       theDataBuffer[ aLastCell+1]=c; */
/*     } */
  *theLinePortionIndex=-1;
}
/* > */
/* < setPortion */
/* 
Check if a new line portion is supplied.
If yes, try to store its features.
If no, update the current portion (column)
OUTPUT: theLinePortionIndex is the current line portion.
*/ 

enum t_linePortionStatus 
{
  SAME_PORTION,
  MAX_PORTION_REACHED,
  NEW_PORTION
};

enum t_linePortionStatus setPortion( struct t_cursor* theCursor, struct t_linePortion* theLinePortion, int* theLinePortionIndex)
{
  enum t_linePortionStatus aStatus=SAME_PORTION;
  ENTER("setPortion");

  if ((*theLinePortionIndex==-1) /* first portion */
      || (theLinePortion[ *theLinePortionIndex].myLine != theCursor->myLine))
    { /* a new portion */
      if (++*theLinePortionIndex >= MAX_LINE_PORTION)
	{
	  --*theLinePortionIndex;
	  aStatus=MAX_PORTION_REACHED;
	  SHOW("MAX_PORTION_REACHED");
	}
      else
	{ /* add a new portion */
	  initPortion( theLinePortion + *theLinePortionIndex, theCursor);
	  aStatus=NEW_PORTION;
	  SHOW("NEW_PORTION");
	}
    }
  else
    {
      theLinePortion[ *theLinePortionIndex].myLastCol=theCursor->myCol;
    }
  return aStatus;
}

/* > */
/* < storeChar, storeStyle */

/* Store the style (color, bold,...); return 1 if the style is new (the style of the cell has been changed) */
int storeStyle(int theCell, struct t_style* theStyle, struct t_style* theStyleBuffer)
{
  int aNewStyle=0;
  ENTER("storeStyle");
  
  /* Test if the style has changed */
  if (0 != compareStyle( theStyleBuffer+theCell, theStyle))
    {
      aNewStyle=1;
      copyStyle( theStyleBuffer+theCell, theStyle);
      SHOW("New Style!");
    }

  return aNewStyle;
}

/* Store the char; return 1 if the char is new (the cell content has been changed) */
int storeChar(int theCell, char theChar, char* theDataBuffer, int theNumberOfCol)
{
  int aNewChar=0;
  ENTER("storeChar");

  /* Check if the character has changed */
  if (theDataBuffer[ theCell] != theChar)
    {
      aNewChar=1;
      theDataBuffer[ theCell] = theChar;
      SHOW("New Char!");
    }

  return aNewChar;
}

/* > */
/* < copyNodes */

void copyModes( struct t_style* theDestination, struct t_style* theSource)
{
  /* First save the colors fields */
  int aForegroundColor=theDestination->myForegroundColor;
  int aBackgroundColor=theDestination->myBackgroundColor;
  ENTER("copyModes");

  copyStyle( theDestination, theSource);

  /* and restore colors */
  theDestination->myForegroundColor=aForegroundColor;
  theDestination->myBackgroundColor=aBackgroundColor;
}

/* > */
/* < eraseLine */
void eraseLine( char* theLine, int theLength)
{
  int i=0;
  ENTER("eraseLine");
  for (i=0;i<theLength;i++)
    {
      theLine[i]=0x20;
    }
}
/* > */
/* < deleteCharacter */
void deleteCharacter(char* theLine, int theErasedLength, int theTotalLength)
{
  ENTER("deleteCharacter");
  memmove(theLine, theLine+theErasedLength, theErasedLength);
  eraseLine(&(theLine[theTotalLength-theErasedLength]), theErasedLength);
}
/* > */
/* < main */

int main()
{
  enum StringCapacity aCapacity;
  struct t_cursor aCursor;
  struct t_cursor aSavedCursor;
  char* aDataBuffer=NULL;
  struct t_style* aStyleBuffer=NULL;
  int aNumberOfLine=30;
  int aNumberOfCol=30;
  int aNumberOfCell = aNumberOfLine * aNumberOfCol;
  struct t_linePortion aLinePortion[ MAX_LINE_PORTION];
  int aLinePortionIndex=-1;

#ifdef DEBUG
  /* TBD GC: debug */
  extern FILE *yyin;
  yyin=fopen("test/1e.txt","r");
#endif

  ENTER("main");

  initCursor( &aCursor, 0);
  copyCursor( &aSavedCursor, &aCursor);

  createBuffer( &aDataBuffer, &aStyleBuffer, aNumberOfCell);
  clearBuffer( aDataBuffer, aStyleBuffer, aNumberOfCell);

  while((aCapacity=yylex()))
    {
      DISPLAY_CAPACITY( aCapacity);

      switch(aCapacity)
	{
	case CLEAR:
	  aCursor.myCol=0;
	  aCursor.myLine=0;
	  clearBuffer( aDataBuffer, aStyleBuffer, aNumberOfCell);
	  break;
	case CUB1:
	  if (aCursor.myCol!=0)
	    {
	      aCursor.myCol--;
	    }
	  break;
	case CUD1:
	  aCursor.myLine++;
	  aCursor.myCol=0;
	  break;
	case NEL:
	  aCursor.myLine++;
	  aCursor.myCol=0;
	  break;
	case CUF1:
	  aCursor.myCol++;
	  break;
	case CUP:
	  aCursor.myCol=myParameters[0];
	  aCursor.myLine=myParameters[1];
	  break;
	case CUU:
	  aCursor.myLine-=myParameters[0];
	  break;
	case DCH: /* delete characters */
	  {
	    int aShift=myParameters[0]; /* number of characters to delete, the following chars shift to the left */
	    int aCell=getCell(aCursor.myLine, aCursor.myCol, aNumberOfCol);
	    deleteCharacter(&(aDataBuffer[ aCell]), aShift, aNumberOfCol-aShift);
	  }
	  break;
	case DCH1:
	  {
	    int aShift=1;
	    int aCell=getCell(aCursor.myLine, aCursor.myCol, aNumberOfCol);
	    deleteCharacter(&(aDataBuffer[ aCell]), aShift, aNumberOfCol-aShift);
	  }
	  break;
	case DL: /* delete lines */
	  break;
	case DL1:
	  break;
	case ECH: /* Erase characters */
	  {
	    int aErasedLength=myParameters[0];
	    int aCell=getCell(aCursor.myLine, aCursor.myCol, aNumberOfCol);
	    eraseLine(&(aDataBuffer[ aCell]), aErasedLength);
	  }
	  break;
	case ED: /* Clear the display after the cursor */
	  break;
	case EL1:
	  break;
	case HOME:
	  aCursor.myCol=0;
	  aCursor.myLine=0;
	  break;
	case HPA:
	  aCursor.myCol=myParameters[0];
	  break;
	case IL: /* several lines are added (the content is shifted to the bottom of the screen) */
	  break;
	case IL1:
	  break;
	case RC:
	  aCursor.myCol=aSavedCursor.myCol;
	  aCursor.myLine=aSavedCursor.myLine;
	  break;
	case SC:
	  aSavedCursor.myCol=aCursor.myCol;
	  aSavedCursor.myLine=aCursor.myLine;
	  break;
	case OP:
	  break;
	case SETB:
	  aCursor.myStyle.myBackgroundColor=myParameters[0];
	  DISPLAY_COLOR( "Background", myParameters[0]);
	  break;
	case SETF:
	  aCursor.myStyle.myForegroundColor=myParameters[0];
	  DISPLAY_COLOR( "Foreground", myParameters[0]);
	  break;
	case SGR:
	  copyModes(& aCursor.myStyle, (struct t_style*)myParameters);
	  DISPLAY_STYLE((struct t_style*)myParameters);
	  break;
	case VPA:
	  aCursor.myLine=myParameters[0];
	  break;
	case TEXTFIELD:
	  {
	    int aCell=getCell(aCursor.myLine, aCursor.myCol, aNumberOfCol);
	    if (storeChar( aCell, *yytext, aDataBuffer, aNumberOfCol))
	      { /* the content has been modified */
		aLinePortion[ aLinePortionIndex].myContentIsModified=1;
	      }
	    if (storeStyle( aCell, &(aCursor.myStyle), aStyleBuffer))
	      { /* the style has been modified */
		aLinePortion[ aLinePortionIndex].myStyleIsModified=1;
	      }  
	    aCursor.myCol++;
	  }
	  break;
	default:
	  break;
	}

      if( setPortion( &aCursor, aLinePortion, &aLinePortionIndex) == MAX_PORTION_REACHED)
	{
	  flushPortion( aLinePortion, &aLinePortionIndex, aDataBuffer, aNumberOfLine, aNumberOfCol, aStyleBuffer);

	  /* and build the new portion */ 
	  setPortion( &aCursor, aLinePortion, &aLinePortionIndex);	  
	}
    }

  flushPortion( aLinePortion, &aLinePortionIndex, aDataBuffer, aNumberOfLine, aNumberOfCol, aStyleBuffer);

  DISPLAY_BUFFER(aDataBuffer, aNumberOfLine, aNumberOfCol);

  free(aDataBuffer);
  free(aStyleBuffer);
  return 0;
}

/* > */
