/* 
----------------------------------------------------------------------------
terminfo.c
$Id: terminfo.c,v 1.4 2004/12/26 21:40:05 gcasse Exp $
$Author: gcasse $
Description: store the layout using the supplied terminfo commands. 
$Date: 2004/12/26 21:40:05 $ |
$Revision: 1.4 $ |
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
/* > */
/* < debug */

#ifdef DEBUG
int TheDebugIsOn=1;

static char* TheENTERFilename="  ";

#define ENTER(a) \
if (strcmp(TheENTERFilename,__FILE__)!=0) \
{\
  TheENTERFilename=__FILE__;\
  printf("\nFILE %s\n",__FILE__);\
}\
printf("== ENTER %s (%d)\n",a,__LINE__)

#define SHOW(a) printf("%s\n",a)
#define SHOW1(a) printf("%s\n",a)
#define SHOW2(a,b) printf(a,b)
#define SHOW3(a,b,c) printf(a,b,c)
#define LEAVE(a) printf("LEAVE %s\n",a)
#define LEAVE2(a,b) printf("LEAVE %s (%d)\n",a,b)
#else
int TheDebugIsOn=0;

#define ENTER(a)
#define SHOW(a)
#define SHOW1(a)
#define SHOW2(a,b)
#define SHOW3(a,b,c)
#define LEAVE(a)
#define LEAVE2(a,b)
#endif

/* > */
/* < flex declarations or definitions */

extern int yylex (void);
extern char *yytext;
extern int yyleng;

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
/* < array myStringCapacity */

char* myStringCapacity[]={
  "",
  "ACSC",
  "CBT",
  "BEL",
  "CR",
  "CPI",
  "LPI",
  "CHR",
  "CVR",
  "CSR",
  "RMP",
  "TBC",
  "MGC",
  "CLEAR",
  "EL1",
  "EL",
  "ED",
  "HPA",
  "CMDCH",
  "CWIN",
  "CUP",
  "CUD1",
  "HOME",
  "CIVIS",
  "CUB1",
  "MRCUP",
  "CNORM",
  "CUF1",
  "LL",
  "CUU1",
  "CVVIS",
  "DEFC",
  "DCH1",
  "DL1",
  "DIAL",
  "DSL",
  "DCLK",
  "HD",
  "ENACS",
  "SMACS",
  "SMAM",
  "BLINK",
  "BOLD",
  "SMCUP",
  "SMDC",
  "DIM",
  "SWIDM",
  "SDRFQ",
  "SMIR",
  "SITM",
  "SLM",
  "SMICM",
  "SNLQ",
  "SNRMQ",
  "PROT",
  "REV",
  "INVIS",
  "SSHM",
  "SMSO",
  "SSUBM",
  "SSUPM",
  "SMUL",
  "SUM",
  "SMXON",
  "ECH",
  "RMACS",
  "RMAM",
  "SGR0",
  "RMCUP",
  "RMDC",
  "RWIDM",
  "RMIR",
  "RITM",
  "RLM",
  "RMICM",
  "RSHM",
  "RMSO",
  "RSUBM",
  "RSUPM",
  "RMUL",
  "RUM",
  "RMXON",
  "PAUSE",
  "HOOK",
  "FLASH",
  "FF",
  "FSL",
  "WINGO",
  "HUP",
  "IS1",
  "IS2",
  "IS3",
  "IF",
  "IPROG",
  "INITC",
  "INITP",
  "ICH1",
  "IL1",
  "IP",
  "KA1",
  "KA3",
  "KB2",
  "KBS",
  "KBEG",
  "KCBT",
  "KC1",
  "KC3",
  "KCAN",
  "KTBC",
  "KCLR",
  "KCLO",
  "KCMD",
  "KCPY",
  "KCRT",
  "KCTAB",
  "KDCH1",
  "KDL1",
  "KCUD1",
  "KRMIR",
  "KEND",
  "KENT",
  "KEL",
  "KED",
  "KEXT",
  "KF0",
  "KF1",
  "KF10",
  "KF11",
  "KF12",
  "KF13",
  "KF14",
  "KF15",
  "KF16",
  "KF17",
  "KF18",
  "KF19",
  "KF2",
  "KF20",
  "KF21",
  "KF22",
  "KF23",
  "KF24",
  "KF25",
  "KF26",
  "KF27",
  "KF28",
  "KF29",
  "KF3",
  "KF30",
  "KF31",
  "KF32",
  "KF33",
  "KF34",
  "KF35",
  "KF36",
  "KF37",
  "KF38",
  "KF39",
  "KF4",
  "KF40",
  "KF41",
  "KF42",
  "KF43",
  "KF44",
  "KF45",
  "KF46",
  "KF47",
  "KF48",
  "KF49",
  "KF5",
  "KF50",
  "KF51",
  "KF52",
  "KF53",
  "KF54",
  "KF55",
  "KF56",
  "KF57",
  "KF58",
  "KF59",
  "KF6",
  "KF60",
  "KF61",
  "KF62",
  "KF63",
  "KF7",
  "KF8",
  "KF9",
  "KFND",
  "KHLP",
  "KHOME",
  "KICH1",
  "KIL1",
  "KCUB1",
  "KLL",
  "KMRK",
  "KMSG",
  "KMOV",
  "KNXT",
  "KNP",
  "KOPN",
  "KOPT",
  "KPP",
  "KPRV",
  "KPRT",
  "KRDO",
  "KREF",
  "KRFR",
  "KRPL",
  "KRST",
  "KRES",
  "KCUF1",
  "KSAV",
  "SHIFTKBEG",
  "SHIFTKCAN",
  "SHIFTKCMD",
  "SHIFTKCPY",
  "SHIFTKCRT",
  "SHIFTKDC",
  "SHIFTKDL",
  "KSLT",
  "SHIFTKEND",
  "SHIFTKEOL",
  "SHIFTKEXT",
  "KIND",
  "SHIFTKFND",
  "SHIFTKHLP",
  "SHIFTKHOM",
  "SHIFTKIC",
  "SHIFTKLFT",
  "SHIFTKMSG",
  "SHIFTKMOV",
  "SHIFTKNXT",
  "SHIFTKOPT",
  "SHIFTKPRV",
  "SHIFTKPRT",
  "KRI",
  "SHIFTKRDO",
  "SHIFTKRPL",
  "SHIFTKRIT",
  "SHIFTKRES",
  "SHIFTKSAV",
  "SHIFTKSPD",
  "KHTS",
  "SHIFTKUND",
  "KSPD",
  "KUND",
  "KCUU1",
  "RMKX",
  "SMKX",
  "LF0",
  "LF1",
  "LF10",
  "LF2",
  "LF3",
  "LF4",
  "LF5",
  "LF6",
  "LF7",
  "LF8",
  "LF9",
  "FLN",
  "RMLN",
  "SMLN",
  "RMM",
  "SMM",
  "MHPA",
  "MCUD1",
  "MCUB1",
  "MCUF1",
  "MVPA",
  "MCUU1",
  "NEL",
  "PORDER",
  "OC",
  "OP",
  "PAD",
  "DCH",
  "DL",
  "CUD",
  "MCUD",
  "ICH",
  "INDN",
  "IL",
  "CUB",
  "MCUB",
  "CUF",
  "MCUF",
  "RIN",
  "CUU",
  "MCUU",
  "PFKEY",
  "PFLOC",
  "PFX",
  "PLN",
  "MC0",
  "MC5P",
  "MC4",
  "MC5",
  "PULSE",
  "QDIAL",
  "RMCLK",
  "REP",
  "RFI",
  "RS1",
  "RS2",
  "RS3",
  "RF",
  "RC",
  "VPA",
  "SC",
  "IND",
  "RI",
  "SCS",
  "SGR",
  "SETB",
  "SMGB",
  "SMGBP",
  "SCLK",
  "SCP",
  "SETF",
  "SMGL",
  "SMGLP",
  "SMGR",
  "SMGRP",
  "HTS",
  "SMGT",
  "SMGTP",
  "WIND",
  "SBIM",
  "SCSD",
  "RBIM",
  "RCSD",
  "SUBCS",
  "SUPCS",
  "HT",
  "DOCR",
  "TSL",
  "TONE",
  "UC",
  "HU",
  "U0",
  "U1",
  "U2",
  "U3",
  "U4",
  "U5",
  "U6",
  "U7",
  "U8",
  "U9",
  "WAIT",
  "XOFFC",
  "XONC",
  "ZEROM",
  "SCESA",
  "BICR",
  "BINEL",
  "BIREP",
  "CSNM",
  "CSIN",
  "COLORNM",
  "DEFBI",
  "DEVT",
  "DISPC",
  "ENDBI",
  "SMPCH",
  "SMSC",
  "RMPCH",
  "RMSC",
  "GETM",
  "KMOUS",
  "MINFO",
  "PCTRM",
  "PFXL",
  "REQMP",
  "SCESC",
  "S0DS",
  "S1DS",
  "S2DS",
  "S3DS",
  "SETAB",
  "SETAF",
  "SETCOLOR",
  "SMGLR",
  "SLINES",
  "SMGTB",
  "EHHLM",
  "ELHLM",
  "ELOHLM",
  "ERHLM",
  "ETHLM",
  "EVHLM",
  "SGR1",
  "SLENGTH",
  "TEXT...",
};

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
/* < displayBuffer */

void displayBuffer( char *theBuffer, int theMaxLine, int theMaxCol)
{
  /* Init */
  int i=0;
  ENTER("displayBuffer");
  for (i=0;i<theMaxLine;i++)
    {
      char* aLine=theBuffer + theMaxCol*i;
      char c=aLine[theMaxCol-1];
      aLine[theMaxCol-1]=0;
      printf("%2d#%s%c\n", i, aLine, c);
      aLine[theMaxCol-1]=c;
    }
}

/* > */
/* < displayCapacity */
void displayCapacity( enum StringCapacity theCapacity)
{ /* debug: display pattern */
  char* aLine=strdup( yytext);
  char* aString=aLine;
  ENTER("displayCapacity");

  while((aString=strchr (aString, '')))
    {
      *aString='E';
    }
  printf("|%s = %s|\n", myStringCapacity[ theCapacity], aLine);
  free(aLine);
}

/* > */
/* < displayModes */
void displayModes(struct t_style* theModes)
{
  ENTER("displayModes");
  printf("|");
  if (theModes->isStandout)
    {
      printf("STANDOUT ");
    }
  if (theModes->isUnderline)
    {
      printf("UNDERLINE ");
    }
  if (theModes->isReverse)
    {
      printf("REVERSE ");
    }
  if (theModes->isBlink)
    {
      printf("BLINK ");
    }
  if (theModes->isDim)
    {
      printf("DIM ");
    }
  if (theModes->isBold)
    {
      printf("BOLD ");
    }
  if (theModes->isBlank)
    {
      printf("BLANK ");
    }
  if (theModes->isProtect)
    {
      printf("PROTECT ");
    }

  if (theModes->isAlternate)
    {
      printf("ALTERNATE_CHARSET ");
    }
  printf(" |\n");
}

/* > */
/* < displayColor */
void displayColor( int theColor)
{
  static char* aColorArray[]=
    {
      "BLACK",
      "RED",
      "GREEN",
      "YELLOW",
      "BLUE",
      "MAGENTA",
      "CYAN",
      "WHITE"
    };
  ENTER("displayColor");

  if (theColor<sizeof(aColorArray)/sizeof(aColorArray[0]))
    {
      printf("|Color: %s|\n",aColorArray[theColor]);
    }
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
If yes, this stykle is copied in theFoundStyle and 1 is returned. 
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

int getBackgroundStyle( char* theDataBuffer, int theLine, int theCol, struct t_style* theStyleBuffer, int theLength, int theNumberOfLine, int theNumberOfCol, struct t_style* theStyle)
{
  enum {MAX_DISTINCT_STYLE=3};
  struct t_style aStyle[MAX_DISTINCT_STYLE];
  int aStyleWeight[MAX_DISTINCT_STYLE];
  int aIndex=0;
  int i=0;
  int aCell=getCell( theLine, theCol, theNumberOfCol);

  ENTER("getBackgroundStyle");

  theStyle=NULL;

  copyStyle( aStyle+aIndex, theStyleBuffer+aCell+i);
  aStyleWeight[ aIndex]=1;

  for(i=1; i<theLength; i++)
    {
      if (compareStyle (aStyle+aIndex, theStyleBuffer+aCell+i) == 0)
	{
	  ++aStyleWeight[ aIndex];
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
      SHOW3("style %d: %d samples", i, aStyleWeight[i]);
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
	      SHOW("Top Left Cell");
	      aIndex=compareAndSetStyle( aStyle, theStyleBuffer, theLine-1, theCol-1, theNumberOfCol, theStyle);
	    }
	  if((theCol+theLength < theNumberOfCol) && (aIndex != 1))
	    {
	      SHOW("Top Right Cell");
	      aIndex=compareAndSetStyle( aStyle, theStyleBuffer, theLine-1, theCol+theLength, theNumberOfCol, theStyle);
	    }
	}
      /* otherwise check the bottom cells */
      if ((theLine+1 < theNumberOfLine) && (aIndex != 1))
	{
	  if (theCol > 0)
	    {
	      SHOW("Bottom Left Cell");
	      aIndex=compareAndSetStyle( aStyle, theStyleBuffer, theLine+1, theCol-1, theNumberOfCol, theStyle);
	    }
	  if((theCol+theLength < theNumberOfCol) && (aIndex != 1))
	    {
	      SHOW("Bottom Right Cell");
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
/* < testIfPortionsAreItems */
int testIfPortionsAreItems( struct t_linePortion* theLinePortion1, struct t_linePortion* theLinePortion2)
{
  ENTER("testIfPortionsAreItems");
  return !theLinePortion1->myContentIsModified 
    && !theLinePortion2->myContentIsModified
    && theLinePortion1->myStyleIsModified
    && theLinePortion2->myStyleIsModified;
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

/* process and display the stored line portions according to the style.
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

  /* Look for two lines without any content modification but style change */ 
  if ((*theLinePortionIndex==1)
      && testIfPortionsAreItems(theLinePortion, theLinePortion+1))
    {
      
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
	      int aStatus=getBackgroundStyle( theDataBuffer, aPortion->myLine, aPortion->myFirstCol, theStyleBuffer, aLength, theNumberOfLine, theNumberOfCol, &aStyle);

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
  /* RAF GC: debug */
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
      if (TheDebugIsOn)
	{
	  displayCapacity( aCapacity);
	}

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
	  if (TheDebugIsOn)
	    {
	      displayColor( myParameters[0]);
	    }
	  break;
	case SETF:
	  aCursor.myStyle.myForegroundColor=myParameters[0];
	  if (TheDebugIsOn)
	    {
	      displayColor( myParameters[0]);
	    }
	  break;
	case SGR:
	  copyModes(& aCursor.myStyle, (struct t_style*)myParameters);
	  if (TheDebugIsOn)
	    {
	      displayModes((struct t_style*)myParameters);
	    }
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

  if (TheDebugIsOn)
    {
      displayBuffer(aDataBuffer, aNumberOfLine, aNumberOfCol);
    }

  free(aDataBuffer);
  free(aStyleBuffer);
  return 0;
}

/* > */
