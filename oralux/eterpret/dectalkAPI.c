// ----------------------------------------------------------------------------
// dectalkAPI.c
// $Id: dectalkAPI.c,v 1.1 2004/09/27 20:29:58 gcasse Exp $
// $Author: gcasse $
// Description: DECtalk software speech server, implements synthAPI
// $Date: 2004/09/27 20:29:58 $ |
// $Revision: 1.1 $ |
// Copyright (C) 2004 Gilles Casse (gcasse@oralux.org)
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
#include <string.h>
#include <dtk/ttsapi.h>
#include "helpEmul.h"
#include "synthAPI.h"

static LPTTS_HANDLE_T dtkHandle;

static struct synthesizer myPreviousSettings;

/* {{{ setSynthStyle */


int setStyleSynth( struct synthesizer* this, struct t_styleSynth theStyle)
{
  return 1;
}

/* }}} */

/* {{{ BuildConversionArray */

#define CHAR2CHARSIZE 256
static unsigned char Char2Char[CHAR2CHARSIZE];

static void BuildConversionArray()
{
  ENTER("BuildConversionArray");
  int i;
  for (i=0; i<CHAR2CHARSIZE; i++)
    {
      Char2Char[i]=i;
    }
}
static void BuildFrenchConversionArray()
{
  ENTER("BuildFrenchConversionArray");
  int i;
  for (i=0; i<CHAR2CHARSIZE; i++)
    {
      if (i<0x20)
	{
	  switch(i)
	    {
	    case 0xa:
	    case 0xb:
	    case 0xd:
	    case 0x9:
	      Char2Char[i]=i;
	      break;
	    default:
	      Char2Char[i]='?';
	      break;
	    }
	}
      else if(i<0xA0)
	{
	  Char2Char[i]=i;
	}
      else
	{
	  switch(i)
	    {
	    case 0xc0:
	    case 0xc1:
	    case 0xc2:
	    case 0xc3:
	    case 0xc4:
	    case 0xc5:
	    case 0xc6:
	      Char2Char[i]='a';
	      break;
	    case 0xc7:
	      Char2Char[i]='ç';
	      break;
	    case 0xc8:
	      Char2Char[i]='è';
	      break;
	    case 0xc9:
	      Char2Char[i]='é';
	      break;
	    case 0xca:
	      Char2Char[i]='ê';
	      break;
	    case 0xcb:
	    case 0xeb:
	      Char2Char[i]='e';
	      break;
	    case 0xcc:
	    case 0xcd:
	    case 0xce:
	    case 0xcf:
	    case 0xec:
	    case 0xed:
	    case 0xee:
	    case 0xef:
	      Char2Char[i]='i';
	      break;
	    case 0xd1:
	    case 0xf1:
	      Char2Char[i]='n';
	      break;
	    case 0xd2:
	    case 0xd3:
	    case 0xd4:
	    case 0xd5:
	    case 0xd6:
	    case 0xd8:
	    case 0xf2:
	    case 0xf3:
	    case 0xf4:
	    case 0xf5:
	    case 0xf6:
	    case 0xf8:
	      Char2Char[i]='o';
	      break;
	    case 0xd7:
	      Char2Char[i]='x';
	      break;
	    case 0xd9:
	    case 0xda:
	    case 0xdb:
	    case 0xdc:
	    case 0xf9:
	    case 0xfa:
	    case 0xfb:
	    case 0xfc:
	      Char2Char[i]='u';
	      break;
	    case 0xdd:
	    case 0xfd:
	    case 0xff:
	      Char2Char[i]='y';
	      break;
	    case 0xe0:
	    case 0xe1:
	    case 0xe2:
	    case 0xe3:
	    case 0xe4:
	    case 0xe5:
	    case 0xe6:
	    case 0xe7:
	    case 0xe8:
	    case 0xe9:
	    case 0xea:
	      Char2Char[i]=i;
	      break;
	    default:
	      Char2Char[i]='?';
	      break;
	    } //switch
	} //else
    } //for
}

/* }}} */
/* {{{ shutdownSynth */
int shutdownSynth( struct synthesizer* this) {
  ENTER("shutdownSynth");
  SHOW("TextToSpeechShutdown");
  int aStatus=( TextToSpeechShutdown(dtkHandle) != MMSYSERR_NOERROR);
  free(this);
  return aStatus;
}
/* }}} */
/* {{{ initSynth */
struct synthesizer* initSynth() 
{
  HELPEMULINIT;
  ENTER("initSynth");
  int status;

  struct synthesizer* this=malloc(sizeof(struct synthesizer*));

  unsigned int devNo = WAVE_MAPPER;
  //  DWORD devOptions = OWN_AUDIO_DEVICE;
  DWORD devOptions = 0;
  unsigned int UShandle;

  char* aLanguage="us";
  char* aTTSLang=(char*)getenv("LANGUAGE");

  if (strcmp(aTTSLang,"de")==0)
    {
      aLanguage="gr";
    }
  else if (strcmp(aTTSLang,"es")==0)
    {
      aLanguage="sp";
    }
  else if (strcmp(aTTSLang,"fr")==0)
    {
      aLanguage="fr";
    }

  LPLANG_ENUM languageINFO;

  SHOW("TextToSpeechEnumLangs");
  if (TextToSpeechEnumLangs(&languageINFO)==0)
  {
    SHOW("Error : TextToSpeechEnumLangs");
    free(this);
    exit(1);
  }
  int aKnownLanguage=0;
  if (languageINFO->MultiLang)
  {
    int i;
    for(i=0;i<languageINFO->Languages;i++)
    {
      SHOW3("LANGUAGE=%s, TTS=%s\n",aLanguage,languageINFO->Entries[i].lang_code);

      if (strcmp(aLanguage,languageINFO->Entries[i].lang_code)==0)
      {
        aKnownLanguage=1;
        break;
      }
    } 
  }

  if (!aKnownLanguage)
  {
    aLanguage="us";
  }

  SHOW("TextToSpeechStartLang");
  UShandle=TextToSpeechStartLang(aLanguage);
  if (UShandle & TTS_LANG_ERROR) {
    free(this);
    exit(1);
  }

  SHOW("TextToSpeechSelectLang");
  if (TextToSpeechSelectLang(NULL, UShandle)==FALSE)
  {
    SHOW1("Error selecting language");
    SHOW("TextToSpeechCloseLang");
    TextToSpeechCloseLang(aLanguage);
    free(this);
    exit(1);
  }

  if (strcmp(aLanguage,"fr")==0)
    {
      BuildFrenchConversionArray();
    }
  else
    {
      BuildConversionArray();
    }

  SHOW1("Calling tts startup.");
  SHOW("TextToSpeechStartup");
  status = TextToSpeechStartup( &dtkHandle, devNo,
                                devOptions, NULL, 0 );

  if (status != MMSYSERR_NOERROR)
    {
      SHOW1("Pb init");
      SHOW2("tts startup returned %d\n", status);
    }

  //TBD GC

/*     Via  Software DecTalk, This,  is   Eamakspeak!  */

  saySynth(this, "[:phoneme arpabet speak on ]\n[:pu s ]\n[:sa c]\n[:np]\nSoftware DecTalk is ready.\n", 1);

  return this;
}

/* }}} */
/* {{{ flushSynth */
int flushSynth(struct synthesizer* this)
{
  ENTER("flushSynth");
  saySynth(this, "", 1);
  return 1;
}
/* }}} */
/* {{{ saySynth */

int saySynth(struct synthesizer* this, char* theString, int theOutputIsForced) 
{
  ENTER("saySynth");
  SHOW2("theString=%s\n",theString);

#define DESTLEN 10000
  unsigned char dst[DESTLEN];
  int i=0;

  if (theOutputIsForced==0)
    {
      i=sprintf(dst,"[_][:sa c][:np][][:pu none]");

      if (myPreviousSettings.myStyle.myVoice.myVoiceRate == this->myStyle.myVoice.myVoiceRate)
	{
	  i+=sprintf(dst+i, "[:ra %d]", this->myStyle.myVoice.myVoiceRate);
	}
    }

  for (; i<strlen(theString); i++)
    {
      dst[i]=Char2Char[(unsigned char)theString[i]];
      if (dst[i]==11)
	{
	  dst[i]=0;
	}
    }
  dst[i]=0;

  if (strlen(dst)==1)
    {
      //      i=sprintf(dst, "[:say letter]%c[:say clause]", *dst);
      i=sprintf(dst, " %c ", *dst);
      SHOW2("dst=%s\n",dst);
    }
  else
    {
      // TBD see type (speech_task) : if s  i=sprintf(dst, "[:i r 1]%s\013", dst);
      //...
      SHOW2("dst=%s\n",dst);
    }

/*   if {$tts(old_rate) != $r } { */
/*     say "\[:ra $r]"  */
/*       set tts(old_rate) $r */
/*     } */

  SHOW2("TextToSpeechSpeak TTS_NORMAL: %s\n", dst);
  if (TextToSpeechSpeak(dtkHandle, dst, TTS_NORMAL) != MMSYSERR_NOERROR )
    {
    //TBD:previous value TTS_NORMAL) != MMSYSERR_NOERROR )
      SHOW("Speech error");
    }

  SHOW("TextToSpeechSpeak TTS_FORCE: empty\n");
  if (TextToSpeechSpeak(dtkHandle, "", TTS_FORCE) != MMSYSERR_NOERROR )
    {
    //TBD:previous value TTS_NORMAL) != MMSYSERR_NOERROR )
      SHOW("Speech error");
    }

  return 1;

/*   if (Tcl_StringMatch(Tcl_GetStringFromObj(objv[0],NULL), "synth")) { */
/*     PRINT("TextToSpeechSpeak, empty, FORCE"); */
/*     rc = TextToSpeechSpeak((dtkHandle, */
/*                            "", */
/*                            TTS_FORCE); */
/*     if (rc != MMSYSERR_NOERROR) { */
/*       Tcl_SetResult(interp, "Internal tts synth error", TCL_STATIC); */
/*       return TCL_ERROR; */
/*     } */
/*   } */

}

/* }}} */
/* {{{ synchronizeSynth */

int synchronizeSynth( struct synthesizer* this) 
{
  ENTER("synchronizeSynth");
  SHOW("TextToSpeechSync");
  int rc = TextToSpeechSync(dtkHandle);
  if (!rc) {
    SHOW1("ko synth\n");
    return 0;
  }
  return 1;
}

/* }}} */
/* {{{ stopSynth */

int stopSynth( struct synthesizer* this) 
{
  ENTER("stopSynth");
  SHOW("TextToSpeechReset");
  TextToSpeechReset (dtkHandle, FALSE);

  char* dst="[:phoneme arpabet speak on :say clause]";
  SHOW2("TextToSpeechSpeak TTS_NORMAL: %s\n", dst);
  TextToSpeechSpeak(dtkHandle,dst,TTS_NORMAL);
  return 1;
}

/* }}} */
/* {{{ pauseSynth */

int pauseSynth( struct synthesizer* this, int theDelayInMilliseconds) {
  ENTER("pauseSynth");
  static int nb=0;
  if (!nb)
  {
    nb=1;
    return 1;
  }
  
  SHOW("TextToSpeechPause");
  if (TextToSpeechPause(dtkHandle)) return 1;
  return 0;
}

/* }}} */
/* {{{ resumeSynth */

int resumeSynth( struct synthesizer* this)
{
  SHOW("TextToSpeechResume");
  if (TextToSpeechResume(dtkHandle)) return 1;
  return 0;
}

/* }}} */
/* {{{ setSynthLanguage */
int setSynthLanguage( struct synthesizer* this) 
{
  ENTER("setSynthLanguage");
  //  sprintf(aFilename, "%s/%s", getenv("HOME"), FILE_LANG);
  //  FILE* fp=fopen (aFilename, "w");
  //  fprintf(fp,"%s", txt);
  //  fclose(fp);
//  system("sync");
  return 1;
}
/* }}} */
/* {{{ resetSynth */

int resetSynth( struct synthesizer* this)
{
  SHOW("TextToSpeechReset");
  TextToSpeechReset (dtkHandle,0);

  stopSynth( this);
  saySynth(this, "[:phoneme arpabet speak on ]\n[:pu s ]\n[:sa c]\nReset. Software DecTalk ready\n", 1);
  return 1;
}

/* }}} */

/* {{{ setSpeechRate */
int setSpeechRateSynth( struct synthesizer* this, int theRate)
{
  // if theRate=100 (100 percent = normal rate), we consider it is 200 words per minute 
  int aRate=theRate*2;
  if (aRate > 600)
    {
      aRate=600;
    }

  // saving the previous value
  myPreviousSettings.myStyle.myVoice.myVoiceRate = this->myStyle.myVoice.myVoiceRate;
  this->myStyle.myVoice.myVoiceRate=aRate;

/*   //ko #define StringFormat "[:rate %3d]" */
/*   //ok #define StringFormat " [:rate %3d] \n [:nh] hello . \n " */
/*   //ko #define StringFormat " [:np ] [:rate %3d] \n hello . \n " */
/*   //ok #define StringFormat "[_][:sa c][:np ][][:pu none][:rate %3d]" */
/*   // StringFormat:  */
/*   // Note: as sizeof uses StringFormat, '%3d' is useful to reserve the correct length (three characters) */
/*   //  */

/*   // TBD a supprimer a terme, sera placé dans saySynth */

/*   #define StringFormat "[:sa c][:rate %3d]" */
/*   char aBuffer[ sizeof(StringFormat)]; */
/*   sprintf( aBuffer, StringFormat, aRate); */
/*   saySynth( this, aBuffer, 1); */
  return 1;
}
/* }}} */

/* {{{ setBoundarySynth */

int setBoundarySynth( struct synthesizer* this, enum t_boundarySynth theBoundary)
{
  //  myBoundary=theBoundary;
  return 1;
}

/* }}} */
