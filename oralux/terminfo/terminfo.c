/* 
----------------------------------------------------------------------------
terminfo.c
$Id: terminfo.c,v 1.3 2004/12/19 23:05:53 gcasse Exp $
$Author: gcasse $
Description: store the layout using the supplied terminfo commands. 
$Date: 2004/12/19 23:05:53 $ |
$Revision: 1.3 $ |
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

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "escape2terminfo.h"

#ifdef DEBUG
int TheDebugIsOn=1;
#else
int TheDebugIsOn=0;
#endif

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
  *theDataBuffer=(char*)malloc( theNumberOfCell * sizeof(char));
  *theStyleBuffer=(struct t_style*)malloc( theNumberOfCell * sizeof(struct t_style));
}

/* > */
/* < clearBuffer */

void clearBuffer( char* theDataBuffer, struct t_style* theStyleBuffer, int theNumberOfCell)
{
  memset(theDataBuffer, 0x20, theNumberOfCell*sizeof(char));
  memset(theStyleBuffer, 0, theNumberOfCell*sizeof(struct t_style));
}

/* > */
/* < displayBuffer */

void displayBuffer( char *theBuffer, int theMaxLine, int theMaxCol)
{
  /* Init */
  int i=0;
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
  theCursor->myLine=theFirstCell;
  theCursor->myCol=theFirstCell;
  memset (& (theCursor->myStyle), 0, sizeof(struct t_style));
  theCursor->myStyle.myBackgroundColor=TERM_COLOR_BLACK;
  theCursor->myStyle.myForegroundColor=TERM_COLOR_WHITE;
}
/* > */
/* < copyCursor */
void copyCursor( struct t_cursor* theDestination, struct t_cursor* theSource)
{
  memcpy( theDestination, theSource, sizeof(struct t_cursor));
}

/* > */
/* < storeChar */

/* Store the char and its style (color, bold,...) return the possible changes (char or style) */
enum t_storeChar { 
  SAME_CHAR, /* the char and its style equal the previously ones displayed  */
  NEW_CHAR, /* the char has changed */
  NEW_STYLE /* the style has changed */
};
 
enum t_storeChar storeChar(struct t_cursor* theCursor, char theChar, char* theDataBuffer, struct t_style* theStyleBuffer, int theNumberOfCol)
{
  enum t_storeChar aStatus=SAME_CHAR;
  int aCell=theCursor->myLine * theNumberOfCol + theCursor->myCol;
  
  /* Check if the character has changed */
  if (theDataBuffer[ aCell] != theChar)
    {
      aStatus=NEW_CHAR;
      theDataBuffer[ aCell] = theChar;
    }

  /* If this is still the same text, test if the style has changed */
  if (0 != memcmp( &(theStyleBuffer[ aCell]), &(theCursor->myStyle), sizeof(struct t_style) ))
    {
      aStatus=NEW_STYLE;
      memcpy( &(theStyleBuffer[ aCell]), &(theCursor->myStyle), sizeof(struct t_style));
    }
  return aStatus;
}

/* > */
/* < isNewWord, processWord */

int isNewWord( struct t_cursor* theCursor, char theChar)
{
/*   static int aLastCol=-1; */
/*   static int aLastLine=-1; */
/*   static int aNewWord=((aLastLine != theCursor->myLine)  */
/* 		       || (abs( aLastCol - theCursor->myCol) != 1 ) */
/* 		       || (*yytext==0x20)); */
/*   static int aSameText=1; /\* true if the written text is the same than the previous one *\/ */
/*   static int aSameStyle=1; */
  return 1;
}

char* processWord( struct t_cursor* theLastWord, enum t_storeChar theStatus)
{
  return NULL;
}

/* > */
/* < copyNodes */

void copyModes( struct t_style* theDestination, struct t_style* theSource)
{
  /* First save the colors fields */
  int aForegroundColor=theDestination->myForegroundColor;
  int aBackgroundColor=theDestination->myBackgroundColor;

  /* then raw copy */
  memcpy( theDestination, theSource, sizeof(struct t_style));

  /* and restore colors */
  theDestination->myForegroundColor=aForegroundColor;
  theDestination->myBackgroundColor=aBackgroundColor;
}

/* > */
/* < eraseLine */

void eraseLine( char* theLine, int theLength)
{
  int i=0;
  for (i=0;i<theLength;i++)
    {
      theLine[i]=0x20;
    }
}

/* > */
/* < deleteCharacter */

void deleteCharacter(char* theLine, int theErasedLength, int theTotalLength)
{
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
  enum terminalColor aBackgroundColor=TERM_COLOR_BLUE; /* RAF determine the main area background */
  char* aDataBuffer=NULL;
  struct t_style* aStyleBuffer=NULL;
  int aNumberOfLine=30;
  int aNumberOfCol=30;
  int aNumberOfCell = aNumberOfLine * aNumberOfCol;

#ifdef DEBUG
  /* RAF GC: debug */
  extern FILE *yyin;
  yyin=fopen("test/1e.txt","r");
#endif

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
	    deleteCharacter(&(aDataBuffer[aCursor.myLine*aNumberOfCol+aCursor.myCol]), aShift, aNumberOfCol-aShift);
	  }
	  break;
	case DCH1:
	  {
	    int aShift=1;
	    deleteCharacter(&(aDataBuffer[aCursor.myLine*aNumberOfCol+aCursor.myCol]), aShift, aNumberOfCol-aShift);
	  }
	  break;
	case DL: /* delete lines */
	  break;
	case DL1:
	  break;
	case ECH: /* Erase characters */
	  {
	    int aErasedLength=myParameters[0];
	    eraseLine(&(aDataBuffer[aCursor.myLine*aNumberOfCol+aCursor.myCol]), aErasedLength);
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
	default:
	  {
	    /* TBD: init,...*/
	    static struct t_cursor aLastWord;
	    static struct t_cursor aLastChar;
	    enum t_storeChar aStatus=NEW_CHAR;

	    if (isNewWord(&aCursor, *yytext))
	      {
		char* aString=processWord( &aLastWord, aStatus);
		if (aString)
		  {
		    printf(">>>Word: %s<<<\n", aString);
		    free( aString);
		  }
		copyCursor( &aLastWord, &aCursor);
	      }
	    copyCursor( &aLastChar, &aCursor);

	    aStatus=storeChar(&aCursor, *yytext, aDataBuffer, aStyleBuffer, aNumberOfCol);
	    switch (aStatus)
	      {
	      case SAME_CHAR:
		break;
	      case NEW_CHAR:
		break;
	      case NEW_STYLE:
		break;
	      }
	    aCursor.myCol++;
	  }
	}
    }

  if (TheDebugIsOn)
    {
      displayBuffer(aDataBuffer, aNumberOfLine, aNumberOfCol);
    }

  free(aDataBuffer);
  free(aStyleBuffer);
  return 0;
}

/* > */

