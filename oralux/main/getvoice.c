// ----------------------------------------------------------------------------
// getvoice.c
// $Id: getvoice.c,v 1.9 2006/04/17 09:11:42 gcasse Exp $
// $Author: gcasse $
// Description: Used at installation time to build the "voices" used by oralux
// $Date: 2006/04/17 09:11:42 $ |
// $Revision: 1.9 $ |
// Copyright (C) 2003, 2004, 2005 Gilles Casse (gcasse@oralux.org)
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
#include <dtk/ttsapi.h>
#include <sys/time.h>
#include <assert.h>

#include "audioUserInterface.h"
#include "constants.h"

static char* TheWav="/tmp/getvoice.wav";

LPTTS_HANDLE_T dtkHandle=NULL;

// For link
void setConsoleFont(enum language theLanguage)
{
}

//

void initDECtalk(char* theLanguageId) 
{
  ENTER("initDECtalk");
  int status;
/*   unsigned int  devNo = 0; */
/*   DWORD devOptions = 0; */
/*   devOptions |= WAVE_OPEN_SHAREABLE; */

  unsigned int devNo = WAVE_MAPPER;
  DWORD devOptions = OWN_AUDIO_DEVICE;

  unsigned int handle;

  LPLANG_ENUM languageINFO;
  if (TextToSpeechEnumLangs(&languageINFO)==0)
  {
    printf("Error: TextToSpeechEnumLangs\n");
    exit(1);
  }

  if (languageINFO->MultiLang)
  {
    int i;
    for(i=0;i<languageINFO->Languages;i++)
    {
      printf("DECtalk Code: %s, %s\n",languageINFO->Entries[i].lang_code,languageINFO->Entries[i].lang_name);
    }
  }

  handle=TextToSpeechStartLang(theLanguageId);

  if (handle & TTS_LANG_ERROR) {
    printf("Error Loading %s\n",theLanguageId);
    exit(1);
  }

  if (TextToSpeechSelectLang(NULL, handle)==FALSE)
  {
    printf("Error selecting language\n");
    TextToSpeechCloseLang(theLanguageId);
    exit(1);    
  }

  status = TextToSpeechStartup( &dtkHandle, devNo,
                                devOptions, NULL, 0 );
  if (dtkHandle == NULL) { 
    printf("Could not open text-to-speech engine");
    exit(1);
  }

  
// DWORD dwFlags=LOG_TEXT|LOG_PHONEMES|LOG_SYLLABLES;
// status = TextToSpeechOpenLogFile(dtkHandle, "dtk.log", dwFlags); 
}

int init(char* theLanguageId) 
{
  ENTER("initDECtalk");
  if ((strcmp(theLanguageId,"ru")!=0) && (strcmp(theLanguageId,"pt_BR")!=0))
    {
      initDECtalk(theLanguageId);
    }
}

static char* TheLanguageId[]={
  "us",
  "fr",
  "gr",
  "sp",
  "pt_BR",
  "ru",
  "all",
};

// To have similar rate, the values seem to be different
static char* TheDefaultRate[]={
  "130",
  "160",
  "160",
  "160",
  "", // br
  "", // ru
  "", // all
};

static char* TheCurrentRate;

void getdtkvoice(char* theSentence, char* theOggFilename, int thePunctuationToBeSaid)
{
  ENTER("getdtkvoice");
  SHOW(theSentence);
  SHOW(theOggFilename);
  int aStatus;
  static char buf[BUFSIZE];

  if (theSentence==NULL)
  {
    return;
  }

  aStatus =  TextToSpeechOpenWaveOutFile (
    dtkHandle,   
    TheWav,   
    WAVE_FORMAT_1M16); // 1M08 ? 1M16 OK -2M16-2M08-2S08

  if (aStatus)
    {
      printf ("Error: opening %s using TextToSpeechOpenWaveOutFile, aStatus=%x\n", TheWav, aStatus);
      exit;
    }

  char* aPunctuation= (thePunctuationToBeSaid) ? " " : "[:pu none]";
  sprintf(buf, "[:np] [:rate %s] %s %s", TheCurrentRate, aPunctuation, theSentence);

  aStatus=(int)TextToSpeechSpeak(dtkHandle, buf, TTS_FORCE);
  //  printf("\nTextToSpeechSpeak status=%d",aStatus);

  aStatus=(int)TextToSpeechSync(dtkHandle); 
  // printf("\nTextToSpeechSync status=%d",aStatus);

  TextToSpeechCloseWaveOutFile (dtkHandle); 
  system("sync");
//  TextToSpeechCloseLogFile(dtkHandle);
  sprintf(buf, "oggenc -Q -o %s %s", theOggFilename, TheWav);
  SHOW(buf);
  system(buf);
}

void getvoice(char* theSentence, char* theOggFilename, enum language theLanguage)
{
  ENTER("getvoice");
  SHOW(theSentence);
  SHOW(theOggFilename);
  int aStatus;
  static char buf[2*BUFSIZE];
  static char aSentence[BUFSIZE];
  int i;

  if (theSentence==NULL)
  {
    return;
  }
  assert(strlen(theSentence) < BUFSIZE);

  if (theLanguage == Russian)
    {
      for (i=0; i<strlen(theSentence); i++)
	{
	  switch(theSentence[i])
	    {
	    case ',':
	    case ';':
	    case '!':
	    case '?':
	    case '(':
	    case ')':
	    case '-':
	    case '_':
	    case '[':
	    case ']':
	      aSentence[i]=' ';
	      break;

	    case '.':
	      aSentence[i] = (isdigit( theSentence[i+1])) ? '.' : ' ';
	      break;

	    default:
	      aSentence[i]=theSentence[i];
	      break;
	    }
	}
      aSentence[i]=0;
    }
  else
    {
      strcpy(aSentence, theSentence);
    }


  switch( theLanguage)
    {
    case Brazilian:
      sprintf(buf,"echo \"%s\" | ttpport | mbrola -t 0.89 -f 1.00 -v 1.00 -l 16000 -e /usr/local/share/mbrola/voices/br1 - -A | exec sox -t raw -s -w -r 16000 -c 1 -v 0.80 - -c 1 -w %s", aSentence, TheWav);  
      break;
    case English:
      break;
    case French:
      break;
    case German:
      break;
    case Spanish:
      break;
    case Russian:
      sprintf(buf,"echo \"%s\" | ru_tts -s /usr/local/lib/ru_tts/lexicon -r 0.14 -p 0.19 | exec sox -t raw -s -b -r 10000 -c 1 -v 0.80 - -c 1 -w %s", aSentence, TheWav);  
      break;
    }

  system(buf);
  SHOW(buf);

  sprintf(buf, "oggenc -Q -o %s %s", theOggFilename, TheWav);
  SHOW(buf);
  system(buf);
}

static void buildAudioFiles( int theLanguage, int isCharacter, int theUniqueFileToBuild, int theValue)
{
  static char buf[BUFSIZE];

  initAUI(ORALUX_RUNTIME_AUDIO, theLanguage, NULL);

  init(TheLanguageId[theLanguage]);
  int aPunctuationToBeSaid=0;

  if (isCharacter)
  {
    // RAF(GC): pronouncing 'Space' for 0x20, + others?
    char a[2];
    a[1]=0;
    char aCharacter;
    char aCharacterMin=0x20;
    char aCharacterMax=0x7f;
    aPunctuationToBeSaid=1;

    if (theUniqueFileToBuild)
      {
	aCharacterMin=theValue; 
	aCharacterMax=theValue+1;
      }

    for (aCharacter = aCharacterMin; aCharacter < aCharacterMax; ++aCharacter)
    {
      if ((aCharacter>=0x41) && (aCharacter<=0x5A))
      {
        continue;
      }
      a[0]=aCharacter;
      switch(theLanguage)
	{
	case Brazilian:
	case Russian:
	    getvoice( a, getOggFilenameChar(aCharacter), theLanguage);
	  break;

	default:
	  getdtkvoice(a, getOggFilenameChar(aCharacter), aPunctuationToBeSaid);
	  break;
	}
    }
  }
  else
  {
    int aNumberOfNotYetTranslatedSentences=0;
    int aSentenceMin=0;
    int aSentenceMax=MaxSentence;
    int i;

    if (theUniqueFileToBuild)
      {
	aSentenceMin=theValue; 
	aSentenceMax=theValue+1;
      }

    for (i=aSentenceMin; i<aSentenceMax; ++i)
    {
        char* aSentence=getText(i);
	if (aSentence==NULL)
	  { // Not yet translated, we copy the English version.
	    char* aEnglishFilename=getOggFilenameForThisLanguage(i, English);
	    char* aCurrentLanguageFilename=getOggFilename(i);
	    sprintf(buf, "cp %s %s", aEnglishFilename, aCurrentLanguageFilename);
	    system(buf);
	    aNumberOfNotYetTranslatedSentences++;
	  }
	else
	  {
	    char* aSentence = getText(i);
	    printf("s%02x %s\n", i, aSentence);
	    switch(theLanguage)
	      {
	      case Brazilian:
	      case Russian:
		getvoice(aSentence, getOggFilename(i), theLanguage);
		break;

	      default:
		getdtkvoice(aSentence, getOggFilename(i), aPunctuationToBeSaid);
		break;
	      }
/* 		{ */
/* 		  static char buf[200]; */
/* 		sprintf(buf,"ogg123 %s\n", getOggFilename(i)); */
/* 		system(buf); */
/* 		} */
	  }
    }
    if (aNumberOfNotYetTranslatedSentences)
      {
	printf("%d of not yet translated sentences (total: %d)\n", aNumberOfNotYetTranslatedSentences, MaxSentence);
      }
    else if (theUniqueFileToBuild)
      {
	printf("The sentence has been translated\n");
      }
    else
      {
	printf("All %d sentences have been translated\n", MaxSentence);
      }
  }
  stopAUI(0);
}

void usage()
{
  printf("getvoice: builds the ogg files for the printable characters (c) or the sentences (s)\n");
  printf("getvoice language c|s [num (xx hexa)]\n");
  printf("with language:\n");
  int j=0;
  int aNumberOfLanguages=sizeof(TheLanguageId)/sizeof(TheLanguageId[0]);
  for (;j<aNumberOfLanguages;j++) 
  {
    printf("%s\n",TheLanguageId[j]);
  }
}

int main(int argc, char *argv[])
{
  enum sentence i;
  int aValue=0;
  int aUniqueFileToBuild=0;

  if ((argc==4) 
      && (strlen(argv[3])==2)
      && isxdigit(argv[3][0])
      && isxdigit(argv[3][1]))
  {
    aValue = strtol(argv[3], NULL, 16);
    aUniqueFileToBuild = 1;
  }
  else if (argc!=3)
  {
    usage();
    exit(1);
  }

  int aLanguage=0;
  int aNumberOfLanguages=sizeof(TheLanguageId)/sizeof(TheLanguageId[0]);
  for (;aLanguage<aNumberOfLanguages;aLanguage++)
  {
    if (strcmp(argv[1],TheLanguageId[aLanguage])==0)
    {
      TheCurrentRate=TheDefaultRate[aLanguage];
      break;
    }
  }

  if (aLanguage==aNumberOfLanguages)
  {
    usage();
    exit(1);
  }

  int isCharacter=0;
  if (strcmp(argv[2],"c")==0)
  {
    isCharacter=1;
  }
  else if (strcmp(argv[2],"s")!=0)
  {
    usage();
    exit(1);
  }

  if (strcmp( TheLanguageId[aLanguage], "all")==0)
    {
      for (aLanguage=0; aLanguage < aNumberOfLanguages-1; aLanguage++)
	{
	  buildAudioFiles(aLanguage, isCharacter, aUniqueFileToBuild, aValue);
	}
    }
  else
    {
      buildAudioFiles(aLanguage, isCharacter, aUniqueFileToBuild, aValue);
    }
}

