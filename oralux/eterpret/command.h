#ifndef COMMAND_H
#define COMMAND_H 1
// ----------------------------------------------------------------------------
// command.h
// $Id: command.h,v 1.1 2004/09/27 20:29:58 gcasse Exp $
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

// If a new enum is added, the displayType array must be updated too 
enum t_commandType
  {
    UNDEFINED,
    LETTER,
    NOTES_INITIALIZE,
    NOTES_SHUTDOWN,
    QUEUE_NOTE,
    QUEUE_SET_RATE,
    QUEUE_TEXT,
    QUEUE_TONE,
    SILENCE,
    SPEAK,
    STOP,
    TTS_ALLCAPS_BEEP,
    TTS_CAPITALIZE,
    TTS_PAUSE,
    TTS_RESET,
    TTS_RESUME,
    TTS_SAY,
    TTS_SET_CHARACTER_SCALE,
    TTS_SET_PUNCTUATIONS,
    TTS_SET_SPEECH_RATE,
    TTS_SPLIT_CAPS,
    TTS_SYNC_STATE,
    VERSION,
    MAX_COMMAND, // Last element!
  };

char* displayType[MAX_COMMAND];

typedef int (*t_CommandCallback)(char*, int*, void *);

struct t_command {
  char* myLabel;
  enum t_commandType myType;
  t_CommandCallback myCallback;
  void* myBuffer;
};


struct t_command * createCommand( struct t_command * theSourceCommand);
void deleteCommand( struct t_command ** theCommand);
struct t_command * allocateCommand( struct t_command * theSourceCommand, void* theNewBuffer);

#endif
