/*Id: tcldtk.c,v 17.0 2002/11/23 01:29:10 raman Exp */
/* <copyright*/
/**
 *Copyright (C) 1995 -- 2002, T. V. Raman 
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
// ###
/* > */
/* <headers*/

#include <tcl.h>
#include <dtk/ttsapi.h>
#define PACKAGENAME "tts"
#define PACKAGEVERSION "1.0"

/* > */
/* <prototypes*/

//#define DTKDEBUG 1
#ifdef DTKDEBUG

#include <sys/time.h>
#include <time.h>

// GC
void InitHelpEmul()
{
  unlink("/tmp/dtk.log");
}

char* Print_Time()
{
  struct timeval tv;
  struct timezone tz;
  static char aBuffer [200];
  gettimeofday(&tv, &tz);
  
  sprintf(aBuffer,"%d'%d",(int)(tv.tv_sec % 100), (int)(tv.tv_usec));
  return aBuffer;
}

void HelpEmul(char* theText)
{
  FILE* fd=fopen ("/tmp/dtk.log","a");
  fprintf(fd,"[%s] %s\n", Print_Time(), theText);
  fclose(fd);
}

void HelpEmul2(char* theText1, char* theText2)
{
  FILE* fd=fopen ("/tmp/dtk.log","a");
  fprintf(fd,"[%s] %s %s\n", Print_Time(), theText1, theText2);
  fclose(fd);
}

#define INIT_HELPEMUL InitHelpEmul()
#define PRINT(a) HelpEmul(a)
#define ENTER(a,b) HelpEmul2(a,b)

#else

#define INIT_HELPEMUL
#define PRINT(a)
#define ENTER(a,b)

#endif

#define WITHDTK 1

#define EXPORT
extern  EXPORT int Tcldtk_Init(Tcl_Interp *interp);

int Say(ClientData, Tcl_Interp *, int, Tcl_Obj * CONST []);
int Stop(ClientData, Tcl_Interp *, int, Tcl_Obj * CONST []);
int SetLanguage(ClientData, Tcl_Interp *, int, Tcl_Obj * CONST []);

int Synchronize(ClientData, Tcl_Interp *, int, Tcl_Obj * CONST []);
int Pause(ClientData, Tcl_Interp *, int, Tcl_Obj * CONST []);
int Resume(ClientData, Tcl_Interp *, int, Tcl_Obj * CONST []);

#define CHAR2CHARSIZE 256
static unsigned char Char2Char[CHAR2CHARSIZE];
/* > */

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
	    } //switch
	} //else
    } //for
}

/* <closing down*/

void TclDtkFree(ClientData dtkHandle) {
  ENTER("tcldtk","TclDtkFree");
#ifndef WITHDTK
  return;
#endif
  PRINT("TextToSpeechShutdown");

  if ( TextToSpeechShutdown( dtkHandle ) != MMSYSERR_NOERROR) 
    fprintf(stderr,"TextToSpeechShutdown failed.\n");
}

/* > */
/* <init*/

int Tcldtk_Init(Tcl_Interp *interp) {

  INIT_HELPEMUL;

  ENTER("tcldtk","Init");
  int status;
  LPTTS_HANDLE_T dtkHandle;
  unsigned int devNo = WAVE_MAPPER;
  //  DWORD devOptions = OWN_AUDIO_DEVICE;
  DWORD devOptions = 0;

  if (Tcl_PkgProvide(interp, PACKAGENAME, PACKAGEVERSION) != TCL_OK) {
    Tcl_AppendResult(interp, "Error loading ", PACKAGENAME, NULL);
    return TCL_ERROR;
  }

  dtkHandle=NULL;
#ifdef WITHDTK
  unsigned int UShandle;
  MMRESULT result;
  LPTTS_HANDLE_T phTTS;

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

  PRINT("TextToSpeechEnumLangs");

  if (TextToSpeechEnumLangs(&languageINFO)==0)
  {
    printf("Error : TextToSpeechEnumLangs\n");
    exit(1);
  }
  int aKnownLanguage=0;
  if (languageINFO->MultiLang)
  {
    int i;
    for(i=0;i<languageINFO->Languages;i++)
    {
      fprintf(stdout, "LANGUAGE=%s, TTS=%s\n",aLanguage,languageINFO->Entries[i].lang_code);

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

  PRINT("TextToSpeechStartLang");

  UShandle=TextToSpeechStartLang(aLanguage);
  if (UShandle & TTS_LANG_ERROR) {
    exit(1);
  }

  PRINT("TextToSpeechSelectLang");

  if (TextToSpeechSelectLang(NULL, UShandle)==FALSE)
  {
//    printf("Error selecting language\n");
    PRINT("TextToSpeechCloseLang");

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

  fprintf(stderr, "Calling tts startup.\n");
  PRINT("TextToSpeechStartup");
  status = TextToSpeechStartup( &dtkHandle, devNo,
                                devOptions, NULL, 0 );
  fprintf(stderr, "tts startup returned %d\n", status);
  switch (status) {
  case MMSYSERR_NODRIVER:
    Tcl_AppendResult(interp, "TTS: Could not find any wave devices\n", NULL);
    return TCL_ERROR;
    break;
  case MMSYSERR_NOTENABLED:
    Tcl_AppendResult(interp,"TTS: DECtalk license not found.\n", NULL);
    return TCL_ERROR;
    break;
  case MMSYSERR_ALLOCATED:
    Tcl_AppendResult(interp,"TTS: DECtalk has exceeded license quota.\n", NULL);
    return TCL_ERROR;
    break;
  case MMSYSERR_NOERROR:
    break;

  default:
    Tcl_AppendResult(interp,"\n%s: TextToSpeechStartup failed, \n", NULL);
    return TCL_ERROR;
  }
  if (dtkHandle == NULL) { 
    Tcl_AppendResult(interp, "Could not open text-to-speech engine", NULL);
    return TCL_ERROR;
  }
#endif

  Tcl_CreateObjCommand(interp, "say", Say, (ClientData) dtkHandle, TclDtkFree);
  Tcl_CreateObjCommand(interp, "synth", Say, (ClientData)
                       dtkHandle, NULL);
  Tcl_CreateObjCommand(interp, "synchronize", Synchronize, (ClientData) dtkHandle, TclDtkFree);
  Tcl_CreateObjCommand(interp, "stop", Stop, (ClientData)
                       dtkHandle, TclDtkFree);
  
  Tcl_CreateObjCommand(interp, "pause", Pause, (ClientData)
                       dtkHandle, TclDtkFree);
  Tcl_CreateObjCommand(interp, "resume", Resume, (ClientData) dtkHandle, TclDtkFree);
  Tcl_CreateObjCommand(interp, "setLanguage", SetLanguage, (ClientData)
                       dtkHandle, TclDtkFree);
  return TCL_OK;
}

/* > */
/* <say*/
int Say(ClientData dtkHandle, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]) {
  ENTER("tcldtk","Say");
  int srcLen=-1;
#define DESTLEN 10000
  int i, rc, length;
  unsigned char dst[DESTLEN];
  int srcReadPtr=0;
  int dstWrotePtr=0;
  int dstCharsPtr=0;
#ifndef WITHDTK
  for (i=1; i<objc; i++) {
    char *txt = Tcl_GetStringFromObj(objv[i], &length);
    if (Tcl_StringMatch(txt, "-reset")) {
      PRINT("-reset");
    }else {
      char *src = Tcl_GetStringFromObj(objv[i], NULL);
      
      PRINT(src);
      int srcLen=-1;
      Tcl_Encoding encoding=Tcl_GetEncoding(interp, "iso8859-1");
      Tcl_UtfToExternal(interp, encoding, 
                        src, srcLen, 
                        0, NULL, 
                        dst, DESTLEN,
                        &srcReadPtr, &dstWrotePtr, &dstCharsPtr);
      
      PRINT(dst);
    }
  }
  return  TCL_OK;
#endif
  
  for (i=1; i<objc; i++) {
    char *txt = Tcl_GetStringFromObj(objv[i], &length);
    if (Tcl_StringMatch(txt, "-reset")) {
      PRINT("TextToSpeechReset");
      TextToSpeechReset (dtkHandle,0);
    }else {
      char *src = Tcl_GetStringFromObj(objv[i], NULL);
      int srcLen=-1;
      unsigned char dst[DESTLEN];
      int srcReadPtr=0;
      int dstWrotePtr=0;
      int dstCharsPtr=0;
      Tcl_Encoding encoding=Tcl_GetEncoding(interp, "iso8859-1");
      Tcl_UtfToExternal(interp, encoding, 
                        src, srcLen, 
                        0, NULL, 
                        dst, DESTLEN,
                        &srcReadPtr, &dstWrotePtr, &dstCharsPtr);

      PRINT(dst);
      for (i=0;i<strlen(dst);i++)
      {
        dst[i]=Char2Char[(unsigned char)dst[i]];
//GC
	if (dst[i]==11)
	  {
	    dst[i]=0;
	  }

      }

// GC
/*       { */
/* 	FILE* fd=fopen ("/tmp/dtk.log","a"); */
/* 	fprintf(fd,"txt: %s\n", dst); */
/* 	fclose(fd); */
/*       } */


      PRINT("TextToSpeechSpeak, TTS_NORMAL");

      if (TextToSpeechSpeak(dtkHandle,
                            dst,
                            TTS_NORMAL) != MMSYSERR_NOERROR ){

        Tcl_SetResult(interp, "Internal tts error", TCL_STATIC);
        return TCL_ERROR;
      }
    }
  }

  if (Tcl_StringMatch(Tcl_GetStringFromObj(objv[0],NULL), "synth")) {
    PRINT("TextToSpeechSpeak, empty, FORCE");
    rc = TextToSpeechSpeak(dtkHandle,
                           "",
                           TTS_FORCE);
    if (rc != MMSYSERR_NOERROR) {
      Tcl_SetResult(interp, "Internal tts synth error", TCL_STATIC);
      return TCL_ERROR;
    }
  }
  return TCL_OK;
}

/* > */
/* < sync*/

int Synchronize(ClientData dtkHandle, Tcl_Interp *interp,
                int objc, Tcl_Obj *CONST objv[]) {
  ENTER("tcldtk","Sync");
#ifndef WITHDTK
  return  TCL_OK;
#endif

  PRINT("TextToSpeechSync");

  int rc = TextToSpeechSync(dtkHandle);
  if (!rc) {
    Tcl_SetResult(interp, "Internal tts synth error", TCL_STATIC);
    return TCL_ERROR;
  }
  return TCL_OK;
}

/* > */
/* <stop*/

int Stop(ClientData dtkHandle, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]) {
  ENTER("tcldtk","Stop");
#ifndef WITHDTK
  return  TCL_OK;
#endif

  PRINT("TextToSpeechReset");

  TextToSpeechReset (dtkHandle, FALSE);
  // French TTS : TTS_NORMAL instead of TTS_FORCE

  PRINT("TextToSpeechSpeak, [:phoneme arpabet speak on :say clause], NORMAL");

  TextToSpeechSpeak(dtkHandle,
                    "[:phoneme arpabet speak on :say clause]", 
		    TTS_NORMAL);

  return TCL_OK;
}

/* > */
/* < pause and resume */

int Pause(ClientData dtkHandle, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]) {
  ENTER("tcldtk","Pause");
#ifndef WITHDTK
  return  TCL_OK;
#endif


  static int nb=0;
  if (!nb)
  {
    nb=1;
    return TCL_OK;
  }
  
  PRINT("TextToSpeechPause");

  if (TextToSpeechPause(dtkHandle)) return TCL_OK;
  Tcl_SetResult(interp, "Could not pause synthesis", TCL_STATIC);
  return TCL_ERROR;
}

int Resume(ClientData dtkHandle, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]) {
  ENTER("tcldtk","Resume");
#ifndef WITHDTK
  return  TCL_OK;
#endif

  PRINT("TextToSpeechResume");

  if (TextToSpeechResume(dtkHandle)) return TCL_OK;
  Tcl_SetResult(interp, "Could not resume synthesis", TCL_STATIC);
  return TCL_ERROR;
}

/* > */

int Dtk_Init(ClientData dtkHandle, char* lang) {
  int status;
  unsigned int  devNo = 0;
  DWORD devOptions = 0;
  devOptions |= WAVE_OPEN_SHAREABLE;
  int handle=0;
  
  LPLANG_ENUM languageINFO;

  PRINT("TextToSpeechEnumLangs");

  if (TextToSpeechEnumLangs(&languageINFO)==0)
  {
    printf("Error : TextToSpeechEnumLangs\n");
    exit(1);
  }

  PRINT("TextToSpeechStartLang");

  handle=TextToSpeechStartLang(lang);

  PRINT("TextToSpeechSelectLang");

  if (TextToSpeechSelectLang(NULL, handle)==FALSE)
  {
    printf("Error selecting us\n");
    //    TextToSpeechCloseLang(the_lang);
    exit(1);    
  }

  PRINT("TextToSpeechStartup");

  status = TextToSpeechStartup( &dtkHandle, devNo,
                                devOptions, NULL, 0 );
  if (dtkHandle == NULL) { 
    printf("Could not open text-to-speech engine");
    exit(1);
  }
}

int SetLanguage(ClientData dtkHandle, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]) {
  ENTER("tcldtk","SetLanguage");
  int length;
  char *txt = Tcl_GetStringFromObj(objv[1], &length);

  static char aFilename[1000];
  //  sprintf(aFilename, "%s/%s", getenv("HOME"), FILE_LANG);
  //  FILE* fp=fopen (aFilename, "w");
  //  fprintf(fp,"%s", txt);
  //  fclose(fp);
//  system("sync");
  return TCL_OK;
}


/* <end of file*/
/* local variables: */
/* folded-file: t */
/* end: */
/* > */
