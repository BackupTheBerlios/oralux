// ----------------------------------------------------------------------------
// eparser.c
// $Id: eparser.c,v 1.1 2004/09/27 20:29:57 gcasse Exp $
// $Author: gcasse $
// Description: interprets the Emacspeak commands 
// $Date: 2004/09/27 20:29:57 $ |
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
// TBD: remove the assert
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>

#include <ctype.h>

#include <limits.h>

#include "command.h"
#include "fifo.h"
#include "synthAPI.h"
#undef DEBUG
#include "helpEmul.h"

//TBD: multi synth:
// array of struct 
// -> current queue + param
// -> pointer to a synth struct
//

static struct synthesizer* mySynth;
 
/* {{{ appendString */
void appendString(char** theOriginalString, char* theNewString)
{ 
  ENTER("appendString");
  char * aNewBuffer=malloc(strlen(*theOriginalString)+strlen(theNewString)+1);
  int i=sprintf(aNewBuffer, "%s", *theOriginalString);
  i+=sprintf(aNewBuffer+i, "%s", theNewString);
  free(*theOriginalString);
  SHOW2("New Buffer ==%s==\n",aNewBuffer);
  *theOriginalString=aNewBuffer;
}
/* }}} */

/* {{{ resetCallback */

int resetCallback( char *theLine, int *theContinuationIsExpected, void* theCommand)
{
  ENTER("resetCallback");
  purgeFifo();
  resetSynth( mySynth);
  *theContinuationIsExpected=0;
  return 1;
}

/* }}} */
/* {{{ fakeCallback */
int fakeCallback( char *theLine, int *theContinuationIsExpected, void* theCommand)
{
  ENTER("fakeCallback");
  *theContinuationIsExpected=0;
  return 1;
}
/* }}} */
/* {{{ setRateCallback */

int convertRate( int theRate)
{
  // if the rate = 235, we consider it is the normal value (100 per cent).
  // So, 100 is returned
  return (theRate*100 + 235/2)/235;
}

int setRateCallback( char *theLine, int *theContinuationIsExpected, void* theCommand)
{
  ENTER("setRateCallback");
  SHOW(theLine);

  // TBD [expr $tts(speech_rate) * $tts(char_factor)
  int aRate=atoi(theLine+sizeof("tts_set_speech_rate"));

  aRate=convertRate( aRate);

  setSpeechRateSynth( mySynth, aRate);

  *theContinuationIsExpected=0;
  return 1;
}
/* }}} */
/* {{{ queueTextCallback */

int queueTextCallback( char *theLine, int *theContinuationIsExpected, void* theCommand)
{
  ENTER("queueTextCallback");
  int aResult=1;
  struct t_command** aCommandPointer=(struct t_command **)theCommand;
  *theContinuationIsExpected=0;

  char* aBuffer=theLine;
  if ((*aCommandPointer)->myBuffer!=NULL)
    {
      appendString((char**)&((*aCommandPointer)->myBuffer), theLine);
      aBuffer=(*aCommandPointer)->myBuffer;
    }


  // TBD : note 
/*         if {$tts(midi) == 1} { */
/*             set mod [expr ($tts(q_tail) - $tts(q_head)) % 50] */
/*             if {$mod == 0}   { */
/*                 note 1 60 .5 */
/*             } */


  // expected format: "q {theText }"
  if (aBuffer[2]=='{')
    {
      char* aStart=&aBuffer[3];
      char* aEnd=strchr (aBuffer, '}');
      if (aEnd)
	{
	  SHOW("char } found");
	  *aEnd=0;

	  //	  if (strcmp(aStart, "[:np ] ")!=0)
	  //	  if (*aStart!='[')
	    {
	      pushStringToFifo(aStart);
	      // myBuffer is freed as soon as the command is deleted
	    }
	}
      else 
	{
	  SHOW("char } not found");
	  *theContinuationIsExpected=1;	
	  if ((*aCommandPointer)->myBuffer==NULL)
	    {
	      // as data to say, the \n must be inserted
	      char *aStoredLine=malloc(strlen(aBuffer)+2);
	      sprintf(aStoredLine, "%s\n", aBuffer);
	      *aCommandPointer=allocateCommand( *aCommandPointer, aStoredLine);
	    }
	}
    }
  else
    {
      SHOW2("Unexpected format: %s\n",aBuffer);
      aResult=0;
    }
  return aResult;
}

/* }}} */




/* {{{ queueRateCallback */

int queueRateCallback( char *theLine, int *theContinuationIsExpected, void* theCommand)
{
  ENTER("queueRateCallback");
  int aResult=1;
  *theContinuationIsExpected=0;

  // expected format: "r {rate}"
  if (theLine[2]=='{')
    {
      char* aStart=&theLine[3];
      char* aEnd=strchr (theLine, '}');
      if (aEnd!=NULL)
	{
	  SHOW("char } found");
	  *aEnd=0;

	  int aRate=convertRate( atoi(aStart));

	  // now we push in fifo the command
	  pushRateToFifo( aRate);
	  
	}
      else 
	{
	  SHOW("Error: char } not found");
	  aResult=0;
	}
    }
  else
    {
      SHOW2("Unexpected format: %s\n",theLine);
      aResult=0;
    }
  return aResult;
}

/* }}} */








/* {{{ letterCallback */

int letterCallback( char *theLine, int *theContinuationIsExpected, void* theCommand)
{
  ENTER("letterCallback");
  int aResult=1;
  *theContinuationIsExpected=0;

  // expected format: "l {character}"
  if (theLine[2]=='{')
    {
      char* aStart=&theLine[3];
      char* aEnd=strchr (theLine, '}');
      if (aEnd==aStart+1)
	{
	  SHOW("char } found");
	  *aEnd=0;
	  saySynth( mySynth, aStart, 1);
	}
      else 
	{
	  SHOW("Error: char } not found");
	  aResult=0;
	}
    }
  else
    {
      SHOW2("Unexpected format: %s\n",theLine);
      aResult=0;
    }
  return aResult;
}

/* }}} */

/* {{{ flushCallback */
int flushCallback( char *theLine, int *theContinuationIsExpected, void* theCommand)
{
  ENTER("flushCallback");
  struct t_fifoElement* anElement=popFifo();

  // TBD
/*     say "\[_]\[:sa c]\[:np]\[]\[:pu $mode]"  */
/*     if {$tts(old_rate) != $r } { */
/*         say "\[:ra $r]"  */
/*         set tts(old_rate) $r */
/*     } */



  while( anElement!=NULL)
    {
      switch(anElement->myType)
	{
	case ElementIsString:
	  {
	    char* aString = (char*)anElement->myValue;

	    // TBD [:i r 1] ajout d'une marque
	    saySynth( mySynth, aString, 0);
	  }
	  break;

	case ElementIsRate:
	  {
	    int* aRate = (int*)anElement->myValue;
	    setSpeechRateSynth( mySynth, *aRate); // TBD: in this case, is not forced in dtk-soft (setSpeechRateSynth forces it)
	  }
	  break;

	default:
	  break;
	}
      anElement=popFifo();
    }

  flushSynth( mySynth);
  *theContinuationIsExpected=0;
  return 1;
}
/* }}} */

/* {{{ stopCallback */
int stopCallback( char *theLine, int *theContinuationIsExpected, void* theCommand)
{
  ENTER("stopCallback");
  stopSynth( mySynth);
  purgeFifo();
  *theContinuationIsExpected=0;
  return 1;
}
/* }}} */
/* {{{ sayCallback */

int sayCallback( char *theLine, int *theContinuationIsExpected, void* theCommand)
{
  ENTER("sayCallback");

  setBoundarySynth( mySynth, SpeakOnEndOfWord);
  saySynth( mySynth, theLine, 0);
  *theContinuationIsExpected=0;
  return 1;
}

/* }}} */
/* {{{ sayCallback */

int synthCallback( char *theLine, int *theContinuationIsExpected, void* theCommand)
{
  ENTER("synthCallback");
  // tbd ajout de \O13
  // synth "\[:sa w]$text\013"
  setBoundarySynth( mySynth, SpeakOnEndOfWord);
  saySynth( mySynth, theLine, 1);
  *theContinuationIsExpected=0;
  return 1;
}

/* }}} */

/* {{{ commands (data) */

struct t_command myOneCharCommands[]=
  {
    {"d", SPEAK, flushCallback, NULL},
    {"l", LETTER, letterCallback, NULL},
    {"n", QUEUE_NOTE, fakeCallback, NULL},
    {"q", QUEUE_TEXT, queueTextCallback, NULL},
    {"r", QUEUE_SET_RATE, queueRateCallback, NULL},
    {"s", STOP, stopCallback, NULL},
    {"t", QUEUE_TONE, fakeCallback, NULL},
  };

struct t_command myCommandsPrefixedByTTS[]=
  {
    {"allcaps_beep", TTS_ALLCAPS_BEEP, fakeCallback, NULL},
    {"capitalize", TTS_CAPITALIZE, fakeCallback, NULL},
    {"pause", TTS_PAUSE, fakeCallback, NULL},
    {"reset", TTS_RESET, resetCallback, NULL},
    {"resume", TTS_RESUME, fakeCallback, NULL},
    {"say", TTS_SAY, synthCallback, NULL},
    {"set_punctuations", TTS_SET_PUNCTUATIONS, fakeCallback, NULL},
    {"set_speech_rate", TTS_SET_SPEECH_RATE, setRateCallback, NULL},
    {"set_character_scale", TTS_SET_CHARACTER_SCALE, fakeCallback, NULL},
    {"split_caps", TTS_SPLIT_CAPS, fakeCallback, NULL},
    {"sync_state", TTS_SYNC_STATE, fakeCallback, NULL},
  };

struct t_command myOthersCommands[]=
  {
    {"sh", SILENCE, fakeCallback, NULL},
    {"notes_initialize", NOTES_INITIALIZE, fakeCallback, NULL}, // see tts-lib.tcl
    {"notes_shutdown", NOTES_SHUTDOWN, fakeCallback, NULL},
    {"version", VERSION, fakeCallback, NULL},
  };

/* }}} */

/* {{{ getOneCharCommand */

struct t_command * getOneCharCommand( char* theLine)
{
  ENTER("getOneCharCommand");
  int i=0;

  struct t_command * aCommand=NULL;
  for(i=0; i<sizeof(myOneCharCommands)/sizeof(myOneCharCommands[0]); i++)
    {
      if (*theLine==myOneCharCommands[i].myLabel[0])
	{
	  aCommand=createCommand(&myOneCharCommands[i]);
	  break;
	}
    }
  return aCommand;
}

/* }}} */
/* {{{ getCommandPrefixedByTTS */

struct t_command * getCommandPrefixedByTTS( char* theLine)
{
  ENTER("getCommandPrefixedByTTS");
  int i=0;
  struct t_command * aCommand=NULL;
  
  // TBD: Getting the token
  char* aEnd=strchr( theLine, ' ');
  if (aEnd!=NULL)
    {
      *aEnd=0;
      for(i=0; i<sizeof(myCommandsPrefixedByTTS)/sizeof(myCommandsPrefixedByTTS[0]); i++)
	{
	  if (strcmp(theLine, myCommandsPrefixedByTTS[i].myLabel)==0)
	    {
	      SHOW("Label found");
	      aCommand=createCommand(&myCommandsPrefixedByTTS[i]);
	      break;
	    }
	}
      *aEnd=' ';
    }
  return aCommand;
}

/* }}} */
/* {{{ getOtherCommand */
struct t_command * getOtherCommand( char* theLine)
{
  ENTER("getOtherCommand");
  int i=0;
  struct t_command * aCommand=NULL;
  for(i=0; i<sizeof(myOthersCommands)/sizeof(myOthersCommands[0]); i++)
    {
      if (strcmp(theLine, myOthersCommands[i].myLabel)==0)
	{
	  aCommand=createCommand(&myOthersCommands[i]);
	  break;
	}
    }
  return aCommand;
}

/* }}} */
/* {{{ getCommand */
// The return pointer must be freed by the caller
struct t_command* getCommand(char* theLine)
{
  ENTER("getCommand");
  SHOW2("theLine=%s\n", theLine);
  struct t_command *aCommand=NULL;

  if ((theLine==0)||(*theLine==0))
    {
      aCommand=NULL;
    }
  else if ((theLine[1]==' ')||(theLine[1]==0))
    {
      aCommand=getOneCharCommand( theLine);
    }
  else if (strncmp(theLine,"tts_",4)==0)
    {
      aCommand=getCommandPrefixedByTTS( &theLine[4]);
    }
  else
    {
      aCommand=getOtherCommand( theLine);
    }

  if (aCommand==NULL)
    {
      SHOW("No returned command (NULL)");
    }
  else
    {
      SHOW(displayType[aCommand->myType]);
    }

  return aCommand;
}

/* }}} */

/* {{{ initParser */
void initParser()
{
  ENTER("initParser");
  initFifo();
  mySynth=initSynth();
}
/* }}} */
/* {{{ eparser */

void eparser(char* theBuffer )
{
  ENTER("eparser");
  char* aLine=NULL;
  char* aDuplicatedBuffer=strdup (theBuffer);
  char* aEndOfLine=aDuplicatedBuffer;
  //  aLine=strtok_r( theBuffer, "\n", &aEndOfLine);
  aLine=strsep( &aEndOfLine, "\n");

  // if false, the line concerns the previous command
  struct t_command * aCommand=NULL; 
  
  while(aLine)
    {
      SHOW2("\n--> Processing line ==%s==\n",aLine);
      if(aCommand==NULL)
	{
	  aCommand=getCommand(aLine);
	}

      if (aCommand!=NULL)
	{
	  int aContinuationIsExpected=1; 
	  (*(aCommand->myCallback))(aLine, &aContinuationIsExpected, &aCommand);
	  if (!aContinuationIsExpected)
	    {
	      deleteCommand(&aCommand);
	    }
	}
      aLine=strsep( &aEndOfLine, "\n");
    }
  free(aDuplicatedBuffer);

/*       const char* aLine="l {"; */
/*       if (strncmp(theLine,aLine,sizeof(aLine)-1)==0) */
/* 	{ */
/* 	  char aSentence[100]; */
/* 	  sprintf(aSentence, "[:say letter]%c[:say clause]", theLine[sizeof(aLine)-1]); */
/* 	  saySynth( mySynth, aSentence); */
/* 	} */
}

/* }}} */
/* {{{ stopParser */
void stopParser()
{
  purgeFifo();
  shutdownSynth( mySynth);
}
/* }}} */
/* {{{ unitary test */

// gcc -DDEBUG -I /ramdisk/dtk/include  -ltts -ltts_us  -o eparser -g eparser.c dectalkAPI.c helpEmul.c
/*   int main() */
/*   { */
/*     HELPEMULINIT; */
/*     initParser(); */
/*     eparser("tts_set_speech_rate 225\ntts_sync_state all 0 0 1 225\nl {a}\ns\nq {hello, world! \n}\n"); */
/*     eparser("q {how are you?\n}\n"); */
/*     synchronizeSynth( mySynth); */
/*     stopParser(); */
/*   } */

/* }}} */
