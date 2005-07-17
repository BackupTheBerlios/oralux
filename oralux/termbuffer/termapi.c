/* 
----------------------------------------------------------------------------
termapi.c
$Id: termapi.c,v 1.1 2005/07/17 17:06:59 gcasse Exp $
$Author: gcasse $
Description: termapi utilities
$Date: 2005/07/17 17:06:59 $ |
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
#include "termapi.h"
#include "debug.h"

void createLinePortion (linePortion* theLinePortion, int theLine, int theCol, int theLastCol, style* theStyle, chartype* theString)
{
  ENTER("createLinePortion");
  theLinePortion->myLine=theLine;
  theLinePortion->myFirstCol=theCol;
  theLinePortion->myLastCol=theLastCol;
  copyStyle(&(theLinePortion->myStyle), theStyle);
  theLinePortion->myString=theString;
  SHOW2("theLine=%d\n",theLine);
  SHOW2("theCol=%d\n",theCol);
  SHOW2("theLastCol=%d\n",theLastCol);
  SHOW2("theString=%s\n",theString);
  DISPLAY_STYLE(theStyle);
}
