/* 
----------------------------------------------------------------------------
termapiSimu.c
$Id: termapiSimu.c,v 1.1 2005/07/16 17:38:29 gcasse Exp $
$Author: gcasse $
Description: testapi implementation for tests.
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
#include "termapi.h"

int _isLineContentModified( linePortion theLinePortion, const chartype* theNewData)
{
  return 0;
}

int _isLineStyleModified( linePortion theLinePortion, const style* theNewStyle)
{
  return 0;
}

int _getBackgroundStyle( linePortion theLinePortion, style* theStyle)
{
  clearStyle( theStyle);
  return 0;
}

int _getCursor( cursor* theCursor)
{
  theCursor->myLine=0;
  theCursor->myCol=0;
  clearStyle( & (theCursor->myStyle));
  return 0;
}

int _getDim( int* theNumberOfLine, int* theNumberOfCol)
{
  *theNumberOfLine=25;
  *theNumberOfCol=80;
  return 0;
}

void getTermapiSimu(termapi* theTermapi)
{
  theTermapi->isLineContentModified=_isLineContentModified;
  theTermapi->isLineStyleModified=_isLineStyleModified;
  theTermapi->getBackgroundStyle=_getBackgroundStyle;
  theTermapi->getCursor=_getCursor;
  theTermapi->getDim=_getDim;
}

