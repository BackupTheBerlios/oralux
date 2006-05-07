/* tcldtk.c,v 21.0 2004/11/25 18:46:03 raman */
/* <copyright*/
/**
 *Copyright (C) 1995 -- 2003, T. V. Raman 
 *All Rights Reserved
 *
 * This file is not part of GNU Emacs, but the same permissions apply.
 *
 * GNU Emacs is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * GNU Emacs is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Emacs; see the file COPYING.  If not, write to
 * the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 */
/*
// ###
// From: Gilles Casse 22 March 2003
// ATTENTION
// This file is not the original one. 
//
// The original file is changed mostly according to the current behavior of the beta french DECtalk software.
// The tts is initialized -if possible- according to the environment variable LANGUAGE.
//
// General changes:
// - Avoiding pb when emacspeak starts with DECtalk (function Pause)
// - UTF-8 to 8859-1 conversion (function Say)
//
// Specific changes for the beta french TTS
// - The default TTS language (function Tcldtk_Init) is - if possible - set according to the environment variable LANGUAGE
// - Filtering the characters which crash dtk (functions Tcldtk_Init and Say)
// - A few temporary lines are added to trace the tcldtk.c functions
// 
// 20 July 2003: selecting language (french, us,...)
// 
// 27 March 2004: Tcldtk_Init: new initialization values for DECtalk 5.0
// October 2004: taking in account the changes from Tim Cross for DECtalk 5.0:
//  http://www-personal.une.edu.au/~tcross
// ###
*/
/* > */
/* <headers*/

#include <tcl.h>
#include <tclExtend.h>
#include <stdlib.h>
#include <string.h>
#undef VOID
#include <dtk/ttsapi.h>
/* > */

/* debug */
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


/* <defines*/

#define PACKAGENAME "tts"
#define PACKAGEVERSION "1.0"

#define EXPORT
extern  EXPORT int Tcldtk_Init(Tcl_Interp *interp);

#define USE_LOG_FILE 0
#define LOG_FILE_NAME "/tmp/tcldtk.log"
#define DEBUG_LEVEL 0

/* > */
/* <prototypes*/

int openLog();
void debugMsg(char *, int, char *);
char *getErrorMsg(MMRESULT);

void TclDtkFree(ClientData);
int Say(ClientData, Tcl_Interp *, int, Tcl_Obj * CONST []);
int Stop(ClientData, Tcl_Interp *, int, Tcl_Obj * CONST []);
int SetLanguage(ClientData, Tcl_Interp *, int, Tcl_Obj * CONST []);

int Synchronize(ClientData, Tcl_Interp *, int, Tcl_Obj * CONST []);
int Pause(ClientData, Tcl_Interp *, int, Tcl_Obj * CONST []);
int Resume(ClientData, Tcl_Interp *, int, Tcl_Obj * CONST []);

#define CHAR2CHARSIZE 256
static unsigned char Char2Char[CHAR2CHARSIZE];
static void BuildConversionArray()
{
  int i;
  for (i=0; i<CHAR2CHARSIZE; i++)
    {
      Char2Char[i]=i;
    }
}

static void BuildFrenchConversionArray()
{
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
	    } 
	} 
    } 
}
/* > */
/* <global variables*/

char *error_msg;
char error_buff[80];
FILE *LOG = NULL;

/* > */
/* <openLog*/

int openLog(void) {

  if (USE_LOG_FILE) {
	LOG = fopen(LOG_FILE_NAME, "w");
	setvbuf(LOG, NULL, _IONBF, 0);
	if (LOG == NULL) {
	  return TCL_ERROR;
	}
	return TCL_OK;
  } else {
	LOG = stderr;
  }
  return TCL_OK;
}

char *getErrorMsg(MMRESULT errno) {

  switch (errno) {
  case MMSYSERR_NOERROR:
	return "Success - No Error";
  case MMSYSERR_ERROR:
	return "Error - Unspecified error";
  case MMSYSERR_BADDEVICEID:
	return "Error - Device ID out of range";
  case MMSYSERR_NOTENABLED:
	return "Error - Driver failed to be enabled";
  case MMSYSERR_ALLOCATED:
	return "Error - Device already allocated";
  case MMSYSERR_INVALHANDLE:
	return "Error - Device handle is invalid";
  case MMSYSERR_NODRIVER:
	return "Error - No device driver present";
  case MMSYSERR_NOMEM:
	return "Error - Memory allocation error";
  case MMSYSERR_NOTSUPPORTED:
	return "Error - Function is not supported";
  case MMSYSERR_BADERRNUM:
	return "Error - Error value out of range";
  case MMSYSERR_INVALFLAG:
	return "Error - Invalid flag passed";
  case MMSYSERR_INVALPARAM:
	return "Error - Invalid parameter passed";
  case MMSYSERR_HANDLEBUSY:
	return "Error - Handle being used in another thread";
  case MMSYSERR_LASTERROR:
	return "Error - Last error in range";
  default:
	sprintf(error_buff, "Error - Unrecognized error number: %d", errno);
	return error_msg;
  }
  return "Opps - shouldn't have got to here!\n";
}

/* > */
/* <debugMsg*/

void debugMsg(char *id, int level, char *msg) {
  
  if (level <= DEBUG_LEVEL) {
	fprintf(LOG, "%s: %s\n", id, msg);
  }
}

/* > */
/* <closing down*/

void TclDtkFree(ClientData dtkHandle) {
  MMRESULT status;
  debugMsg("TclDtkFree", 10, "Entering function");

  status = TextToSpeechShutdown( dtkHandle );
  if (status != MMSYSERR_NOERROR) {
	error_msg = getErrorMsg(status);
	debugMsg("TclDtkFree", 1, error_msg);
  }
  debugMsg("TclDtkFree", 10, "Successful return from function");
}

/* > */
/* <init*/

int Tcldtk_Init(Tcl_Interp *interp) {
  MMRESULT status;
  LPTTS_HANDLE_T dtkHandle;
  unsigned int devNo = 0;
  DWORD devOptions = 0;
  unsigned int UShandle;
  char* aLanguage="us";
  char* aTTSLang=(char*)getenv("LANGUAGE");

  LPLANG_ENUM languageINFO;
  int aKnownLanguage=0;
  
  devNo = WAVE_MAPPER;
  if (openLog() == TCL_ERROR) {
	sprintf(error_buff, "Error in openLog()\n");
	Tcl_SetObjResult(interp, Tcl_NewStringObj(error_msg, -1));
	return TCL_ERROR;
  }

  if (Tcl_PkgProvide(interp, PACKAGENAME, PACKAGEVERSION) != TCL_OK) {
	sprintf(error_buff, "Error loading %s\n", PACKAGENAME);
    Tcl_SetObjResult(interp, Tcl_NewStringObj(error_msg, -1));
    return TCL_ERROR;
  }

  dtkHandle=NULL;

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

  if (TextToSpeechEnumLangs(&languageINFO)==0)
  {
    exit(1);
  }
  if (languageINFO->MultiLang)
  {
    int i;
    for(i=0;i<languageINFO->Languages;i++)
    {

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

  UShandle=TextToSpeechStartLang( aLanguage);
  if (UShandle & TTS_LANG_ERROR) {
    exit(1);
  }

  if (TextToSpeechSelectLang(NULL, UShandle)==FALSE)
  {
    TextToSpeechCloseLang(aLanguage);
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

  debugMsg("Tcldtk_Init", 5, "Calling TTS startup.");
  status = TextToSpeechStartup(&dtkHandle, devNo, devOptions, NULL, 0);
  sprintf(error_buff, "TTS startup returned %d", status);
  debugMsg("Tcldtk_init", 5, error_buff);
  if (status != MMSYSERR_NOERROR) {
	error_msg = getErrorMsg(status);
	debugMsg("Tcldtk_Init", 0, error_msg);
	Tcl_SetObjResult(interp, Tcl_NewStringObj(error_msg, -1));
	return TCL_ERROR;
  }

  if (dtkHandle == NULL) {
	sprintf(error_buff, "Could not open text-to-speech engine");
	debugMsg("Tcldtk_Init", 1, error_msg);
	Tcl_SetObjResult(interp, Tcl_NewStringObj(error_msg, -1));
	return TCL_ERROR;
  }
  
  debugMsg("Tcldtk_Init", 10, "Creating Tcl commands");
  Tcl_CreateObjCommand(interp, "say", Say, (ClientData) dtkHandle, TclDtkFree);
  Tcl_CreateObjCommand(interp, "synth", Say, (ClientData) dtkHandle, NULL);
  Tcl_CreateObjCommand(interp, "synchronize", Synchronize,
					   (ClientData) dtkHandle, TclDtkFree);
  Tcl_CreateObjCommand(interp,"stop", Stop, (ClientData) dtkHandle, TclDtkFree);
  Tcl_CreateObjCommand(interp, "pause", Pause,
					   (ClientData) dtkHandle, TclDtkFree);
  Tcl_CreateObjCommand(interp, "resume", Resume,
					   (ClientData) dtkHandle, TclDtkFree);
  debugMsg("Tcldtk_Init", 10, "Completed successfully");
  return TCL_OK;
}

/* > */
/* <say*/

int Say(ClientData dtkHandle, Tcl_Interp *interp, int objc,
		Tcl_Obj *CONST objv[]) {
#define DESTLEN 10000
  int i, length;

  MMRESULT status;
  DWORD dwFlags = TTS_NORMAL;
  char *txt = NULL;

  debugMsg("Say", 10, "Entering function");
  for (i=1; i<objc; i++) {
	sprintf(error_buff, "For loop - %d. objc = %d", i, objc);
	debugMsg("Say", 6, error_buff);
    txt = Tcl_GetStringFromObj(objv[i], &length);
	sprintf(error_buff, "String length is %d", length);
	debugMsg("Say loop", 6, error_buff);
	sprintf(error_buff, "Tcl obj %d. String = %s\n", i, txt);
	debugMsg("Say", 6, error_buff);
    if (Tcl_StringMatch(txt, "-reset")) {
      status = TextToSpeechReset(dtkHandle, FALSE);
	  if (status != MMSYSERR_NOERROR) {
		error_msg = getErrorMsg(status);
		debugMsg("Say reset", 1, error_msg);
		Tcl_SetObjResult(interp, Tcl_NewStringObj(error_msg, -1));
        return TCL_ERROR;
      }
    }else {
      char *src = Tcl_GetStringFromObj(objv[i], NULL);
      int srcLen=-1;
      char *dst=NULL;
      int srcReadPtr=0;
      int dstWrotePtr=0;
      int dstCharsPtr=0;
      Tcl_Encoding encoding=Tcl_GetEncoding(interp, "iso8859-1");

      dst=malloc(DESTLEN);

      Tcl_UtfToExternal(interp, encoding, 
                        src, srcLen, 
                        0, NULL, 
                        dst, DESTLEN,
                        &srcReadPtr, &dstWrotePtr, &dstCharsPtr);

      for (i=0;i<strlen(dst);i++)
      {
        dst[i]=Char2Char[(unsigned char)dst[i]];
	if (dst[i]==11)
	  {
	    dst[i]=0;
	  }
      }

      status = TextToSpeechSpeak(dtkHandle, dst, dwFlags);
      if (status != MMSYSERR_NOERROR) {
	free(dst);
	error_msg = getErrorMsg(status);
	debugMsg("Say speak", 1, error_msg);
	Tcl_SetObjResult(interp, Tcl_NewStringObj(error_msg, -1));
        return TCL_ERROR;
      }
      free(dst);
    }
  }
  if (Tcl_StringMatch(Tcl_GetStringFromObj(objv[0],NULL), "synth")) {
	debugMsg("Say synth", 6, "Forcing text with TTSSpeak");
    status = TextToSpeechSpeak(dtkHandle, "", TTS_FORCE);
    if (status != MMSYSERR_NOERROR) {
	  error_msg = getErrorMsg(status);
	  debugMsg("Say synth", 1, error_msg);
	  Tcl_SetObjResult(interp, Tcl_NewStringObj(error_msg, -1));
	  return TCL_ERROR;
	}
  }
  debugMsg("Say", 10, "Successful return from function");
  return TCL_OK;
}

/* > */
/* < sync*/

int Synchronize(ClientData dtkHandle, Tcl_Interp *interp,
                int objc, Tcl_Obj *CONST objv[]) {
  MMRESULT status;

  debugMsg("Synchronize", 10, "Entering function");
  status = TextToSpeechSync(dtkHandle);
  if (status != MMSYSERR_NOERROR) {
	error_msg = getErrorMsg(status);
	debugMsg("Synchronize", 1, error_msg);
	Tcl_SetObjResult(interp, Tcl_NewStringObj(error_msg, -1));
	return TCL_ERROR;
  }
  debugMsg("Synchronize", 10, "Successful return from function");
  return TCL_OK;
}

/* > */
/* <stop*/

int Stop(ClientData dtkHandle, Tcl_Interp *interp,
		 int objc, Tcl_Obj *CONST objv[]) {
  MMRESULT status;

  debugMsg("Stop", 10, "Entering function");
  status = TextToSpeechReset (dtkHandle, FALSE);
  if (status != MMSYSERR_NOERROR) {
	error_msg = getErrorMsg(status);
	debugMsg("Stop reset", 1, error_msg);
	Tcl_SetObjResult(interp, Tcl_NewStringObj(error_msg, -1));
	return TCL_ERROR;
  }
  status = TextToSpeechSpeak(dtkHandle,
                    "[:phoneme arpabet speak on :say clause]", TTS_NORMAL);
  if (status != MMSYSERR_NOERROR) {
	error_msg = getErrorMsg(status);
	debugMsg("Stop init", 1, error_msg);
	Tcl_SetObjResult(interp, Tcl_NewStringObj(error_msg, -1));
	return TCL_ERROR;
  }
  debugMsg("Stop", 10, "Successful return from function");
  return TCL_OK;
}

/* > */
/* < pause and resume */

int Pause(ClientData dtkHandle, Tcl_Interp *interp,
		  int objc, Tcl_Obj *CONST objv[]) {
  static int nb=0;
  MMRESULT status;
  if (!nb)
  {
    nb=1;
    return TCL_OK;
  }

  debugMsg("Pause", 10, "Entering function");
  status = TextToSpeechPause(dtkHandle);
  if (status != MMSYSERR_NOERROR) {
	error_msg = getErrorMsg(status);
	debugMsg("Pause", 1, error_msg);
	Tcl_SetObjResult(interp, Tcl_NewStringObj(error_msg, -1));
	return TCL_ERROR;
  }
  debugMsg("Pause", 10, "Successful return from function");
  return TCL_OK;
}

int Resume(ClientData dtkHandle, Tcl_Interp *interp,
		   int objc, Tcl_Obj *CONST objv[]) {
  MMRESULT status;

  debugMsg("resume", 10, "Entering function");
  status = TextToSpeechResume(dtkHandle);
  if (status != MMSYSERR_NOERROR) {
	error_msg = getErrorMsg(status);
	debugMsg("Resume", 1, error_msg);
	Tcl_SetObjResult(interp, Tcl_NewStringObj(error_msg, -1));
	return TCL_ERROR;
  }
  debugMsg("Resume", 10, "Successful return from function");
  return TCL_OK;
}

/* > */
/* <end of file*/
/* local variables: */
/* folded-file: t */
/* end: */
/* > */
