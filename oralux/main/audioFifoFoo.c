// ----------------------------------------------------------------------------
// audioFifoFoo.c
// $Id: audioFifoFoo.c,v 1.1 2004/09/27 20:30:29 gcasse Exp $
// $Author: gcasse $
// Description: Simulate audioFifo for test purpose
// $Date: 2004/09/27 20:30:29 $ |
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
#include <stdio.h>
#include <stdlib.h>
#include "constants.h"

void audioFifoInit()
{
}

void playOggVorbisFile( char* theFilename, int theDoubleIsFiltered, int theCallIsSynchronous)
{
  ENTER("playOggVorbisFile");
  const char* aCommand="ogg123 %s";
  char *aString=malloc(strlen(theFilename)+sizeof(aCommand));
  sprintf(aString, aCommand, theFilename);
  system(aString);
}

void clearAll()
{
  ENTER("clearAll");
}

void resayAll()
{
  ENTER("resayAll");
}

void audioFifoQuit(int theSoundMustBeFinished)
{
  ENTER("audioFifoQuit");
}

