/* 
----------------------------------------------------------------------------
tifilter2l.c
$Id: tifilter2l.c,v 1.1 2005/07/10 20:33:57 gcasse Exp $
$Author: gcasse $
Description: terminfo filter, two lines.
$Date: 2005/07/10 20:33:57 $ |
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
#include "tifilter.h"

/* < terminfofilter2lines */
void terminfofilter2lines (termapi* theTermAPI, chartype* theInputTerminfo, chartype* theOutputTerminfo)
{
  theOutputTerminfo=NULL;
}
/* > */

/* < interpretEscapeSequence */

void interpretEscapeSequence( struct t_termbuffer* this, FILE* theStream, chartype** theOutput)
{
  enum StringCapacity aCapacity;
  struct t_cursor* aCursor=&(this->myCursor);

  ENTER("interpretEscapeSequence");

  this->myLinePortionIndex=-1;

  yyin=theStream;

  while((aCapacity=yylex()))
    {
      DISPLAY_CAPACITY( aCapacity);

      switch(aCapacity)
	{
	case CLEAR:
	  this->myLinePortionIndex=-1;
	  aCursor->myCol=0;
	  aCursor->myLine=0;
	  { /* Erase the lines */ 
	    char* aOutput=NULL;
	    struct t_cursor aFirstCursor;
	    struct t_cursor aLastCursor;
	    
	    aFirstCursor.myLine=0;
	    aFirstCursor.myCol=0;
	    
	    aLastCursor.myLine=this->myNumberOfLine - 1;
	    aLastCursor.myCol=0;
	    
	    eraseLine( this, & aFirstCursor, & aLastCursor, &(this->myDefaultStyle), & aOutput);
	    if (aOutput != NULL)
	      {
		free(aOutput);
	      }
	  }
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
	  aCursor->myLine = (TheParameter[0] > 0) ? TheParameter[0] - 1 : TheParameter[0];
	  aCursor->myCol = (TheParameter[1] > 0) ? TheParameter[1] - 1 : TheParameter[1];
	  break;
	case CUU:
	  aCursor->myLine-=TheParameter[0];
	  break;
	case DCH: /* delete characters (shorter line) */
	    /* TheParameter[0] gives the number of characters to delete */
	    deleteCharacter( this, & this->myCursor, TheParameter[0], theOutput);
	  break;
	case DL: /* delete lines */
	    /* TheParameter[0] gives the number of lines to delete */
	    deleteLine( this, TheParameter[0], theOutput);
	  break;
	case ECH: /* Erase characters (same line length) */
	    /* TheParameter[0] gives the number of characters to erase */
	    eraseCharacter( this, & this->myCursor, TheParameter[0], & this->myCursor.myStyle, theOutput);
	  break;
	case ED: /* Clear the display after the cursor */
	  {
	    struct t_cursor aFirstCursor;
	    struct t_cursor aLastCursor;

	    switch( TheParameter[0])
	      {
	      case 1: /* erase from start to cursor */
		aFirstCursor.myLine=0;
		aFirstCursor.myCol=0;
		aLastCursor.myLine = this->myCursor.myLine;
		aLastCursor.myCol = this->myCursor.myCol;
		break;
	      case 2: /* whole display */
		aFirstCursor.myLine=0;
		aFirstCursor.myCol=0;
		aLastCursor.myLine=this->myNumberOfLine - 1;
		aLastCursor.myCol=this->myNumberOfCol - 1;
		break;
	      case 0: /* from cursor to end of display */
	      default:
		aFirstCursor.myLine = this->myCursor.myLine;
		aFirstCursor.myCol = this->myCursor.myCol;
		aLastCursor.myLine=this->myNumberOfLine - 1;
		aLastCursor.myCol=this->myNumberOfCol - 1;
		break;
	      }
	    eraseLine( this, &aFirstCursor, &aLastCursor, &(this->myCursor.myStyle), theOutput);
	  }
	  break;
	case EL:
	  {
	    struct t_cursor aFirstCursor;
	    struct t_cursor aLastCursor;

	    aFirstCursor.myLine = this->myCursor.myLine;
	    aFirstCursor.myCol = this->myCursor.myCol;
	    aLastCursor.myLine = aFirstCursor.myLine;

	    switch( TheParameter[0])
	      {
	      case 1: /* erase from start of line to cursor */
		aLastCursor.myCol=0;
		break;
	      case 2: /* whole line */
		aFirstCursor.myCol=0;
		aLastCursor.myCol=this->myNumberOfCol - 1;
		break;
	      case 0: /* from cursor to end of line */
	      default:
		aLastCursor.myCol=this->myNumberOfCol - 1;
		break;
	      }
	    eraseLine( this, &aFirstCursor, &aLastCursor, &(this->myCursor.myStyle), theOutput);
	  }
	  break;
	case HOME:
	  aCursor->myCol=0;
	  aCursor->myLine=0;
	  break;
	case HPA:
	  aCursor->myCol=TheParameter[0] - 1;
	  break;
	case ICH: /* insert n characters */
	    insertCol( this, this->myCursor.myCol + TheParameter[0] - 1, &(this->myCursor.myStyle), theOutput);
	  break;
	case IL: /* several lines are added (the content is shifted to the bottom of the screen) */
	  break;
	case RC:
	  aCursor->myCol=this->mySavedCursor.myCol;
	  aCursor->myLine=this->mySavedCursor.myLine;
	  break;
	case SC:
	  this->mySavedCursor.myCol=aCursor->myCol;
	  this->mySavedCursor.myLine=aCursor->myLine;
	  break;
	case SGR:
	  copyStyle(& aCursor->myStyle, &TheCurrentStyle);
	  DISPLAY_STYLE( &TheCurrentStyle);
	  break;
	case VPA:
	  aCursor->myLine=(TheParameter[0] > 0) ? TheParameter[0] - 1 : TheParameter[0];
	  break;
	case TEXTFIELD:
	    setChar( this, (char)*yytext, theOutput);
	  break;
	default:
	case RMACS:
	case BEL:
	  break;
	}
    }

  flushPortion( this, theOutput);

  DISPLAY_BUFFER(this->myDataBuffer, this->myStyleBuffer, this->myNumberOfLine, this->myNumberOfCol);

  checkString( theOutput);
}

/* > */
