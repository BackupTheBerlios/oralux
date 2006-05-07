// ----------------------------------------------------------------------------
// textToSpeech.c
// $Id: textToSpeech.c,v 1.13 2006/05/07 23:15:58 gcasse Exp $
// $Author: gcasse $
// Description: Ask about the whished TTS and install it. 
// $Date: 2006/05/07 23:15:58 $ |
// $Revision: 1.13 $ |
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
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "audioUserInterface.h"
#include "constants.h"
#include "getnchar.h"
#include "textToSpeech.h"
#include "menu.h"

static char TheLine[BUFSIZE];
static int pf=0;

/* < data */

struct synthItem
{
  enum textToSpeech myIdentifier;
  enum sentence mySentence;
  int myCompliantDesktop;
  char* myLabel;
  int mySynthIsExternal;
};

typedef struct synthItem synthItem;


static synthItem myProposedSynth[]= { 
  //  {TTS_ViaVoice, DoYouWantViaVoice, Emacspeak|Yasr, "ViaVoice", 0}, // index=0
  {TTS_Multispeech, DoYouWantMultispeech, Emacspeak|Speakup|Yasr, "Multispeech", 0},
  {TTS_Flite, DoYouWantFlite, Emacspeak|Speakup|Yasr, "Flite", 0},
  {TTS_Cicero, DoYouWantCicero, Emacspeak|Yasr, "Cicero", 0},
  {TTS_DECtalk, DoYouWantToInstallDECtalk, Emacspeak|Yasr, "DECtalk", 0},
  {TTS_AccentSA, AccentSA, Emacspeak|Speakup, "AccentSA", 1},
  {TTS_AccentPC, AccentPC, Speakup, "AccentPC", 1},
  {TTS_Audapter, Audapter, Speakup, "Audapter", 1},
  {TTS_BrailleLite, BrailleLiteTTS, Emacspeak|Speakup,"BrailleLite", 1},
  {TTS_BrailleNSpeak, BrailleNSpeak, Emacspeak,"BrailleNSpeak", 1},
  {TTS_Ciber232, Ciber232, Emacspeak,"Ciber232", 1},
  {TTS_Ciber232Plus,Ciber232Plus, Emacspeak,"Ciber232Plus", 1},
  {TTS_DECtalkExpress,DECtalkExpress, Emacspeak|Speakup,"DECtalkExpress", 1},
  {TTS_DECtalkExternal,DECtalkExternal, Speakup, "DECtalkExternal", 1},
  {TTS_DECtalkMultivoice,DECtalkMultivoice, Emacspeak,"DECtalkMultivoice", 1},
  {TTS_DECtalk3,DECtalk3, 0,"DECtalk3", 1},
  {TTS_DolphinApollo,DolphinApollo, Emacspeak|Speakup,"DolphinApollo", 1},
  {TTS_DoubleTalkLT,DoubleTalkLT, Emacspeak|Speakup,"DoubleTalkLT", 1},
  {TTS_DoubleTalkPC,DoubleTalkPC, Emacspeak|Speakup,"DoubleTalkPC", 1},
  {TTS_KeynoteGoldPC,KeynoteGoldPC, Speakup,"KeynoteGoldPC", 1},
  {TTS_LiteTalk,LiteTalk, 0,"LiteTalk", 1},
  {TTS_PcHabladoNotebook,PcHabladoNotebook, Emacspeak,"PcHabladoNotebook", 1},
  {TTS_SpeakOut,SpeakOut, Speakup, "SpeakOut", 1},
  {TTS_Transport,Transport, Speakup, "Transport", 1},
  {TTS_TypeNSpeak,TypeNSpeak, 0,"TypeNSpeak", 1},
};
#define MaxProposedSynth (sizeof(myProposedSynth)/sizeof(myProposedSynth[0]))

struct TextToSpeechCheatCode
{
  enum textToSpeech myIdentifier;
  char* myCode;
};

static struct TextToSpeechCheatCode myEmacspeakCheatCode[]={
  {TTS_AccentSA, "accent"},
  {TTS_BrailleLite, "braillenspeak"},
  {TTS_Ciber232, "ciber"},
  {TTS_DECtalkExpress,"dtk-exp"},
  {TTS_DECtalkMultivoice,"dtk-mv"},
  {TTS_DolphinApollo,"apollo"},
  {TTS_DoubleTalkLT,"doubletalk"},
  {TTS_DoubleTalkPC,"doubletalkpc"},
  {TTS_PcHabladoNotebook,"pchablado"},
};

/* < external synth: menu*/

enum MENU_ExtSynthState {
  MENU_ExtSynth,
  MENU_Port,
  MENU_End,
};

enum TypeOfSynth {AnySynth, OnlySoftwareSynth, OnlyExternalSynth};

/* > */


/* < getIndex, getNextIndex, getPreviousIndex */

static int getIndex( enum textToSpeech theIdentifier, enum desktopIdentifier theDesktop, int* theIndex)
{
  ENTER("getIndex");
  int aResult=0;
  for (*theIndex=0; *theIndex<MaxProposedSynth; ++*theIndex)
    {
      if (myProposedSynth[*theIndex].myIdentifier == theIdentifier)
	{ 
	  if (myProposedSynth[*theIndex].myCompliantDesktop & theDesktop)
	    {
	      aResult = 1;
	    }
	  break;
	}
    }

  if (!aResult)
    { // fallback
      for (*theIndex=0; *theIndex<MaxProposedSynth; ++*theIndex)
	{
	  if (myProposedSynth[*theIndex].myCompliantDesktop & theDesktop)
	    {
	      aResult = 1;
	      break;
	    }
	}
    }

  return aResult;
}

static int condition( int* theIndex, enum desktopIdentifier theDesktop, enum TypeOfSynth theType)
{
  ENTER("condition");
  int aResult=0;
  if (myProposedSynth[*theIndex].myCompliantDesktop & theDesktop)
    {
      switch(theType)
	{
	case AnySynth:
	  aResult = 1;
	  break;
	case OnlySoftwareSynth:
	  aResult = (0==myProposedSynth[*theIndex].mySynthIsExternal);
	  break;
	case OnlyExternalSynth:
	  aResult = (1==myProposedSynth[*theIndex].mySynthIsExternal);
	  break;
	}
    } 
  return aResult;
}

static int getNextIndex( int* theIndex, enum desktopIdentifier theDesktop, enum TypeOfSynth theType)
{
  ENTER("getNextIndex");
  int aResult=0;
  int n = MaxProposedSynth;
  
  while( n-- && !aResult)
    {
      *theIndex = (*theIndex >= MaxProposedSynth-1) ? 0 : ++*theIndex;
      aResult = condition( theIndex, theDesktop, theType);
    }

  return aResult;
}

static int getPreviousIndex( int* theIndex, enum desktopIdentifier theDesktop, enum TypeOfSynth theType)
{
  ENTER("getPreviousIndex");
  int aResult=0;
  int n = MaxProposedSynth;

  while( n-- && !aResult)
    {
      *theIndex = (*theIndex > 0) ? --*theIndex : MaxProposedSynth-1;
      aResult = condition( theIndex, theDesktop, theType);
    }

  return aResult;
}
/* > */
int isExternalSynthPossible( enum desktopIdentifier theDesktop)
{
  ENTER("isExternalSynthPossible");
  int aResult = 0;
  int i;
  for (i=0; i<MaxProposedSynth; i++)
    {
      if (myProposedSynth[i].mySynthIsExternal 
	  && (myProposedSynth[i].myCompliantDesktop & theDesktop) )
	{
	  aResult=1;
	  break;
	}
    }
  return aResult;
}
/* < HasEmacspeakExternalTextToSpeech */
// Read the oralux.conf file to see if an external synth must be used instead of the sound card.
//
int HasEmacspeakExternalTextToSpeech(struct textToSpeechStruct* theExternalTextToSpeech)
{
  ENTER("HasEmacspeakExternalTextToSpeech");
  FILE* fd=fopen("/etc/oralux.conf","r");
  int aResult=0;

  if (fd==NULL)
    {
      return 0;
    }

  char* aBuffer=(char *)malloc(BUFSIZE);
  char* aTextToSpeech=NULL;
  char* aPort=NULL;

  while(fgets(aBuffer, BUFSIZE, fd))
  {
    const char* aKeyword="tts=";
    char* aString=NULL;
    if (strncmp(aBuffer, aKeyword, sizeof(aKeyword))==0)
      {
	// removing the trailing char \r \n
	aString=strstr( aBuffer, "\r");
	if (aString != NULL)
	  {
	    *aString=0;
	  }
	aString=strstr( aBuffer, "\n");
	if (aString != NULL)
	  {
	    *aString=0;
	  }

	aTextToSpeech = aBuffer + sizeof(aKeyword);

	aString=strstr( aBuffer, ",");
	if (aString != NULL)
	  {
	    *aString=0;

	    // Port
 	    aPort = aString+1; 
	  }
	break;
      }
  }

  if (aTextToSpeech)
    {
      int aMaxCheatCode=sizeof(myEmacspeakCheatCode)/sizeof(myEmacspeakCheatCode[0]);
      int i=0;

      // Looking for the index in the array
      for (i=0; i<aMaxCheatCode; i++)
	{
	  if (strcmp(aTextToSpeech, myEmacspeakCheatCode[i].myCode)==0)
	    {
	      theExternalTextToSpeech->myIdentifier=myEmacspeakCheatCode[i].myIdentifier;
	      aResult=1;
	      break;
	    }
	}
    }

  if (aResult)
    {
      theExternalTextToSpeech->myPort = getEnumSerialPort(aPort);
      theExternalTextToSpeech->myLanguage = English;
    }

  fclose(fd);
  free(aBuffer);
  return aResult;
}
/* > */
/* < setSoftwareSynth */
/* static void setViavoice(enum desktopIdentifier theDesktop) */
/* { */
/*   ENTER("setViavoice"); */
/*   int i=0; */

/*   if (getIndex( TTS_ViaVoice, theDesktop, &i)) */
/*     { */
/*       struct stat buf; */
/*       int isViaVoicePresent = ((stat("/usr/lib/ibmeci", &buf)==0) */
/* 			       || (stat("/usr/share/oraluxGold", &buf)==0)); */
      
/*       myProposedSynth[i].myCompliantDesktop = isViaVoicePresent; */
/*     } */
/* } */
/* > */

static enum language getSoftwareSynthLanguage(enum textToSpeech theTextToSpeech, enum language thePreferredLanguage)
{
  ENTER("getSoftwareSynthLanguage");
  enum language aLanguage;

  switch (theTextToSpeech)
    {
    case TTS_Multispeech:
    case TTS_ViaVoice:
    case TTS_DECtalk:
      aLanguage = thePreferredLanguage;
      break;
      
    case TTS_Cicero:
      aLanguage = French;
      break;

    default:
    case TTS_Flite:
      aLanguage = English;
      break;
    }

  return aLanguage;
}


/* < chooseSynth */
// chooseSynth
// Asking to the user which voice synthesis is required
// Return true if a synth is selected
// false if  menu must exit
//
static int chooseSynth( struct textToSpeechStruct* theTextToSpeech,
		       enum language thePreferredLanguage,
		       enum desktopIdentifier theDesktop)
{
  ENTER("chooseSynth");
  int aStatus=0;
  int aRequest=0;
  int aIndex=0;
  enum TypeOfSynth aType = OnlySoftwareSynth;
  int isFirstRequest=1;

  //  setViavoice(theDesktop);

  //  ??ajout dectalk software
  if (getIndex( theTextToSpeech->myIdentifier, theDesktop, &aIndex))
    {
      say(yourVoiceSynthIs);
      say(myProposedSynth[aIndex].mySentence);
      //      saySynth();
      say(changeTypeOfSynth);
      say(PleasePressKey);
      
      if (getAnswer() == MENU_Yes)
	{
	  aRequest=1;
	  
	  if (isExternalSynthPossible(theDesktop))
	    {
	      // External or software synth?
	      int a_loop=1;
	      enum TypeOfSynth aCurrentType = (myProposedSynth[aIndex].mySynthIsExternal) ? 
		OnlyExternalSynth : OnlySoftwareSynth;
	      aType = aCurrentType;

	      while(a_loop)
		{
		  if (aType == OnlyExternalSynth)
		    {
		      say(selectExternalSynth);
		    }
		  else
		    {
		      say(selectSoftSynth);
		    }
		  
		  if(getAnswer() == MENU_Yes)
		    {
		      a_loop=0;
		    }
		  else
		    {
		      aType = (aType == OnlySoftwareSynth) ?  OnlyExternalSynth : OnlySoftwareSynth;
		    }
		}

	      if (aCurrentType != aType)
		{
		  aRequest = getNextIndex( &aIndex, theDesktop, aType);
		}
	    }
	}
    }

  while(aRequest)
    {
      say(myProposedSynth[aIndex].mySentence);
      if (isFirstRequest)
	{
	  say(PleasePressKey);
	  isFirstRequest = 0;
	}

      switch(getAnswer())
	{
	case MENU_Yes:
	  theTextToSpeech->myIdentifier = myProposedSynth[aIndex].myIdentifier;
	  if (myProposedSynth[aIndex].mySynthIsExternal)
	    {
	      serialPortChoose( & theTextToSpeech->myPort);
	    }
	  else
	    {
	      theTextToSpeech->myLanguage = getSoftwareSynthLanguage(theTextToSpeech->myIdentifier, thePreferredLanguage);
	    }
	  aRequest=0;
	  aStatus=1;
	  break;

	case MENU_Previous:
	  aRequest = getPreviousIndex( &aIndex, theDesktop, aType);
	  break;
	  
	case MENU_Next:
	  aRequest = getNextIndex( &aIndex, theDesktop, aType);
	  break;

	case MENU_No:
	default:
	  aRequest = getNextIndex( &aIndex, theDesktop, aType);
	  break;
	}

      SHOW3("aRequest=%d, aIndex=%d\n",aRequest, aIndex);
    }

  return aStatus;
}
/* > */
/* < DECtalk or Viavoice install */

static int readPHPStatus(enum phpStatus* theStatus, char* theDtkPath, enum language* theTextToSpeechLanguage);
static int BuildingEmacspeakSharedLibrary();
static int isFileConsistent(char* theFilename);
static int installDECtalk(enum language* theTextToSpeechLanguage);
static int installViaVoice();

static char* theOptionalDirectory[]={
  NULL, 
  "french",
  "german",
  "spanish",
};

// Where DECtalk libraries are stored
#define DTK_PATH_LIB "/ramdisk/dtk/lib"

static char* theDECtalkLibrary[]={
  "tts_us", 
  "tts_fr",
  "tts_gr",
  "tts_sp",
};

// readPHPStatus
// Read the php status (a simple file) and may pronounce the related status.
//
// Output 
// theStatus: the php status 
// theDtkPath: where the user creates the dtk directory
// theLanguage: the optional language if any (e.g; french), or the default one (english)
// Return
// 0: fatal error, no php status file 
static int readPHPStatus(enum phpStatus* theStatus, char* theDtkPath, enum language* theTextToSpeechLanguage)
{
  ENTER("readPHPStatus");
  *theStatus=PHP_OK;
  *theDtkPath=0;
  *theTextToSpeechLanguage=English;
  
  FILE* fd=fopen("/tmp/phpstatus", "r");

  if (fd==NULL)
  {
    say(ProblemWithPHPStatus);
    return 0;
  }

  // The first line is for the status
  fgets(TheLine,BUFSIZE,fd);

  // Saying the status
  if (strlen(TheLine)<2)
  {
    return 0;
  }

  TheLine[strlen(TheLine)-1]=0; // remove the newline
  *theStatus=atoi(TheLine);

  // The second line is the dtk path (if any)
  fgets(theDtkPath, BUFSIZE, fd);
  theDtkPath[strlen(theDtkPath)-1]=0; //removing \n

  // The third one the language
  fgets(TheLine, BUFSIZE, fd);

  if (strcmp(TheLine,"french\n")==0)
  {
    *theTextToSpeechLanguage=French;
  }
  else if (strcmp(TheLine,"german\n")==0)
  {
    *theTextToSpeechLanguage=German;
  }
  else if (strcmp(TheLine,"spanish\n")==0)
  {
    *theTextToSpeechLanguage=Spanish;
  }

  return 1;
}

// ManageDECtalkInstallation
static void ManageDECtalkInstallation( enum textToSpeech* theTextToSpeech,
			    enum language* theTextToSpeechLanguage)
{
  ENTER("ManageDECtalkInstallation");
  *theTextToSpeech=TTS_Multispeech;
  *theTextToSpeechLanguage=English;

  // installDECtalk
  // Trying to install DECtalk
  // First step, placing the DECtalk softwares in the ramdisk
  sprintf(TheLine, "%s/text2speechinstaller/dtkInstallerMain.php", ORALUX_RUNTIME);
  SHOW(TheLine);
  system(TheLine);

  // Reading the php status
  enum phpStatus aStatus=PHP_OK;
  static char aDtkPath[BUFSIZE];
  static char aLanguage[BUFSIZE];

  if (!readPHPStatus(&aStatus, aDtkPath, theTextToSpeechLanguage))
    {
      say(ProblemWithPHPStatus);
    }
  else
    {
      switch(aStatus)
	{
	case PHP_DECTALK_CANT_BE_FOUND:
	  say(DectalkCantBeFound);
	  break;
	case PHP_MAKEFILE_CANT_BE_FOUND:
	  say(MakefileCantBeFound);
	  break;
	case PHP_TCLDTK_LIB_NOT_BUILT:
	  say(TheTcldtkLibraryIsNotBuilt);
	  break;
	case PHP_EMACSPEAK_NOT_DEFINED:
	  say(EmacspeakVariableNotDefined);
	  break;
	case PHP_SOFTWARE_DTK_NOT_FOUND:
	  say(SoftwareDECtalkNotFound);
	  break;
	case PHP_DTK_DIRECTORY_NOT_FOUND:
	  say(DECtalkDirectoryNotFound);
	  break;
	case PHP_SERIAL_NUMBER_IS_EXPECTED:
	  if (installDECtalk( theTextToSpeechLanguage))
	    { // Successfull installation
	      *theTextToSpeech=TTS_DECtalk;

	      // Finally, trying to save the directories in the storage directory
	      sprintf(TheLine,"cp -R %s/DECtalk %s/dtk", RAMDISK, aDtkPath);
	      SHOW(TheLine);
	      system(TheLine);

	      if (theOptionalDirectory[*theTextToSpeechLanguage])
		{
		  sprintf(TheLine,"cp -R %s/%s %s/dtk", RAMDISK, theOptionalDirectory[*theTextToSpeechLanguage], aDtkPath);
		  SHOW(TheLine);
		  system(TheLine);
		}

	      // DECtalk.conf is stored for a future use
	      // Warning: This file is in fact - thanks to a symbolic link - stored in RAMDISK
	      sprintf(TheLine,"cp %s/DECtalk.conf %s/dtk/DECtalk", INSTALL_TREE, aDtkPath);
	      SHOW(TheLine);
	      system(TheLine);
	    }
	  else
	    {
	      *theTextToSpeech=TTS_Multispeech;
	      *theTextToSpeechLanguage=English;
	    }
	  break;
	case PHP_OK:
	  *theTextToSpeech=TTS_DECtalk;
	  break;
	default:
	  break;
	}
    }

/*   // The dtk partition is remount in read only access */
/*   if (0!=strcmp("/",aDtkPath)) // To avoid any problem with a hard disk installation */
/*     { */
/*       sprintf(TheLine, "mount -o remount,ro %s 1>/dev/null", aDtkPath); */
/*       SHOW(TheLine); */
/*       system(TheLine); */
/*     } */

  // Building the emacspeak shared libary
  if ((*theTextToSpeech==TTS_DECtalk) && !BuildingEmacspeakSharedLibrary())
    {
      *theTextToSpeech=TTS_Multispeech;
      *theTextToSpeechLanguage=English;
      say(TheTcldtkLibraryIsNotBuilt);
    }
}

// ManageDECtalkInstallation
static void ManageViaVoiceInstallation( enum textToSpeech* theTextToSpeech,
					enum language* theTextToSpeechLanguage,
					int theUserMustBeAskedFor)
{
  ENTER("ManageViaVoiceInstallation");
  *theTextToSpeech=TTS_Multispeech;
  *theTextToSpeechLanguage=English;

  if (installViaVoice( theUserMustBeAskedFor))
    { // Successfull installation
      *theTextToSpeech=TTS_ViaVoice;  
    }
  else
    {
      *theTextToSpeech=TTS_Flite;
      *theTextToSpeechLanguage=English;
    }
}

// BuildingEmacspeakSharedLibrary
// Once the files of the DECtalk software are installed, the emacspeak shared library must be built (tcldtk.so)
// As the emacspeak original tree from the CD is read-only, the built files (tcldtk.o and tcldtk.so) are 
// symbolic links pointing to the directory /ramdisk/dtk/emacspeak
//
// Returned value:
// 1 if true
//

static int BuildingEmacspeakSharedLibrary()
{
  ENTER("BuildingEmacspeakSharedLibrary");
  int aStatus=0;

  // Compiling the tcldtk.so library
  char* aPath=getenv("EMACSPEAK_DIR");
  if (!aPath)
  {
    return 0;
  }	

  // Removing the previous target files if any
  sprintf(TheLine,"%s/tcldtk.o",DTK_EMACSPEAK);
  unlink(TheLine);
  sprintf(TheLine,"%s/tcldtk.so",DTK_EMACSPEAK);
  unlink(TheLine);

  // Now, crossing fingers
  sprintf(TheLine, "%s/servers/software-dtk", aPath);
  SHOW(TheLine);
  chdir(TheLine);
#ifdef DEBUG
  system("make");
#else
  system("make &>/dev/null");
#endif

  // Checking that the library exists
  sprintf(TheLine, "%s/servers/software-dtk/tcldtk.so", aPath);
  SHOW(TheLine);
  if (isFileConsistent(TheLine))
  {
    aStatus=1;
  }

  return aStatus;
}


// isFileConsistent
// Test if the supplied file exists and is not empty
static int isFileConsistent(char* theFilename)
{
  ENTER("isFileConsistent");
  int isConsistent=0;
  FILE* fd=fopen(theFilename,"r");
  
  if (fd)
  {
    // Checking that the size of the file
    if (fread(TheLine,1,1,fd))
    {
      isConsistent=1;
      fclose(fd);
    }
  }
  return isConsistent;
}


/* < installDECtalk */

// This function asks for the serial number (with an aural feedback)
// and then supplies it to the DECtalk installer
// Finally, if possible the uncompressed files are stored in the dtk directory for the future boot. 
// Output 
// theTextToSpeechLanguage: if the DECtalk software installation succeeds the optional language 
// (if any) is supplied
// return;
// 1 if the installation succeeds
//
static int installDECtalk(enum language* theTextToSpeechLanguage)
{
  ENTER("installDECtalk");

  int aSerialNumberMustBeChecked=1;
  int aCorrectSerialNumber=0;

  chdir("/ramdisk/DECtalk");
    
  while(aSerialNumberMustBeChecked)
  {
    // Starting the DECtalk installer
    say(PleaseEnterYourSerialNumber);
    enum GNC_Answer aStatus=GNC_Ok;

    char* anAnswer = getnchar(pf, 30, 0, 1, &aStatus, keyPressedCallback3);

//    sprintf(TheLine, "echo \"%s\" | ./installer 1>/dev/null", anAnswer);
    sprintf(TheLine, "echo \"%s\" | ./installer", anAnswer);
    system(TheLine);

    // Checking if the previous stage was OK: the DECtalk.conf file must be built
    if (isFileConsistent("/etc/DECtalk.conf"))
    {
      aCorrectSerialNumber=1;
      aSerialNumberMustBeChecked=0;
    }

    if (!aCorrectSerialNumber)
    {
      SHOW(anAnswer);
      say(SorrySerialNumberNotCorrect);
      say(IfYouWantToTryAgainPress);
      say(IfYouWantToStopThisStage);
      aSerialNumberMustBeChecked = (getAnswer() == MENU_Yes); 
    }
  } // while

  // Installing the optional language
  if (aCorrectSerialNumber && (theOptionalDirectory[*theTextToSpeechLanguage]))
  {
    SHOW("Installing the optional directory");
    sprintf(TheLine, "/ramdisk/%s", theOptionalDirectory[ *theTextToSpeechLanguage ]);
    chdir(TheLine);
//    system("./installer 1>/dev/null");
    system("./installer");

    // Checking if the optional lib is present
    sprintf(TheLine, "%s/lib%s.so", DTK_PATH_LIB, theDECtalkLibrary[ *theTextToSpeechLanguage ]);

    if (isFileConsistent(TheLine))
    { // OK
      // Updating the Makefile (emacspeak) so that the optional library is used
      sprintf(TheLine,"sed 's+ltts_us+ltts_us -l%s+g' %s/Makefile >/tmp/Makefiledtk;mv /tmp/Makefiledtk %s/Makefile ", theDECtalkLibrary[ *theTextToSpeechLanguage ], DTK_EMACSPEAK, DTK_EMACSPEAK);

      SHOW(TheLine);
      system(TheLine);
    }
    else
    {
      *theTextToSpeechLanguage=English;
    }
  }

  return aCorrectSerialNumber;
}

/* > */
/* < askIfAnotherViavoiceInstall: return 1 if yes, 0 otherwises */

int askIfAnotherViavoiceInstall( int thePreviousInstallWasOk)
{ 
  int anotherInstall=1;
  if (thePreviousInstallWasOk)
    {
      say(InstallAnotherLanguage);
      say(PleasePressKey);
      
      if (MENU_Yes != getAnswer())
	{
	  anotherInstall=0;
	}
    }
  else
    {
      say(SorryPasswordNotCorrect);
      say(IfYouWantToTryAgainPress);
      say(IfYouWantToStopThisStage);
      anotherInstall = (getAnswer() == MENU_Yes); 
    }
  return anotherInstall;
}

/* > */
/* < installViaVoice */

// install
// This function asks for the serial number (with an aural feedback)
// and then supplies it to the ViaVoice installer
// Finally, if possible the files are stored in the ibmeci directory for the future boot. 
// return;
// 1 if the installation succeeds
//

static int installViaVoice( int theUserMustBeAskedFor)
{
  ENTER("installViaVoice");

  int aPasswordMustBeChecked=1;
  int aCorrectPassword=0;
  struct stat buf;
  const char* aPath="/ramdisk/ibmeci";
  mkdir( aPath, 0755);
  chdir("/ramdisk/ibmeci");

  if (stat("libibmeci.so", &buf) == 0)
    {
      aCorrectPassword=1;
      if (theUserMustBeAskedFor)
	{
	  aPasswordMustBeChecked = askIfAnotherViavoiceInstall(1);
	}
      else
	{
	  aPasswordMustBeChecked = 0;
	}
    }

  while(aPasswordMustBeChecked)
    {
      enum GNC_Answer aStatus=GNC_Ok;
      char a[4];
      char *b, *c;

      say(PleaseEnterYourPassword);

      if ((b=getnchar(pf, 40, 0, 1, &aStatus, keyPressedCallback3)) 
	  && (*b!=0) 
	  && (c=strchr( b, '-')) 
	  && (b+4 == c))
	{
	  for (c=b;*c;c++)
	    {
	      *c = isalpha(*c) ? tolower(*c):*c;
	    }

	  strncpy(a,b,3);
	  a[3] = 0;
	  b[strlen(b)-1] = 0;
	
	  sprintf(TheLine, "%s/text2speechinstaller/installibmeci.sh %s '%s' %s/lib", ORALUX_RUNTIME, a, b, IBMECI_DIR);
	  SHOW(TheLine);
	  system(TheLine);
	
	  // ibmeci.conf exists if the installation succeeded

	  if (stat("/tmp/ibmeci.conf",&buf)==0)
	    {
	      aCorrectPassword=1;
	      sprintf(TheLine, "%s/inigen /usr/lib/ibmeci/%s50.so /etc", IBMECI_DIR, a);
	      SHOW(TheLine);
	      system(TheLine);
	      unlink("/tmp/ibmeci.conf");
	    }
	}
      aPasswordMustBeChecked = askIfAnotherViavoiceInstall( aCorrectPassword);
    } // while
  return aCorrectPassword;
}

/* > */

/* > */
/* < install */

// return 1: Ok
// return 0: No possible installation
static int install(struct textToSpeechStruct* theTextToSpeech, enum language thePreferredLanguage, int theUserMustBeAskedFor)
{
  int aStatus=1;
  system("modprobe -r dtlk");
  system("rm /dev/dtlk 2>/dev/null");

  switch (theTextToSpeech->myIdentifier)
    {
    case TTS_ViaVoice:
      ManageViaVoiceInstallation( & theTextToSpeech->myIdentifier, & theTextToSpeech->myLanguage, theUserMustBeAskedFor);
      if (theTextToSpeech->myIdentifier != TTS_ViaVoice)
	{ // Ask the synth again, if ViaVoice is not installed
	  aStatus=0;
	}
      else if (thePreferredLanguage == English)
	{
	  theTextToSpeech->myLanguage=English;
	}
      break;

    case TTS_DECtalk:
      ManageDECtalkInstallation( & theTextToSpeech->myIdentifier, & theTextToSpeech->myLanguage);
      if (theTextToSpeech->myIdentifier != TTS_DECtalk)
	{ // Ask the synth again, if DECtalk is not installed
	  aStatus=0;
	}
      else if (thePreferredLanguage == English)
	{
	  theTextToSpeech->myLanguage=English;
	}
      break;

    case TTS_DoubleTalkPC:
      system("modprobe dtlk");
      system("mknod -m 666 /dev/dtlk c 127 0");
      break;
      
    default:
      break;
    }

  return aStatus;
}

/* > */
/* < setTextToSpeech */
int setTextToSpeech(struct textToSpeechStruct* theTextToSpeech,
		    enum language thePreferredLanguage,
		    enum desktopIdentifier theDesktop,
		    int theUserMustBeAskedFor)
{
  struct textToSpeechStruct aTextToSpeech;

  ENTER("setTextToSpeech");
  pf=getTTY();
  int aRequest=1;

  if (!theTextToSpeech)
    {
      return 0;
    }

  memcpy(&aTextToSpeech, theTextToSpeech, sizeof(struct textToSpeechStruct));

  while(aRequest)
    {
      aRequest=0;
      theTextToSpeech->myLanguage = thePreferredLanguage;
      if (theUserMustBeAskedFor)
	{
	  //if (!chooseSynth( theTextToSpeech, thePreferredLanguage, theDesktop))
	    if (!chooseSynth( theTextToSpeech, theTextToSpeech->myLanguage, theDesktop))
	    {
	      return 0;
	    }
	}
 
      //if (!install(theTextToSpeech, thePreferredLanguage, theUserMustBeAskedFor))
	if (!install(theTextToSpeech, theTextToSpeech->myLanguage, theUserMustBeAskedFor))
	{
	  aRequest=1;
	  theUserMustBeAskedFor=1;
	}
    }

  return (0 != memcmp(&aTextToSpeech, theTextToSpeech, sizeof(struct textToSpeechStruct)));
}
/* > */
/* < getStringSynthesis, getEnumSynthesis */

char* getStringSynthesis(enum textToSpeech theValue)
{
  char* aValue="Multispeech";
  int i;
  for (i=0; i<MaxProposedSynth; i++)
    {
      if (myProposedSynth[i].myIdentifier == theValue)
	{
	  aValue = myProposedSynth[i].myLabel;
	  break;
	}
    }
  return aValue;  
}

enum textToSpeech getEnumSynthesis(char* theValue)
{
  int aValue = TTS_Multispeech;
  int i;
  for (i=0; i<MaxProposedSynth; i++)
    {
      if (strcmp(myProposedSynth[i].myLabel, theValue)==0)
	{
	  aValue=myProposedSynth[i].myIdentifier;
	  break;
	}
    }
  return aValue;  
}

/* > */


