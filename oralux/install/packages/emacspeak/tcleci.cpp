/* Warning: this code is not the original one. Modified for Oralux */
/*Id: tcleci.cpp,v 21.0 2004/11/25 18:46:03 raman Exp */
/* Tcl ViaVoiceOutloud Interface program
   (c) Copyright 1999 by Paige Phault

   The author hereby grants permission to use, copy, modify, distribute, and 
   license this software for any purpose, provided that existing copyright notices
   are retained in all copies and that this notice is included verbatim in any 
   distributions. No written agreement, license, or royalty fee is required for 
   any of the authorized uses.  Modifications to this software may be copyrighted 
   by their authors and need not follow the licensing terms described here, 
   provided that the new terms are clearly indicated on the first page of each 
   file where they apply.

   IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY FOR 
   DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT 
   OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY DERIVATIVES THEREOF, 
   EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

   THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES, INCLUDING, 
   BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
   PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE IS PROVIDED ON AN 
   "AS IS" BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE NO OBLIGATION TO PROVIDE 
   MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

   dynamic loading of eci library contributed by Jeffrey Sorensen
   --this allows a compiled version  of this 
   speech server to be distributed without violating the IBM
   Viavoice license.
   This means that end-users only need install the Viavoice RTK
   (Runtime toolkit)
   to use Emacspeak with the ViaVoice TTS engine.
*/
/* TCL usage
	package require tts

	proc index x {
		puts "I just played index $x"
	}

	synth "Hello world"
	synth -index 0 "This is some" -index 1 "really wierd"
	say -index 2 "text"
	say -reset

The only difference bewtween say and synth is that synth calls
eciSynthesize and say doesn't.  You can put as many text blocks as
you like after a command.
*/
/*
Changelog:
25 March 2005: language switching (Gilles Casse gcasse@oralux.org)
*/

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

#include <linux/soundcard.h>
#include <sys/ioctl.h>
#include <tcl.h>
#include <dlfcn.h>

#define PACKAGENAME "tts"
#define PACKAGEVERSION "1.0"

#ifdef WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

//#define DEBUG 1
#undef DEBUG
#ifdef DEBUG
void ENTER( char* theString) 
{
  struct stat buf;
  stat("ENTER function", &buf);
  stat(theString, &buf);

  FILE* fd=fopen("/tmp/vv.log","a");
  fprintf(fd,"ENTER %s\n",theString);
  fclose(fd);
}
void SHOW( char* theString) 
{
  struct stat buf;
  stat("SHOW param", &buf);
  stat(theString, &buf);

  FILE* fd=fopen("/tmp/vv.log","a");
  fprintf(fd,"%s\n",theString);
  fclose(fd);
}
void SHOW2( char* a, char* b) 
{
  FILE* fd=fopen("/tmp/vv.log","a");
  fprintf(fd, a, b);
  fclose(fd);
}
void SHOW2I( char* a,int b) 
{
  FILE* fd=fopen("/tmp/vv.log","a");
  fprintf(fd, a, b);
  fclose(fd);
}
#else
void ENTER( char* theString){}
void SHOW( char* theString){}
void SHOW2( char* a, char*b){} 
void SHOW2I( char* a, int b){} 
#endif

#define ECILIBRARYNAME "libibmeci.so"

#define DSP "/dev/dsp"
int dsp = -1;
//.04  second using 11025k samples.
//note that in the tcl server we select for 0.02 seconds so
//that we dont queue up too many speech samples,
//This is important for stopping speech immediately.
#define BUFSIZE 512
short waveBuffer[BUFSIZE];

/* The following declarations are derived from the publically available
   documentation for ViaVoice TTS outloud.  
--they are placed here to obviate the need for having the
ViaVoice SDK installed.
*/

typedef enum
{
  eciDataNotProcessed, eciDataProcessed
}
ECICallbackReturn;

typedef enum
{
  eciWaveformBuffer, eciPhonemeBuffer, eciIndexReply, eciPhonemeIndexReply
}
ECIMessage;

typedef enum
{
  eciSynthMode,
  eciInputType,
  eciTextMode,
  eciDictionary,
  eciSampleRate = 5,
  eciWantPhonemeIndices = 7,
  eciRealWorldUnits,
  eciLanguageDialect,
  eciNumberMode,
  eciPhrasePrediction,
  eciNumParams
}
ECIParam;
static void (*_eciVersion) (char *);
static void *(*_eciNew) ();
static void *(*_eciNewEx) (int);
static void (*_eciDelete) (void *);
static int (*_eciReset) (void *);
static int (*_eciStop) (void *);
static int (*_eciClearInput) (void *);
static int (*_eciPause) (void *, int);
static int (*_eciSynthesize) (void *);
static int (*_eciSynchronize) (void *);
static int (*_eciSpeaking) (void *);
static int (*_eciAddText) (void *, char *);
static int (*_eciInsertIndex) (void *, int);
static int (*_eciSetParam) (void *, int, int);
static int (*_eciGetVoiceParam) (void *, int, int);
static int (*_eciSetVoiceParam) (void *, int, int, int);
static int (*_eciSetOutputBuffer) (void *, int, short *);
static int (*_eciSetOutputDevice) (void *, int);
static void (*_eciRegisterCallback) (void *,
				     int (*)(void *, int, long, void *),
				     void *);
static int openDSP ();
extern "C" EXPORT int Tcleci_Init (Tcl_Interp * interp);
int SetRate (ClientData, Tcl_Interp *, int, Tcl_Obj * CONST[]);
int GetRate (ClientData, Tcl_Interp *, int, Tcl_Obj * CONST[]);
int getTTSVersion (ClientData, Tcl_Interp *, int, Tcl_Obj * CONST[]);
int Say (ClientData, Tcl_Interp *, int, Tcl_Obj * CONST[]);
int Set_Language (ClientData, Tcl_Interp *, int, Tcl_Obj * CONST[]);
int Stop (ClientData, Tcl_Interp *, int, Tcl_Obj * CONST[]);
int SpeakingP (ClientData, Tcl_Interp *, int, Tcl_Obj * CONST[]);
int Synchronize (ClientData, Tcl_Interp *, int, Tcl_Obj * CONST[]);
int Pause (ClientData, Tcl_Interp *, int, Tcl_Obj * CONST[]);
int Resume (ClientData, Tcl_Interp *, int, Tcl_Obj * CONST[]);
int setOutput (ClientData, Tcl_Interp *, int, Tcl_Obj * CONST[]);
int closeDSP (ClientData, Tcl_Interp *, int, Tcl_Obj * CONST[]);
int eciCallback (void *, int, long, void *);
int playWaveFile (ClientData, Tcl_Interp *, int, Tcl_Obj * CONST[]);


enum ECILanguageDialect {
    NODEFINEDCODESET                = 0x00000000,
    eciGeneralAmericanEnglish       = 0x00010000,
    eciBritishEnglish               = 0x00010001,
    eciCastilianSpanish             = 0x00020000,
    eciMexicanSpanish               = 0x00020001,
    eciStandardFrench               = 0x00030000,
    eciCanadianFrench               = 0x00030001,
    eciStandardGerman               = 0x00040000,
    eciStandardItalian              = 0x00050000,
    eciMandarinChinese              = 0x00060000,
    eciMandarinChineseGB            = eciMandarinChinese,
    eciMandarinChinesePinYin        = 0x00060100,
    eciMandarinChineseUCS           = 0x00060800,
    eciTaiwaneseMandarin            = 0x00060001,
    eciTaiwaneseMandarinBig5        = eciTaiwaneseMandarin,
    eciTaiwaneseMandarinZhuYin      = 0x00060101,
    eciTaiwaneseMandarinPinYin      = 0x00060201,
    eciTaiwaneseMandarinUCS         = 0x00060801,
    eciBrazilianPortuguese          = 0x00070000,
    eciStandardJapanese             = 0x00080000,
    eciStandardJapaneseSJIS         = eciStandardJapanese,
    eciStandardJapaneseUCS          = 0x00080800,
    eciStandardFinnish              = 0x00090000,
    eciStandardKorean               = 0x000A0000,
    eciStandardKoreanUHC            = eciStandardKorean,
    eciStandardKoreanUCS            = 0x000A0800,
    eciStandardCantonese            = 0x000B0000,
    eciStandardCantoneseGB          = eciStandardCantonese,
    eciStandardCantoneseUCS         = 0x000B0800,
    eciHongKongCantonese            = 0x000B0001,
    eciHongKongCantoneseBig5        = eciHongKongCantonese,
    eciHongKongCantoneseUCS         = 0x000B0801,
    eciStandardDutch                = 0x000C0000,
    eciStandardNorwegian            = 0x000D0000,
    eciStandardSwedish              = 0x000E0000,
    eciStandardDanish               = 0x000F0000,
    eciStandardReserved             = 0x00100000,
    eciStandardThai                 = 0x00110000,
    eciStandardThaiTIS              = eciStandardThai
};

struct langInfo {
  char* lib;
  char* code;
  char* encoding;
  char* id;
  char* label;
  enum ECILanguageDialect codeset;
};



struct langInfo TheLanguages[]={
  {"chs","zh_CN","gb2312","6.0","Chinese Simplified",eciMandarinChinese}, // gb18030 not in tcl ?
  {"cht","zh_HK","big5","6.1","Chinese Traditional",eciTaiwaneseMandarin},
  {"deu","de_DE","iso8859-1","4.0","Deutsch",eciStandardGerman},
  {"fin","fi_FI","iso8859-1","9.0","Finnish",eciStandardFinnish},
  {"ita","it_IT","iso8859-1","5.0","Italiano",eciStandardItalian},
  {"jpn","ja_JP","shiftjis","8.0","Japanese",eciStandardJapanese},
  {"eng","en_GB","iso8859-1","1.1","British",eciBritishEnglish},
  {"enu","en_US","iso8859-1","1.0","American",eciGeneralAmericanEnglish},
  {"esm","es_MX","iso8859-1","2.1","Mexicano",eciMexicanSpanish},
  {"esp","es_ES","iso8859-1","2.0","Español",eciCastilianSpanish},
  {"fra","fr_FR","iso8859-1","3.0","Français",eciStandardFrench},
  {"frc","fr_CA","iso8859-1","3.1","Français Canadien",eciCanadianFrench},
  {"ptb","pt_BR","iso8859-1","7.0","Brazilian Portuguese",eciBrazilianPortuguese},
};

#define langInfoMax sizeof(TheLanguages)/sizeof(TheLanguages[0])

#define MAXLINE 256
static char buffer1[MAXLINE];
static char buffer2[MAXLINE];

static int searchLangLib( char* theLib, int* theIndex)
{
  int aStatus=0;
  char aLib[4];
  strncpy( aLib, theLib, 3);
  aLib[3]=0;
  
  for (*theIndex=0; *theIndex<langInfoMax; (*theIndex)++)
    {
      if (strcmp(TheLanguages[*theIndex].lib, aLib)==0)
	{
	  aStatus=1;
	  break;
	}
    }
  return aStatus;
}

static int searchLangCode( char* theCode, int* theIndex)
{
  ENTER("searchLangCode");
  SHOW2("theCode=%s=\n",theCode);
  int aStatus=0;
  
  for (*theIndex=0; *theIndex<langInfoMax; (*theIndex)++)
    {
      if (strcmp(TheLanguages[*theIndex].code, theCode)==0)
	{
	  aStatus=1;
	  SHOW2I("*theIndex=%d=\n",*theIndex);
	  break;
	}
    }
  return aStatus;
}

static int filtlib (const struct dirent *theFile)                
{                                          
  return ((theFile->d_name)[3]=='5');
}                                          


void
TclEciFree (ClientData eciHandle)
{
  _eciDelete (eciHandle);
  close (dsp);
  dsp = -1;
}

int
Tcleci_Init (Tcl_Interp * interp)
{
  int rc, tmp;
  void *eciHandle;
  void *eciLib;
  int aNewEci=1;
  //< configure shared library symbols

  ENTER("Tcleci_Init");
  eciLib = dlopen (ECILIBRARYNAME, RTLD_LAZY);
  if (eciLib == NULL)
    {
      Tcl_AppendResult (interp, "Could not load ",
			ECILIBRARYNAME,
			"\nPlease install the IBM ViaVoice Outloud RTK",
			NULL);
      return TCL_ERROR;
    }

  _eciVersion = (void (*)(char *)) dlsym (eciLib, "eciVersion");
  _eciNew = (void *(*)()) dlsym (eciLib, "eciNew");
  _eciNewEx = (void *(*)(int)) dlsym (eciLib, "eciNewEx");
  _eciDelete = (void (*)(void *)) dlsym (eciLib, "eciDelete");
  _eciReset = (int (*)(void *)) dlsym (eciLib, "eciReset");
  _eciStop = (int (*)(void *)) dlsym (eciLib, "eciStop");
  _eciClearInput = (int (*)(void *)) dlsym (eciLib, "eciClearInput");
  _eciPause = (int (*)(void *, int)) dlsym (eciLib, "eciPause");
  _eciSynthesize = (int (*)(void *)) dlsym (eciLib, "eciSynthesize");
  _eciSynchronize = (int (*)(void *)) dlsym (eciLib, "eciSynchronize");
  _eciSpeaking = (int (*)(void *)) dlsym (eciLib, "eciSpeaking");
  _eciInsertIndex = (int (*)(void *, int)) dlsym (eciLib, "eciInsertIndex");
  _eciAddText = (int (*)(void *, char *)) dlsym (eciLib, "eciAddText");
  _eciSetParam = (int (*)(void *, int, int)) dlsym (eciLib, "eciSetParam");
  _eciGetVoiceParam = (int (*)(void *, int, int))
    dlsym (eciLib, "eciGetVoiceParam");
  _eciSetVoiceParam = (int (*)(void *, int, int, int))
    dlsym (eciLib, "eciSetVoiceParam");
  _eciRegisterCallback = (void
			  (*)(void *, int (*)(void *, int, long, void *),
			      void *)) dlsym (eciLib, "eciRegisterCallback");
  _eciSetOutputBuffer =
    (int (*)(void *, int, short *)) dlsym (eciLib, "eciSetOutputBuffer");
  _eciSetOutputDevice =
    (int (*)(void *, int)) dlsym (eciLib, "eciSetOutputDevice");

  //>
  //< check for needed symbols 

  int okay = 1;
  if (!_eciNew)
    {
      okay = 0;
      Tcl_AppendResult (interp, "eciNew undef\n", NULL);
    }
  if (!_eciNewEx)
    {
      okay = 0;
      Tcl_AppendResult (interp, "eciNewEx undef\n", NULL);
    }
  if (!_eciDelete)
    {
      okay = 0;
      Tcl_AppendResult (interp, "eciDelete undef\n", NULL);
    }
  if (!_eciReset)
    {
      okay = 0;
      Tcl_AppendResult (interp, "eciReset undef\n", NULL);
    }
  if (!_eciStop)
    {
      okay = 0;
      Tcl_AppendResult (interp, "eciStop undef\n", NULL);
    }
  if (!_eciClearInput)
    {
      okay = 0;
      Tcl_AppendResult (interp, "eciClearInput undef\n", NULL);
    }
  if (!_eciPause)
    {
      okay = 0;
      Tcl_AppendResult (interp, "eciPause undef\n", NULL);
    }
  if (!_eciSynthesize)
    {
      okay = 0;
      Tcl_AppendResult (interp, "eciSynthesize undef\n", NULL);
    }
  if (!_eciSpeaking)
    {
      okay = 0;
      Tcl_AppendResult (interp, "eciSpeaking undef\n", NULL);
    }
  if (!_eciInsertIndex)
    {
      okay = 0;
      Tcl_AppendResult (interp, "eciInsertIndex undef\n", NULL);
    }
  if (!_eciAddText)
    {
      okay = 0;
      Tcl_AppendResult (interp, "eciAddText undef\n", NULL);
    }
  if (!_eciSetParam)
    {
      okay = 0;
      Tcl_AppendResult (interp, "eciSetParam undef\n", NULL);
    }
  if (!_eciSetParam)
    {
      okay = 0;
      Tcl_AppendResult (interp, "eciSetParam undef\n", NULL);
    }
  if (!_eciGetVoiceParam)
    {
      okay = 0;
      Tcl_AppendResult (interp, "eciGetVoiceParam undef\n", NULL);
    }
  if (!_eciSetVoiceParam)
    {
      okay = 0;
      Tcl_AppendResult (interp, "eciSetVoiceParam undef\n", NULL);
    }
  if (!_eciRegisterCallback)
    {
      okay = 0;
      Tcl_AppendResult (interp, "eciRegisterCallback undef\n", NULL);
    }
  if (!_eciSetOutputBuffer)
    {
      okay = 0;
      Tcl_AppendResult (interp,
                        "eciSetOutputBuffer undef\n", NULL);
    }
  if (!_eciSetOutputDevice)
    {
      okay = 0;
      Tcl_AppendResult (interp, "eciSetOutputDevice undef\n", NULL);
    }
  if (!okay)
    {
      Tcl_AppendResult (interp, "Missing symbols from ", ECILIBRARYNAME,
			NULL);
      return TCL_ERROR;
    }

  //>

  if (Tcl_PkgProvide (interp, PACKAGENAME, PACKAGEVERSION) != TCL_OK)
    {
      Tcl_AppendResult (interp, "Error loading ", PACKAGENAME, NULL);
      return TCL_ERROR;
    }

  SHOW("language switching");
  {
    // List the available languages
    struct dirent **dir=NULL; 
    int aIndex; 
    int i;                             
    int n=scandir ("/usr/lib/ibmeci", &dir, filtlib, NULL);
    char* aDefaultLang=(char*)getenv("LANGUAGE");
    if (aDefaultLang==NULL)
      {
	aDefaultLang="en";
      }
    SHOW2("default lang=%s=\n",aDefaultLang);

    for (i=0; i<langInfoMax; i++)
      {
	sprintf(buffer1, "%d", i); 
	Tcl_SetVar2(interp, "langalias", TheLanguages[i].code, buffer1, 0);
      }

    if (n >= 0)                              
      {                                      
	int j=0;
	Tcl_SetVar2(interp, "langsynth", "current", "0", 0);
	for (i = 0; i < n; ++i)
	  {  
	    if (searchLangLib( dir[i]->d_name, &aIndex))
	    {
	      SHOW2("Available lang=%s=\n",TheLanguages[ aIndex].code);
	      sprintf(buffer1, "%d", j++); 
	      sprintf(buffer2, "%d", aIndex); 
	      Tcl_SetVar2(interp, "langsynth", buffer1, buffer2, 0);
	
	      if (strncmp(aDefaultLang, dir[i]->d_name, 2)==0)
	      {
		Tcl_SetVar2(interp, "langsynth", "current", buffer1, 0);
	      }

	      Tcl_SetVar2(interp, "langlabel", buffer1, TheLanguages[ aIndex].label, 0);

	      Tcl_SetVar2(interp, "langsynth", "top", buffer1, 0);

	      if (aNewEci)
		{
		  aNewEci=0;
		  eciHandle = _eciNewEx (TheLanguages[ aIndex].codeset);
		  if (eciHandle == 0)
		    {
		      Tcl_AppendResult (interp, "Could not open text-to-speech engine", NULL);
		      return TCL_ERROR;
		    }
		}
	    }
	  }
      }
    
    if (dir)
      {
	free(dir);
      }
  }

  //<initialize TTS 

  if ((_eciSetParam (eciHandle, eciInputType, 1) == -1)
      || (_eciSetParam (eciHandle, eciSynthMode, 1) == -1)
      || (_eciSetParam (eciHandle, eciSampleRate, 1) == -1)
      //|| (_eciSetParam(eciHandle,8/*eciRealWorldUnits*/, 1) == -1)
    )
    {
      Tcl_AppendResult (interp, "Could not initialized tts", NULL);
      _eciDelete (eciHandle);
      return TCL_ERROR;
    }
  _eciRegisterCallback (eciHandle, eciCallback, interp);

  //>
  //<register tcl commands 

  Tcl_CreateObjCommand (interp, "setRate", SetRate,
			(ClientData) eciHandle, TclEciFree);
  Tcl_CreateObjCommand (interp, "getRate", GetRate, (ClientData) eciHandle,
			TclEciFree);
  Tcl_CreateObjCommand (interp, "ttsVersion", getTTSVersion,
			(ClientData) eciHandle, TclEciFree);
  Tcl_CreateObjCommand (interp, "say", Say, (ClientData) eciHandle,
			TclEciFree);
  Tcl_CreateObjCommand (interp, "synth", Say, (ClientData) eciHandle, NULL);
  Tcl_CreateObjCommand (interp, "synchronize", Synchronize,
			(ClientData) eciHandle, TclEciFree);
  Tcl_CreateObjCommand (interp, "stop", Stop, (ClientData) eciHandle,
			TclEciFree);
  Tcl_CreateObjCommand (interp, "speakingP", SpeakingP,
			(ClientData) eciHandle, TclEciFree);
  Tcl_CreateObjCommand (interp, "pause", Pause, (ClientData) eciHandle,
			TclEciFree);
  Tcl_CreateObjCommand (interp, "resume", Resume, (ClientData) eciHandle,
			TclEciFree);
  Tcl_CreateObjCommand (interp, "setOutput", setOutput,
			(ClientData) eciHandle, TclEciFree);
  Tcl_CreateObjCommand (interp, "playWave", playWaveFile, (ClientData) NULL,
			NULL);
  Tcl_CreateObjCommand (interp, "closeDSP", closeDSP, (ClientData) eciHandle,
			TclEciFree);
  Tcl_CreateObjCommand (interp, "set_language", Set_Language, (ClientData) eciHandle, TclEciFree);

  //>
  rc = Tcl_Eval (interp, "proc index x {global tts; \
set tts(last_index) $x}");
  return TCL_OK;
}

/* will play wave files 16 bit stereo and sample rate 11025k */
int
playWaveFile (ClientData unused, Tcl_Interp * interp, int objc,
	      Tcl_Obj * CONST objv[])
{
  size_t count;
  int length;
  char *filename;
  FILE *fp;
  short samples[2 * BUFSIZE];

  ENTER("playWaveFile");


  if (objc != 2)
    {
      Tcl_AppendResult (interp, "Usage: playWave filename", NULL);
      return TCL_ERROR;
    }
  filename = Tcl_GetStringFromObj (objv[1], &length);
  fp = fopen (filename, "r");
  if (fp == NULL)
    {
      Tcl_AppendResult (interp, "Error opening wave file.", NULL);
      return TCL_ERROR;
    }
  while ((count = fread (samples, 2, 2 * BUFSIZE, fp)) > 0)
    {
      write (dsp, samples, count);
    }
  fclose (fp);
  fprintf (stderr, "Played %s\n", filename);
  return TCL_OK;
}

int
playTTS (int samples)
{
  int i;
  short stereo[2 * samples];

  ENTER("playTTS");

  /* mono to stereo */
  for (i = 0; i < samples; i++)
    {
      stereo[2 * i] = waveBuffer[i];
      stereo[2 * i + 1] = waveBuffer[i];
    }
  if (dsp == -1)
    openDSP ();
  write (dsp, stereo, 4 * samples);
  return eciDataProcessed;
}

int
eciCallback (void *eciHandle, int msg, long lparam, void *data)
{
  int rc;
  Tcl_Interp *interp = (Tcl_Interp *) data;

  ENTER("eciCallback");


  if (msg == eciIndexReply /* eciIndexReply */ )
    {
      char buffer[128];
      sprintf (buffer, "index %d", lparam);
      rc = Tcl_Eval (interp, buffer);
      if (rc != TCL_OK)
	Tcl_BackgroundError (interp);
    }
  else if ((msg == eciWaveformBuffer) && (lparam > 0))
    {
      playTTS (lparam);
    }
  return 1;
}

int
GetRate (ClientData eciHandle, Tcl_Interp * interp, int objc,
	 Tcl_Obj * CONST objv[])
{
  int rc, rate, voice;

  ENTER("GetRate");

  if (objc != 2)
    {
      Tcl_AppendResult (interp, "Usage: getRate voiceCode  ", TCL_STATIC);
      return TCL_ERROR;
    }
  rc = Tcl_GetIntFromObj (interp, objv[1], &voice);
  if (rc != TCL_OK)
    return rc;
  rate = _eciGetVoiceParam (eciHandle, voice, 6 /*eciSpeed */ );
  Tcl_SetObjResult (interp, Tcl_NewIntObj (rate));
  return TCL_OK;
}

int
SetRate (ClientData eciHandle, Tcl_Interp * interp, int objc,
	 Tcl_Obj * CONST objv[])
{
  int rc, rate, voice;

  ENTER("SetRate");

  if (objc != 3)
    {
      Tcl_AppendResult (interp, "Usage: setRate voiceCode speechRate ",
			TCL_STATIC);
      return TCL_ERROR;
    }
  rc = Tcl_GetIntFromObj (interp, objv[1], &voice);
  if (rc != TCL_OK)
    return rc;
  rc = Tcl_GetIntFromObj (interp, objv[2], &rate);
  if (rc != TCL_OK)
    return rc;
  rc = _eciSetVoiceParam (eciHandle, voice, 6 /*eciSpeed */ , rate);
  if (rc == -1)
    {
      Tcl_AppendResult (interp, "Could not set rate", TCL_STATIC);
      return TCL_ERROR;
    }
  //fprintf(stderr, "setRate returned %d\n", rc);
  rate = _eciGetVoiceParam (eciHandle, voice, 6 /*eciSpeed */ );
  fprintf (stderr, "eciGetVoiceParam returned %d for voice %d \n",
	   rate, voice);
  return TCL_OK;
}

int getLangIndex( Tcl_Interp * interp, int* theIndex)
{
  ENTER( "getLangIndex");

  int aStatus=0;
  char* aInfo=Tcl_GetVar2(interp, "langsynth", "current", 0);
  if (aInfo)
    {
      *theIndex=atoi(aInfo);
      if ((*theIndex > 0) && (*theIndex < langInfoMax))
	{
	  aStatus=1;
	  SHOW("OK");
	}
    }
  return aStatus;
}

int
Say (ClientData eciHandle, Tcl_Interp * interp, int objc,
     Tcl_Obj * CONST objv[])
{
  int i, rc, index, length;

  ENTER("Say");

  int aIndex=0;
  char* aEncoding="iso8859-1";

  if (getLangIndex(interp, &aIndex))
    {
      aEncoding=TheLanguages[aIndex].encoding;
      SHOW2("aEncoding=%s=\n",aEncoding);
    }

  for (i = 1; i < objc; i++)
    {
      // if string begins with -, assume it is an index value
      char *txt = Tcl_GetStringFromObj (objv[i], &length);
      if (Tcl_StringMatch (txt, "-reset"))
	{
	  _eciReset (eciHandle);
	  if ((_eciSetParam (eciHandle, 1 /*eciInputType */ , 1) == -1)
	      || (_eciSetParam (eciHandle, 0 /*eciSynthMode */ , 1) == -1))
	    {
	      Tcl_AppendResult (interp, "Could not initialized tts", NULL);
	      return TCL_ERROR;
	    }
	}
      else if (Tcl_StringMatch (txt, "-index"))
	{
	  i++;
	  if (i == objc)
	    {
	      Tcl_AppendResult (interp, "missing index parameter",
				TCL_STATIC);
	      return TCL_ERROR;
	    }
	  rc = Tcl_GetIntFromObj (interp, objv[i], &index);
	  if (rc != TCL_OK)
	    return rc;
	  rc = _eciInsertIndex (eciHandle, index);
	  if (!rc)
	    {
	      Tcl_AppendResult (interp, "Could not insert index", TCL_STATIC);
	      return TCL_ERROR;
	    }
	}
      else
	{
	  // assume objv[i] is text to synthesize...

	  // For Oralux: characters are converted from utf-8 to the expected encoding
	  char *src = Tcl_GetStringFromObj(objv[i], NULL);
	  int srcLen=-1;
	  int aLength=strlen(src)+1;
	  char* dst=new char[ aLength ];
	  int srcReadPtr=0;
	  int dstWrotePtr=0;
	  int dstCharsPtr=0;

	  Tcl_Encoding encoding=Tcl_GetEncoding(interp, aEncoding);
	  Tcl_UtfToExternal(interp, encoding, 
			    src, srcLen, 
			    0, NULL, 
			    dst, aLength,
			    &srcReadPtr, &dstWrotePtr, &dstCharsPtr);
	  SHOW2("Say=%s=",dst);

	  rc = _eciAddText(eciHandle, dst);
	  delete dst;
	  if (!rc)
	    {
	      Tcl_SetResult (interp, "Internal tts error", TCL_STATIC);
	      return TCL_ERROR;
	    }
	}
    }
  if (Tcl_StringMatch (Tcl_GetStringFromObj (objv[0], NULL), "synth"))
    {
      rc = _eciSynthesize (eciHandle);
      if (!rc)
	{
	  Tcl_SetResult (interp, "Internal tts synth error", TCL_STATIC);
	  return TCL_ERROR;
	}
    }
  return TCL_OK;
}

int
Synchronize (ClientData eciHandle, Tcl_Interp * interp,
	     int objc, Tcl_Obj * CONST objv[])
{
  int rc = _eciSynchronize (eciHandle);

  ENTER("Synchronize");

  if (!rc)
    {
      Tcl_SetResult (interp, "Internal tts synth error", TCL_STATIC);
      return TCL_ERROR;
    }
  if (dsp != -1)
    {
      close (dsp);
      dsp = -1;
    }
  return TCL_OK;
}

int
Stop (ClientData eciHandle, Tcl_Interp * interp, int objc,
      Tcl_Obj * CONST objv[])
{

  ENTER("Stop");

  if (_eciStop (eciHandle))
    {
      if (dsp != -1)
	{
	  close (dsp);
	  dsp = -1;
	}
      return TCL_OK;
    }
  Tcl_SetResult (interp, "Could not stop synthesis", TCL_STATIC);
  return TCL_ERROR;
}

int
SpeakingP (ClientData eciHandle, Tcl_Interp * interp, int
	   objc, Tcl_Obj * CONST objv[])
{

  ENTER("SpeakingP");

  if (_eciSpeaking (eciHandle))
    {
      Tcl_SetObjResult (interp, Tcl_NewIntObj (1));
    }
  else
    {
      Tcl_SetObjResult (interp, Tcl_NewIntObj (0));
    }
  return TCL_OK;
}

int
Pause (ClientData eciHandle, Tcl_Interp * interp, int objc,
       Tcl_Obj * CONST objv[])
{
  ENTER("Pause");

  if (_eciPause (eciHandle, 1))
    return TCL_OK;
  Tcl_SetResult (interp, "Could not pause synthesis", TCL_STATIC);
  return TCL_ERROR;
}

int
Resume (ClientData eciHandle, Tcl_Interp * interp, int objc,
	Tcl_Obj * CONST objv[])
{

  ENTER("Resume");

  if (_eciPause (eciHandle, 0))
    return TCL_OK;
  Tcl_SetResult (interp, "Could not resume synthesis", TCL_STATIC);
  return TCL_ERROR;
}

int
openDSP ()
{
  int tmp, _dsp;

  ENTER("openDSP");


  _dsp = open (DSP, O_WRONLY);
  if (_dsp == -1)
    {
      return -1;
    }
  dsp = _dsp;
  ioctl (dsp, SNDCTL_DSP_RESET, 0);
  tmp = 11025;
  ioctl (dsp, SNDCTL_DSP_SPEED, &tmp);
  tmp = 1;
  ioctl (dsp, SNDCTL_DSP_STEREO, &tmp);
  tmp = 16;
  ioctl (dsp, SNDCTL_DSP_SAMPLESIZE, &tmp);
  tmp = 11025;
  ioctl (dsp, SNDCTL_DSP_GETBLKSIZE, &tmp);
  return dsp;
}

int
closeDSP (ClientData eciHandle, Tcl_Interp * interp, int objc,
	  Tcl_Obj * CONST objv[])
{

  ENTER("closeDSP");

  close (dsp);
  dsp = -1;
  return TCL_OK;
}

int
setOutput (ClientData eciHandle, Tcl_Interp * interp, int objc,
	   Tcl_Obj * CONST objv[])
{
  int rc, length, tmp;
  char *output;

  ENTER("setOutput");


  if (objc != 2)
    {
      Tcl_AppendResult (interp, "Usage: setOutput [buffer | default] ",
			TCL_STATIC);
      return TCL_ERROR;
    }
  output = Tcl_GetStringFromObj (objv[1], &length);
  if (Tcl_StringMatch (output, "buffer"))
    {
      fprintf (stderr, "setting output to buffer\n");
      //<set output wave buffer 
      rc = _eciSynchronize (eciHandle);
      if (!rc)
	{
	  Tcl_AppendResult (interp, "Error  resetting TTS engine.\n", NULL);
	  return TCL_ERROR;
	}
      rc = _eciSetOutputBuffer (eciHandle, BUFSIZE, waveBuffer);
      if (!rc)
	{
	  Tcl_AppendResult (interp, "Error setting output buffer.\n", NULL);
	  return TCL_ERROR;
	}
      openDSP ();
      if (dsp == -1)
	{
	  Tcl_AppendResult (interp, "Could not open output device ",
			    DSP, NULL);
	  return (TCL_ERROR);
	}
      //>
    }
  else if (Tcl_StringMatch (output, "default"))
    {
      //stop using wave buffers
      fprintf (stderr, "switching to default device.\n");
      rc = _eciSetOutputDevice (eciHandle, -1);
      if (!rc)
	{
	  Tcl_AppendResult (interp,
			    "Failed to set output to default device. \n",
			    NULL);
	  return TCL_ERROR;
	}
      rc = _eciSetOutputBuffer (eciHandle, 0, NULL);
      if (!rc)
	{
	  Tcl_AppendResult (interp, "Error unsetting output buffer.\n", NULL);
	  return TCL_OK;
	}
      close (dsp);
    }
  else
    {
      Tcl_AppendResult (interp, "Usage: setOutput [buffer | default] ",
			TCL_STATIC);
      return TCL_ERROR;
    }
  fprintf (stderr, "Set output to %s\n", output);
  return TCL_OK;
}

int
getTTSVersion (ClientData eciHandle, Tcl_Interp * interp, int objc,
	       Tcl_Obj * CONST objv[])
{
  ENTER("getTTSVersion");
  char *version = (char *) malloc (16);

  if (objc != 1)
    {
      Tcl_AppendResult (interp, "Usage: ttsVersion   ", TCL_STATIC);
      return TCL_ERROR;
    }
  _eciVersion (version);
  Tcl_SetResult (interp, version, TCL_STATIC);
  return TCL_OK;
}


int Set_Language(ClientData eciHandle, Tcl_Interp *interp,
		 int objc, Tcl_Obj *CONST objv[]) 
{
  ENTER("Set_Language");
  int aIndex=0;
  
  if (getLangIndex( interp, &aIndex))
    {
      int rc;
      sprintf(buffer1,"`l%s", TheLanguages[aIndex].id);
      SHOW2("text=%s=\n", buffer1);
      SHOW2("label=%s=\n", TheLanguages[aIndex].label);
      rc = _eciAddText(eciHandle, buffer1);
    }
  return TCL_OK;
}

//<end of file 
//local variables:
//folded-file: t
//end:
//>
