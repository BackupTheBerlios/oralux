// ----------------------------------------------------------------------------
// speakup.c
// $Id: speakup.c,v 1.1 2006/03/25 22:11:55 gcasse Exp $
// $Author: gcasse $
// Description: Speakup configuration file. 
// $Date: 2006/03/25 22:11:55 $ |
// $Revision: 1.1 $ |
// Copyright (C) 2004, 2005 Gilles Casse (gcasse@oralux.org)
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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/sysmacros.h>
#include <errno.h>

#include "constants.h"
#include "textToSpeech.h"
#include "audioUserInterface.h"
#include "speakup.h"

/* < Data */
struct TextToSpeechCheatCode
{
  enum textToSpeech myIdentifier;
  char* myCode;
};

static struct TextToSpeechCheatCode mySpeakupCheatCode[]={
  {TTS_AccentSA, "acntsa"},
  {TTS_AccentPC, "acntpc"},
  {TTS_Audapter, "audptr"},
  {TTS_BrailleLite, "bns"},
  {TTS_DECtalkExpress, "dectlk"},
  {TTS_DECtalkExternal, "decext"},
  {TTS_DolphinApollo,"apolo"},
  {TTS_DoubleTalkPC, "dtlk"},
  {TTS_KeynoteGoldPC, "keypc"},
  {TTS_DoubleTalkLT, "ltlk"},
  {TTS_SpeakOut, "spkout"},
  {TTS_Transport, "txprt"},
};

#define MaxCheatcode (sizeof(mySpeakupCheatCode)/sizeof(mySpeakupCheatCode[0]))

static int myVolume=0;
static char* mySynth=NULL;
#define DEV_SOFTSYNTH "/dev/softsynth"
#define PROC_SPEAKUP_VOL "/proc/speakup/vol"
#define PROC_SPEAKUP_SYNTH_NAME "/proc/speakup/synth_name"

/* > */

static void setVol( int theValue)
{
  ENTER("setVol");
  FILE* fd = fopen (PROC_SPEAKUP_VOL, "w");
  if (fd)
    {
      if (theValue > 9)
	{
	  theValue = 9;
	}
      fprintf(fd,"%d\n",theValue);
      fclose(fd);
    }
}

static int getVol()
{
  ENTER("getVol");
  int aValue=0;
  FILE* fd = fopen (PROC_SPEAKUP_VOL, "r");
  if (fd != NULL)
    {
      char buf[8];
      fgets(buf, sizeof(buf), fd);
      fclose(fd);
      
      if (('0'<=buf[0]) && (buf[0]<='9'))
	{
	  aValue = buf[0]-'0';
	}
    }

  return aValue;
}

static void setSynth( char* theSynthName)
{
  ENTER("setSynth");
  if (theSynthName)
    {
      FILE* fd = fopen (PROC_SPEAKUP_SYNTH_NAME, "w");
      if (fd)
	{
	  fprintf(fd,"%s\n",theSynthName);
	  fclose(fd);
	}
    }
}

static char* convertSynth( enum textToSpeech theIdentifier)
{
  ENTER("convertSynth");

  int i;
  char* aSynthName = NULL;

  for (i=0; i<MaxCheatcode; i++)
    {
      if (mySpeakupCheatCode[i].myIdentifier == theIdentifier)
	{
	  aSynthName = mySpeakupCheatCode[i].myCode;
	  break;
	}
    }
  return aSynthName;
}

int initSpeakup(struct textToSpeechStruct* theTextToSpeech)
{
  ENTER("initSpeakup");

  system("pkill speechd-up 2>/dev/null;multispeech-up-ctl stop 2>/dev/null");
  setSynth("none");

  struct stat buf;
  if (-1 == lstat(DEV_SOFTSYNTH, &buf))
    {
      if ((errno != ENOENT)
	  || (-1 == mknod(DEV_SOFTSYNTH, S_IFCHR|0644, makedev(10,26))))
	{
	  perror("initSpeakup");
	  return 1;
	}
    }

  // set the new synth
  char* aSynthName = convertSynth( theTextToSpeech->myIdentifier);
  if (aSynthName)
    {
      setSynth( aSynthName);

      switch( theTextToSpeech->myIdentifier)
	{
	case TTS_Flite:
	  system("speechd-up");
	  break;
	  
	case TTS_Multispeech:
	  system("multispeech-up-ctl start");
	  break;
	  
	default:
	  break;
	}
    }

  return 0;
}

/* < disableSpeakup, enableSpeakup */
void disableSpeakup()
{
  ENTER("disableSpeakup");

  myVolume = getVol();
  setVol(0);
}

void enableSpeakup( struct menuInfo* theSelectedInfo)
{
  ENTER("enableSpeakup");

  if ((theSelectedInfo->myDesktop == Speakup)
      && (0 == initSpeakup( &(theSelectedInfo->myTextToSpeech))))
    {
      setVol(myVolume);
      system("su - knoppix 'speakupconf'");
    }
}

/* > */

