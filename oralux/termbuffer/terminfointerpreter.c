/* 
----------------------------------------------------------------------------
terminfointerpreter.c
$Id: terminfointerpreter.c,v 1.1 2005/07/16 17:38:29 gcasse Exp $
$Author: gcasse $
Description: an alpha stage terminfo interpreter
$Date: 2005/07/16 17:38:29 $ |
$Revision: 1.1 $ |
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

#include "terminfo2list.h"
#include "debug.h"

/* < terminfointerpreter */
void terminfointerpreter( terminfoEntry* theEntry, cursor* theSavedCursor)
{
  cursor* aCursor=&(theEntry->myStartingPosition);
  int aData1=(int)(theEntry->myData1);
  int aData2=(int)(theEntry->myData2);

  switch(theEntry->myCapacity)
    {
    case CLEAR:
/*       myContext.myLinePortionIndex=-1; */
      aCursor->myCol=0;
      aCursor->myLine=0;
/*       { /\* Erase the lines *\/  */
/* 	char* aOutput=NULL; */
/* 	struct t_cursor aFirstCursor; */
/* 	struct t_cursor aLastCursor; */
	
/* 	aFirstCursor.myLine=0; */
/* 	aFirstCursor.myCol=0; */
	
/* 	aLastCursor.myLine=myContext.myNumberOfLine - 1; */
/* 	aLastCursor.myCol=0; */
	    
/* 	eraseLine( this, & aFirstCursor, & aLastCursor, &(myContext.myDefaultStyle), & aOutput); */
/* 	if (aOutput != NULL) */
/* 	  { */
/* 	    free(aOutput); */
/* 	  } */
/*       } */
      break;
    case CUB1:
      if (aCursor->myCol!=0)
	{
	  aCursor->myCol--;
	}
      break;
    case CUD1:
      aCursor->myLine++;
      break;
    case CR:
      aCursor->myCol=0;
      break;
    case NEL:
      aCursor->myLine++;
      aCursor->myCol=0;
      break;
    case CUF1:
      aCursor->myCol++;
      break;
    case CUP:
      aCursor->myLine = (aData1 > 0) ? aData1 - 1 : aData1;
      aCursor->myCol = (aData2 > 0) ? aData2 - 1 : aData2;
      break;
    case CUU:
      aCursor->myLine-=aData1;
      break;
    case DCH: /* delete characters (shorter line) */
      /* aData1 gives the number of characters to delete */
/*       deleteCharacter( this, & myContext.myCursor, aData1, theOutput); */
      break;
    case DL: /* delete lines */
      /* aData1 gives the number of lines to delete */
/*       deleteLine( this, aData1, theOutput); */
      break;
    case ECH: /* Erase characters (same line length) */
      /* aData1 gives the number of characters to erase */
/*       eraseCharacter( this, & myContext.myCursor, aData1, & myContext.myCursor.myStyle, theOutput); */
      break;
    case ED: /* Clear the display after the cursor */
      {
/* 	struct t_cursor aFirstCursor; */
/* 	struct t_cursor aLastCursor; */
	
/* 	switch( aData1) */
/* 	  { */
/* 	  case 1: /\* erase from start to cursor *\/ */
/* 	    aFirstCursor.myLine=0; */
/* 	    aFirstCursor.myCol=0; */
/* 	    aLastCursor.myLine = myContext.myCursor.myLine; */
/* 	    aLastCursor.myCol = myContext.myCursor.myCol; */
/* 	    break; */
/* 	  case 2: /\* whole display *\/ */
/* 	    aFirstCursor.myLine=0; */
/* 	    aFirstCursor.myCol=0; */
/* 	    aLastCursor.myLine=myContext.myNumberOfLine - 1; */
/* 	    aLastCursor.myCol=myContext.myNumberOfCol - 1; */
/* 	    break; */
/* 	  case 0: /\* from cursor to end of display *\/ */
/* 	  default: */
/* 	    aFirstCursor.myLine = myContext.myCursor.myLine; */
/* 	    aFirstCursor.myCol = myContext.myCursor.myCol; */
/* 	    aLastCursor.myLine=myContext.myNumberOfLine - 1; */
/* 	    aLastCursor.myCol=myContext.myNumberOfCol - 1; */
/* 	    break; */
/* 	  } */
/* 	eraseLine( this, &aFirstCursor, &aLastCursor, &(myContext.myCursor.myStyle), theOutput); */
      }
      break;
    case EL:
      {
/* 	    struct t_cursor aFirstCursor; */
/* 	    struct t_cursor aLastCursor; */

/* 	    aFirstCursor.myLine = myContext.myCursor.myLine; */
/* 	    aFirstCursor.myCol = myContext.myCursor.myCol; */
/* 	    aLastCursor.myLine = aFirstCursor.myLine; */

/* 	    switch( aData1) */
/* 	      { */
/* 	      case 1: /\* erase from start of line to cursor *\/ */
/* 		aLastCursor.myCol=0; */
/* 		break; */
/* 	      case 2: /\* whole line *\/ */
/* 		aFirstCursor.myCol=0; */
/* 		aLastCursor.myCol=myContext.myNumberOfCol - 1; */
/* 		break; */
/* 	      case 0: /\* from cursor to end of line *\/ */
/* 	      default: */
/* 		aLastCursor.myCol=myContext.myNumberOfCol - 1; */
/* 		break; */
/* 	      } */
/* 	    eraseLine( this, &aFirstCursor, &aLastCursor, &(myContext.myCursor.myStyle), theOutput); */
      }
      break;
    case HOME:
      aCursor->myCol=0;
      aCursor->myLine=0;
      break;
    case HPA:
      aCursor->myCol=aData1 - 1;
      break;
    case ICH: /* insert n characters */
/* 	    insertCol( this, myContext.myCursor.myCol + aData1 - 1, &(myContext.myCursor.myStyle), theOutput); */
      break;
    case IL: /* several lines are added (the content is shifted to the bottom of the screen) */
      break;
    case RC:
      aCursor->myCol=theSavedCursor->myCol;
      aCursor->myLine=theSavedCursor->myLine;
      break;
    case SC:
      theSavedCursor->myCol=aCursor->myCol;
      theSavedCursor->myLine=aCursor->myLine;
      break;
    case SGR:
      copyStyle( &(theEntry->myStyle), (style*)(theEntry->myData1));
      break;
    case VPA:
      aCursor->myLine=(aData1 > 0) ? aData1 - 1 : aData1;
      break;
    case TEXTFIELD:
      break;
    default:
    case RMACS:
    case BEL:
      break;
    }
}
/* > */
