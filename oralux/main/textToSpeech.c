// ----------------------------------------------------------------------------
// textToSpeech.c
// $Id: textToSpeech.c,v 1.7 2006/01/01 21:24:09 gcasse Exp $
// $Author: gcasse $
// Description: Ask about the whished TTS and install it. 
// $Date: 2006/01/01 21:24:09 $ |
// $Revision: 1.7 $ |
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

/* {{{ External synth */

struct textToSpeechItem
{
  enum textToSpeech myIdentifier;
  enum sentence myName;
};

static struct textToSpeechItem myExternalSynth[]={
  {TTS_AccentSA, AccentSA},
  {TTS_BrailleLite, BrailleLiteTTS},
  {TTS_BrailleNSpeak, BrailleNSpeak},
  {TTS_Ciber232, Ciber232},
  {TTS_Ciber232Plus,Ciber232Plus},
  {TTS_DECtalkExpress,DECtalkExpress},
  {TTS_DECtalkMultivoice,DECtalkMultivoice},
  {TTS_DECtalk3,DECtalk3},
  {TTS_DolphinApollo,DolphinApollo},
  {TTS_DoubleTalkLT,DoubleTalkLT},
  {TTS_DoubleTalkPC,DoubleTalkPC},
  {TTS_LiteTalk,LiteTalk},
  {TTS_PcHabladoNotebook,PcHabladoNotebook},
  {TTS_TypeNSpeak,TypeNSpeak},
};

struct TextToSpeechCheatCode
{
  enum textToSpeech myIdentifier;
  char* myCode;
};

static struct TextToSpeechCheatCode mySynthesizerCheatCode[]={
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


static void chooseExternalSynth( enum textToSpeech* theIdentifier)
{
  ENTER("chooseExternalSynth");
  int aRequest=0;
  int aChoice=0;
  int aQuestion=1;
  int aMaxExternalSynth=sizeof(myExternalSynth)/sizeof(myExternalSynth[0]);
  int i=0;

  // Looking for the index in the array
  for (i=0; i<aMaxExternalSynth; i++)
    {
      if (*theIdentifier==myExternalSynth[i].myIdentifier)
      {
	aChoice=i;
	break;
      }
    }

  say(externalSynthIs);
  say(myExternalSynth[aChoice].myName);
  say(changeExternalSynth);
  say(PleasePressKey);

  if (MENU_Yes == getAnswer())
    {
      aRequest=1;
      say(whichExternalSynth);
    }

  while(aRequest)
    {
      say(myExternalSynth[aChoice].myName);

      if (aQuestion)
	{
	  say( PleasePressKey);
	  aQuestion=0;
	}

      switch(getAnswer())
	{
	case MENU_Yes:
	  aRequest=0;
	  break;
	    
	case MENU_Previous:
	  aChoice = (aChoice > 0) ? --aChoice : aMaxExternalSynth-1;
	  break;

	default:
	  aChoice = (aChoice >= aMaxExternalSynth-1) ? 0 : ++aChoice;
	  break;
	}
    }

  *theIdentifier=myExternalSynth[aChoice].myIdentifier;
}

enum MENU_ExtSynthState {
  MENU_ExtSynth,
  MENU_Port,
  MENU_End,
};

static void setExternalSynth( struct textToSpeechStruct * theTextToSpeech)
{
  ENTER("setExternalSynth");

  enum MENU_ExtSynthState aMenuState=MENU_ExtSynth;
  while(aMenuState != MENU_End)
    {
      switch(aMenuState)
	{
	case MENU_ExtSynth:
	  chooseExternalSynth( & theTextToSpeech->myIdentifier);
	  aMenuState = (GNC_UpArrowKey == getLastKeyPressed()) ? MENU_End : MENU_Port;
	  break;
	  
	case MENU_Port:
	  serialPortChoose( & theTextToSpeech->myPort);
	  aMenuState = (GNC_UpArrowKey == getLastKeyPressed()) ? MENU_ExtSynth : MENU_End;
	  break;
	  
	default:
	  aMenuState = MENU_End;
	  break;
	}
    }
}


// Read the oralux.conf file to see if an external synth must be used instead of the sound card.
//
int HasExternalTextToSpeech(struct textToSpeechStruct* theExternalTextToSpeech)
{
  ENTER("HasExternalTextToSpeech");
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
      int aMaxCheatCode=sizeof(mySynthesizerCheatCode)/sizeof(mySynthesizerCheatCode[0]);
      int i=0;

      // Looking for the index in the array
      for (i=0; i<aMaxCheatCode; i++)
	{
	  if (strcmp(aTextToSpeech, mySynthesizerCheatCode[i].myCode)==0)
	    {
	      theExternalTextToSpeech->myIdentifier=mySynthesizerCheatCode[i].myIdentifier;
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

/* }}} */

/* {{{ Choose a synth (external or not) */

// chooseSynt
// Asking to the user which voice synthesis is required
// Return true if a synth is selected
// false if  menu must exit
//
struct synthItem
{
  enum textToSpeech myIdentifier;
  enum sentence mySentence;
};

static struct synthItem TheProposedSynth[]= { 
  {TTS_ViaVoice, DoYouWantViaVoice}, // index=0
  {TTS_Multispeech, DoYouWantMultispeech},
  {TTS_Flite, DoYouWantFlite},
  {TTS_Cicero, DoYouWantCicero},
  {TTS_DECtalk, DoYouWantToInstallDECtalk},
  {TTS_Undefined, selectExternalSynth}, //last item
};

typedef struct synthItem synthItem;

int getIndexInArrayFromIdentifier( synthItem** theArray, int theMaxIndex, enum textToSpeech theIdentifier)
{
  int aIndex=0;
  int i=0;
  for (i=0; i<theMaxIndex; i++)
    {
      if ((*theArray)[i].myIdentifier == theIdentifier)
	{
	  aIndex=i;
	  break;
	}
    }
  return aIndex;
}

static int chooseSynt( struct textToSpeechStruct* theTextToSpeech,
		       enum language thePreferredLanguage,
		       enum desktopIdentifier theDesktop)
{
  ENTER("chooseSynt");
  int aStatus=1;
  int aRequest=1;
  int aMaxTextToSpeech=0;
  int aIndex=0;
  int i=0;
  int j=0;
  int isFirstRequest=1;
  int aMaxSynth=sizeof(TheProposedSynth)/sizeof(TheProposedSynth[0]);

  synthItem** aArrayOfAvailableSynth=(synthItem**)malloc( aMaxSynth * sizeof(synthItem*));

  // ViaVoice is considered present if it was already installed by OG
  // or if OG is in use
  // two conditions for security (hard disk install)
  struct stat buf;
  int isViaVoicePresent = ((stat("/usr/lib/ibmeci", &buf)==0)
			   || (stat("/usr/share/oraluxGold", &buf)==0));

  for( i=0; i<aMaxSynth; i++)
    {
      if ((TheProposedSynth[i].myIdentifier == TTS_ViaVoice)
	  && !isViaVoicePresent)
	{
	  continue;
	}
      else if ((theDesktop != Emacspeak)
	       && (TheProposedSynth[i].myIdentifier == TTS_Undefined))
	{
	  continue;
	}
      aArrayOfAvailableSynth[ aMaxTextToSpeech++] = TheProposedSynth+i;
    }

  switch(theDesktop)
    {
    case Emacspeak:
      {
	enum textToSpeech aIdentifier=isExternalSynth(theTextToSpeech->myIdentifier) ? TTS_Undefined:theTextToSpeech->myIdentifier;

	aIndex=getIndexInArrayFromIdentifier(aArrayOfAvailableSynth , aMaxTextToSpeech, aIdentifier);
      }
      break;

    case Yasr:
    default:
      {
	// hardware synth are not yet available using Yasr
	enum textToSpeech aIdentifier=isExternalSynth(theTextToSpeech->myIdentifier) ? TTS_Multispeech:theTextToSpeech->myIdentifier;

	aIndex=getIndexInArrayFromIdentifier(aArrayOfAvailableSynth , aMaxTextToSpeech, aIdentifier);
      }
      break;
    }

  while(aRequest)
    {
      say(aArrayOfAvailableSynth[aIndex]->mySentence);

      if (isFirstRequest)
	{
	  say(PleasePressKey);
	}

      switch(getAnswer())
	{
	case MENU_Yes:
	  if (aArrayOfAvailableSynth[aIndex]->mySentence==selectExternalSynth)
	    {
	      setExternalSynth( theTextToSpeech);
	    }
	  else
	    {
	      theTextToSpeech->myIdentifier = aArrayOfAvailableSynth[aIndex]->myIdentifier;
	      switch (theTextToSpeech->myIdentifier)
	 	{
		case TTS_Multispeech:
		case TTS_ViaVoice:
		  theTextToSpeech->myLanguage = thePreferredLanguage;
		  break;

		case TTS_Flite:
		  theTextToSpeech->myLanguage = English;
		  break;

		case TTS_Cicero:
		  theTextToSpeech->myLanguage = French;
		  break;

		default:
		  break;
		}
	    }
	  aRequest=0;
	  break;

	case MENU_Previous:
	  if (isFirstRequest)
	    {
	      aRequest=0;
	      aStatus=0; // Exit
	    }
	  else
	    {
	      aIndex = (aIndex > 0) ? --aIndex : aMaxTextToSpeech-1;
	    }
	  break;
	  
	case MENU_Next:
	  if (isFirstRequest)
	    {
	      aRequest=0;
	    }
	  else
	    {
	      aIndex = (aIndex >= aMaxTextToSpeech-1) ? 0 : ++aIndex;
	    }
	  break;

	case MENU_No:
	default:
	  aIndex = (aIndex >= aMaxTextToSpeech-1) ? 0 : ++aIndex;
	  break;
	}

      isFirstRequest=0;
      SHOW3("aRequest=%d, aIndex=%d\n",aRequest, aIndex);
    }

  free(aArrayOfAvailableSynth);

  return aStatus;
}

/* }}} */

/* {{{ DECtalk installation */

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

  // The dtk partition is remount in read only access
  if (0!=strcmp("/",aDtkPath)) // To avoid any problem with a hard disk installation
    {
      sprintf(TheLine, "mount -o remount,ro %s 1>/dev/null", aDtkPath);
      SHOW(TheLine);
      system(TheLine);
    }

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


// installDECtalk
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
      sprintf(TheLine,"replace -s 'ltts_us' 'ltts_us -l%s' -- %s/Makefile  1>/dev/null", theDECtalkLibrary[ *theTextToSpeechLanguage ], DTK_EMACSPEAK);
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

/* }}} */
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

/* <*/

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

/* {{{ Installing the required text to speech */

// install
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

// setTextToSpeech
void setTextToSpeech(struct textToSpeechStruct* theTextToSpeech,
		     enum language thePreferredLanguage,
		     enum desktopIdentifier theDesktop,
		     int theUserMustBeAskedFor)
{
  ENTER("setTextToSpeech");
  pf=getTTY();
  int aRequest=1;

  while(aRequest)
    {
      aRequest=0;
      theTextToSpeech->myLanguage = thePreferredLanguage;
      if (theUserMustBeAskedFor)
	{
	  //if (!chooseSynt( theTextToSpeech, thePreferredLanguage, theDesktop))
	    if (!chooseSynt( theTextToSpeech, theTextToSpeech->myLanguage, theDesktop))
	    {
	      return;
	    }
	}
 
      //if (!install(theTextToSpeech, thePreferredLanguage, theUserMustBeAskedFor))
	if (!install(theTextToSpeech, theTextToSpeech->myLanguage, theUserMustBeAskedFor))
	{
	  aRequest=1;
	  theUserMustBeAskedFor=1;
	}
    }
}

int isExternalSynth( enum textToSpeech theIdentifier)
{
  int aResult=0;
  int i=0;

  for (i=0; i<sizeof(myExternalSynth)/sizeof(myExternalSynth[0]); i++)
    {
      if (myExternalSynth[i].myIdentifier==theIdentifier)
	{
	  aResult=1;
	}
    }
  return aResult;
}

/* }}} */

/* {{{ Converting identifier to string */

typedef struct t_idLabel IDLABEL;
struct t_Label{
  enum textToSpeech myID;
  char* myLabel;
};

static struct t_Label myLabels[]={
  {TTS_Flite, "Flite"},
  {TTS_DECtalk, "DECtalk"},
  {TTS_Multispeech, "Multispeech"},
  {TTS_Cicero, "Cicero"},
  {TTS_ViaVoice, "ViaVoice"},
  {TTS_AccentSA,"AccentSA"},
  {TTS_BrailleLite,"BrailleLite"},
  {TTS_BrailleNSpeak,"BrailleNSpeak"},
  {TTS_Ciber232,"Ciber232"},
  {TTS_Ciber232Plus,"Ciber232Plus"},
  {TTS_DECtalkExpress,"DECtalkExpress"},
  {TTS_DECtalkMultivoice,"DECtalkMultivoice"},
  {TTS_DECtalk3,"DECtalk3"},
  {TTS_DolphinApollo,"DolphinApollo"},
  {TTS_DoubleTalkLT,"DoubleTalkLT"},
  {TTS_DoubleTalkPC,"DoubleTalkPC"},
  {TTS_LiteTalk,"LiteTalk"},
  {TTS_PcHabladoNotebook,"PcHabladoNotebook"},
  {TTS_TypeNSpeak,"TypeNSpeak"},
  };

char* getStringSynthesis(enum textToSpeech theValue)
{
  char* aValue="Multispeech";
  int i=0;
  for (i=0;i<sizeof(myLabels)/sizeof(myLabels[0]);i++)
    {
      if (myLabels[i].myID==theValue)
	{
	  aValue=myLabels[i].myLabel;
	  break;
	}
    }
  return aValue;  
}

enum textToSpeech getEnumSynthesis(char* theValue)
{
  int aValue=myLabels[0].myID;
  int i=0;
  for (i=0;i<sizeof(myLabels)/sizeof(myLabels[0]);i++)
    {
      if (strcmp(myLabels[i].myLabel,theValue)==0)
	{
	  aValue=myLabels[i].myID;
	  break;
	}
    }
  return aValue;  
}

/* }}} */

