// ----------------------------------------------------------------------------
// getvoice.c
// $Id: getvoice.c,v 1.2 2005/01/30 21:43:51 gcasse Exp $
// $Author: gcasse $
// Description: Used at installation time to build the "voices" used by oralux
// $Date: 2005/01/30 21:43:51 $ |
// $Revision: 1.2 $ |
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
      printf("Code: %s, %s\n",languageINFO->Entries[i].lang_code,languageINFO->Entries[i].lang_name);
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
  if (strcmp(theLanguageId,"ru")!=0)
    {
      initDECtalk(theLanguageId);
    }
}

static char* TheLanguageId[]={
  "us",
  "fr",
  "gr",
  "sp",
  "ru",
};

// To have similar rate, the values seem to be different
static char* TheDefaultRate[]={
  "130",
  "160",
  "160",
  "160",
  "", // ru
};

static char* TheCurrentRate;

void getvoice(char* theSentence, char* theOggFilename, int thePunctuationToBeSaid)
{
  ENTER("getvoice");
  SHOW(theSentence);
  SHOW(theOggFilename);
  int aStatus;
  static char buf[BUFSIZE];
  static char tmpFile[BUFSIZE];

  if (theSentence==NULL)
  {
    return;
  }

//  sprintf(tmpFile, "%s.wav", theOggFilename);
//  SHOW(tmpFile);
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

void getvoiceRussian(char* theSentence, char* theOggFilename)
{
  ENTER("getvoiceRussian");
  SHOW(theSentence);
  SHOW(theOggFilename);
  int aStatus;
  static char buf[BUFSIZE];
  static char aSentence[BUFSIZE];
  static char tmpFile[BUFSIZE];
  int i;

  if (theSentence==NULL)
  {
    return;
  }

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

  sprintf(buf,"echo \"%s\" | ru_tts -s /usr/local/lib/ru_tts/lexicon -r 0.14 -p 0.19 | exec sox -t raw -s -b -r 10000 -c 1 -v 0.70 - -c 1 -w %s", aSentence, TheWav);

  system(buf);
  SHOW(buf);

  sprintf(buf, "oggenc -Q -o %s %s", theOggFilename, TheWav);
  SHOW(buf);
  system(buf);
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
  static char buf[BUFSIZE];
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

  getcwd(buf, BUFSIZE);
  strcat(buf,"/../audio"),
  initAUI(buf, aLanguage, NULL);

  init(TheLanguageId[aLanguage]);
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

    if (aUniqueFileToBuild)
      {
	aCharacterMin=aValue; 
	aCharacterMax=aValue+1;
      }

    for (aCharacter = aCharacterMin; aCharacter < aCharacterMax; ++aCharacter)
    {
      if ((aCharacter>=0x41) && (aCharacter<=0x5A))
      {
        continue;
      }
      a[0]=aCharacter;
      getvoice(a, getOggFilenameChar(aCharacter), aPunctuationToBeSaid);
    }
  }
  else
  {
    int aNumberOfNotYetTranslatedSentences=0;
    int aSentenceMin=0;
    int aSentenceMax=MaxSentence;

    if (aUniqueFileToBuild)
      {
	aSentenceMin=aValue; 
	aSentenceMax=aValue+1;
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
	else if (aLanguage==Russian)
	  {
	    getvoiceRussian(getText(i), getOggFilename(i));
	  }
	else
	  {
	    getvoice(getText(i), getOggFilename(i), aPunctuationToBeSaid);
	  }
    }
    if (aNumberOfNotYetTranslatedSentences)
      {
	printf("%d of not yet translated sentences (total: %d)\n", aNumberOfNotYetTranslatedSentences, MaxSentence);
      }
    else if (aUniqueFileToBuild)
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

