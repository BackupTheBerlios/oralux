// ----------------------------------------------------------------------------
// command.c
// $Id: command.c,v 1.1 2004/09/27 20:29:58 gcasse Exp $
// $Author: gcasse $
// Description: Emacspeak command
// $Date: 2004/09/27 20:29:58 $ |
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
#include "command.h"
#include <stdlib.h>
#include "helpEmul.h"

/* {{{ displayType */

char* displayType[MAX_COMMAND]=
  {
    "UNDEFINED",
    "LETTER",
    "NOTES_INITIALIZE",
    "NOTES_SHUTDOWN",
    "QUEUE_NOTE",
    "QUEUE_SET_RATE",
    "QUEUE_TEXT",
    "QUEUE_TONE",
    "SILENCE",
    "SPEAK",
    "STOP",
    "TTS_ALLCAPS_BEEP",
    "TTS_CAPITALIZE",
    "TTS_PAUSE",
    "TTS_RESET",
    "TTS_RESUME",
    "TTS_SAY",
    "TTS_SET_CHARACTER_SCALE",
    "TTS_SET_PUNCTUATIONS",
    "TTS_SET_SPEECH_RATE",
    "TTS_SPLIT_CAPS",
    "TTS_SYNC_STATE",
    "VERSION",
  };

/* }}} */


/* {{{ createCommand */
struct t_command * createCommand( struct t_command * theSourceCommand)
{
  ENTER("createCommand");
  return theSourceCommand;
}
/* }}} */
/* {{{ deleteCommand */
void deleteCommand( struct t_command ** theCommand)
{
  ENTER("deleteCommand");
  if (*theCommand!=NULL)
    {
      if ((*theCommand)->myBuffer!=NULL)
	{ // allocateCommand has been called
	  SHOW("free pointers");
	  free((*theCommand)->myBuffer);
	  free(*theCommand);
	}
      *theCommand=NULL;
    }
}
/* }}} */

/* {{{ allocateCommand */
struct t_command * allocateCommand( struct t_command * theSourceCommand, void* theNewBuffer)
{
  ENTER("allocateCommand");
  struct t_command * aNewCommand=malloc(sizeof(struct t_command));
  aNewCommand->myLabel=theSourceCommand->myLabel;
  aNewCommand->myType=theSourceCommand->myType;
  aNewCommand->myCallback=theSourceCommand->myCallback;
  aNewCommand->myBuffer=theNewBuffer;
  return aNewCommand;
}
/* }}} */

