// ----------------------------------------------------------------------------
// audioUserInterfaceFoo.c
// $Id: audioUserInterfaceFoo.c,v 1.1 2004/09/27 20:30:28 gcasse Exp $
// $Author: gcasse $
// Description: just for compilation
// $Date: 2004/09/27 20:30:28 $ |
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
#include <fcntl.h>

enum sentence
  {
    foo1, // for compilation
  };

void beep()
{
}

void enableSound(int the_sound_is_enable)
{
}

void sayChar(char c)
{
}
void say(enum sentence theSentence)
{
}

void beepLoudSpeaker()
{
  //  system("ogg123 -q /usr/share/oralux/audio/beep.ogg");
  //  system("echo -e \"\007\"");
}

// Storing the current tty file descriptor
static int pf=0;
void setTTY(int the_port)
{
  pf=the_port;
}

int getTTY()
{
  return pf;
}

int initAUI(char* theOggDirectory, int theLanguage, char* thePortName)
{
  int pf = open(thePortName, O_RDWR);
  if (pf < 0)
    {
      fprintf(stderr, "pb when opening tty %s\n", thePortName);
      return 0;
    }

  setTTY(pf);
  return 1;
}

void stopAUI(int theSoundMustBeFinished)
{
  close(pf);
}


 
