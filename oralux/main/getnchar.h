#ifndef GETNCHAR
#define GETNCHAR 1
// ----------------------------------------------------------------------------
// main.c
// $Id: getnchar.h,v 1.1 2004/09/27 20:30:25 gcasse Exp $
// $Author: gcasse $
// Description: capturing each key pressed with aural feedback
// $Date: 2004/09/27 20:30:25 $ |
// $Revision: 1.1 $ |
// Copyright (C) 2003, 2004 Gilles Casse (gcasse@oralux.org)
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// ----------------------------------------------------------------------------

enum GNC_Answer {
  GNC_Ok,
  GNC_UpArrowKey,
  GNC_DownArrowKey,
};

typedef void HANDLER(char* theKeyPressed);

char* getnchar(int pf, int theNumberOfChars, int theDelayInTenthOfMilliSeconds, int theCharMustBeSpelled, enum GNC_Answer *theStatus, HANDLER theCallback);

enum GNC_Answer getLastKeyPressed();

#endif
