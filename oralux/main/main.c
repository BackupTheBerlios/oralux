// ----------------------------------------------------------------------------
// main.c
// $Id: main.c,v 1.7 2005/04/03 00:36:28 gcasse Exp $
// $Author: gcasse $
// Description: entry point. 
// $Date: 2005/04/03 00:36:28 $ |
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
#include <stdlib.h>

#include "audioUserInterface.h"
#include "constants.h"
#include "getnchar.h"
#include "menu.h"
#include "i18n.h"

static char TheLine[BUFSIZE];

enum OraluxState {
  ORALUX_Start,
  ORALUX_Stop,
  ORALUX_Reconfigure,
};

// buildConfiguration
// Creating the oralux configuration file: environment variables useful for emacspeak.
// In the future, the parameters could be distinct according to the tty.
// Useful (?) e.g. to start tty1 and tty2 with two distinct Text To Speech softwares.

void buildConfigurationEmacspeak(struct textToSpeechStruct* theTextToSpeech)
{
  ENTER("buildConfigurationEmacspeak");
  char* aFilename="/etc/emacspeak.conf";
  unlink(aFilename);
  sprintf(TheLine, "cp %s/emacspeak.conf %s", ORALUX_RUNTIME_EMACSPEAK, aFilename);
  SHOW(TheLine);
  system(TheLine);

  // Customizing it
  FILE* fd=fopen(aFilename, "a");
  if (!fd)
    {
      return;
    }

  char* aTTS=getStringSynthesis(theTextToSpeech->myIdentifier);
  char* aPort=serialPortGetString(theTextToSpeech->myPort);

  if (theTextToSpeech->myIdentifier==TTS_EFM)
    {
      // if EFM is selected, the user still has the choice to start the original Emacspeak (with Flite).
      aTTS=getStringSynthesis(TTS_Flite);
    }
  fprintf(fd, "\nTTS=%s", aTTS);
  fprintf(fd, "\nPORT=%s", aPort);
  fclose(fd);

  // The final lines come from emacspeak.conf2
  sprintf(TheLine, "cat %s/emacspeak.conf2 >> %s", ORALUX_RUNTIME_EMACSPEAK, aFilename);
  SHOW(TheLine);
  system(TheLine);
}

void buildConfiguration(struct menuInfo* theSelectedInfo)
{
  ENTER("buildConfiguration");

  buildConfigurationEmacspeak(&(theSelectedInfo->myTextToSpeech));

  // RAF: cleaner
  // The yasr configuration already built by the menu
  ////  buildConfigurationYasr(&(theSelectedInfo->myTextToSpeech));

  buildI18n( theSelectedInfo->myMenuLanguage, 
	     theSelectedInfo->myTextToSpeech, 
	     theSelectedInfo->myKeyboard,
	     theSelectedInfo->myKeyboardFeatures,
	     theSelectedInfo->myDesktop);
}


char* getConf(char* theParameter, char* theFilename)
{
  ENTER("getConf");
  char* aPointer=NULL;
  char* aWord=NULL;
  static char TheGetConf[BUFSIZE];

  FILE* fd=fopen(theFilename,"r");
  if (fd==NULL)
    {
      return;
    }

  while(fgets(TheGetConf, BUFSIZE, fd))
    {
      char* aPointer=strstr(TheGetConf,"=");
      int aLength=strlen(TheGetConf);
      if (aPointer==0)
	{	  
	  continue;
	}
      *aPointer=0;
      if ((strcmp(TheGetConf,theParameter)==0) 
	  && (aLength>strlen(TheGetConf)+1))
	{
	  aWord=aPointer+1;
	  break;
	}
    }
  if ((aWord!=NULL) && (strlen(aWord)>=3))
    {
      // removing braquets
      aWord=strstr(aWord,"\"");
      if (aWord!=NULL)
	{
	  char* aPointer=strstr(++aWord,"\"");
	  if (aPointer!=NULL)
	    {
	      *aPointer=0;
	    }
	  else
	    {
	      aWord=NULL;
	    }
	}
    }
  fclose(fd);
  return aWord;
}


// main
// Entry point
int main(int argc, char *argv[])
{
  ENTER("main");

  if (argc!=3)
    {
      fprintf(stderr, "oralux current_tty state\n");
      exit(1);
    }

  char *portname = argv[1];

  enum OraluxState aState=ORALUX_Start;
  if (strcmp(argv[2],"start") == 0)
    {
      aState=ORALUX_Start;
    }
  else if (strcmp(argv[2],"redo") == 0)
    {
      aState=ORALUX_Reconfigure;
    }
  else if (strcmp(argv[2],"stop") == 0)
    {
      aState=ORALUX_Stop;
    }
  else
    {
      fprintf(stderr, "Wrong state: %s\n", argv[2]);
      exit(1);
    }
  /*  setTTY(pf); */

  // Initializing the auditory interface
  getcwd(TheLine, BUFSIZE);
  strcat(TheLine,"/../audio");

  char* aStringMenuLanguage=getConf("ORALUXTTSLANG", "/etc/sysconfig/knoppix");
  enum language aEnumMenuLanguage=English;
  if (aStringMenuLanguage!=NULL)
    {
      aEnumMenuLanguage=getEnumLanguage(aStringMenuLanguage);
    }

  // Which is the default keyboard
  char* aStringKeytable=getConf("KEYTABLE", "/etc/sysconfig/knoppix");
  enum keyboard aEnumKeytable=americanKeyboard;
  if (aStringKeytable!=NULL)
    {
      aEnumKeytable=getEnumKeyboard(aStringKeytable);
    }

  // Is the sticky keys feature selected?
  char* aStringStickyKey = getConf("ORALUXSTICKYKEYS", "/etc/sysconfig/knoppix");
  if (aStringStickyKey==NULL)
    {
      aStringStickyKey="0";
    }
			   
  int aStickyKey=(strcmp(aStringStickyKey, "1") == 0) ? 1: 0;

  // Is the repeat keys feature selected?
  char* aStringRepeatKey = getConf("ORALUXREPEATKEYS", "/etc/sysconfig/knoppix");
  if (aStringRepeatKey==NULL)
    {
      aStringRepeatKey="1";
    }

  int aRepeatKey=(strcmp(aStringRepeatKey, "1") == 0) ? 1: 0;

  // Which is the default synthesis for emacspeak
  char* aStringSynthesis=getConf("EMACSPEAKTTS", "/etc/sysconfig/knoppix");
  enum textToSpeech aEnumSynthesis=TTS_Flite;

#ifdef ORALUXGOLD
  aEnumSynthesis=TTS_ViaVoice;
#endif

  if (aStringSynthesis!=NULL)
    {
      aEnumSynthesis=getEnumSynthesis(aStringSynthesis);
    }

  // Port for an external synth
  char* aStringPort=getConf("EMACSPEAKTTSPORT", "/etc/sysconfig/knoppix");
  enum serialPortIdentifier aEnumPort=TTYS0;
  if (aStringPort!=NULL)
    {
      aEnumPort=getEnumSerialPort(aStringPort);
    }

  // Which is the current LANGUAGE
  char* aStringLanguage=getConf("LANGUAGE", "/etc/sysconfig/knoppix");
  enum language aEnumLanguage=English;
  if (aStringLanguage!=NULL)
    {
      aEnumLanguage=getEnumLanguage(aStringLanguage);
    }

  // Which is the current Desktop
  char* aStringDesktop=getConf("DESKTOP", "/etc/sysconfig/knoppix");
  enum desktopIdentifier aEnumDesktop=getEnumDesktop(aStringDesktop);

  struct menuInfo aSelectedInfo;

  aSelectedInfo.myMenuLanguage=aEnumMenuLanguage;
  aSelectedInfo.myKeyboard=aEnumKeytable;

  aSelectedInfo.myKeyboardFeatures.myStickyKeysAreAvailable=aStickyKey;
  aSelectedInfo.myKeyboardFeatures.myRepeatKeysAreAvailable=aRepeatKey;

  aSelectedInfo.myTextToSpeech.myIdentifier=aEnumSynthesis;
  aSelectedInfo.myTextToSpeech.myLanguage=aEnumLanguage;
  aSelectedInfo.myTextToSpeech.myPort=aEnumPort;
  aSelectedInfo.myDesktop=aEnumDesktop;

  // If an external synthesizer is forced, the sound card is not used (useful when the auto detection failed)

  struct textToSpeechStruct aExternalTextToSpeech;
  int aConfHasBeenUpdated=0;

  int aExternalSynthesizerIsForced=HasExternalTextToSpeech( & (aSelectedInfo.myTextToSpeech));

  if (!aExternalSynthesizerIsForced)
    {
      switch (aState)
	{
	case ORALUX_Start:

	  // Default volume
	  system("aumix -L 1>/dev/null");
	  initAUI(TheLine, aEnumMenuLanguage, portname);

	  // Running the introductory menu
      
	  // Sing Cocoralux, sing !
	  playOGG("cocoralux2.ogg");
	  
	  // Creating the new emacspeak tree
	  sprintf(TheLine,"mkdir -p %s", DTK_EMACSPEAK);
	  SHOW(TheLine);
	  system(TheLine);
	  break;
	  
	case ORALUX_Stop:
	  {
	    initAUI(TheLine, aEnumMenuLanguage, portname);

	    saveconfig( &aSelectedInfo);

	    enum ShutdownStatus aStatus=askIfShutdownIsRequired();
	    stopAUI(1);

	    switch (aStatus)
	      {
	      case StatusReboot:
		system("rm -f /tmp/oralux.tmp;reboot");
		break;
	      
	      case StatusEjectCDROMAndShutdown:
		// knoppix-halt ejects the CD if oralux.tmp is absent
		system("rm -f /tmp/oralux.tmp; halt");
		break;

	      case StatusKeepCDROMAndShutdown:
		system("halt");
		break;

	      default:
	      case StatusUndefined:
		SHOW("StatusUndefined");
		break;
	      }
	    return 0;
	  }
	default:
	  break;
	}
	menu( &aSelectedInfo, &aConfHasBeenUpdated);
    }
  else
    {
      // RAF GC: the following lines will be updated once the tcl drivers will be linked
      switch (aState)
	{
	case ORALUX_Start:
	  if (aSelectedInfo.myTextToSpeech.myIdentifier == TTS_DoubleTalkPC)
	    {
	      system("modprobe dtlk");
	      system("mknod -m 666 /dev/dtlk c 127 0");
	    }
	  break;
	  
	case ORALUX_Stop:
	default:
	    system("halt");
	    break;
	}
    }

  if (aConfHasBeenUpdated)
    {
      // Building the new configuration files
      buildConfiguration(&aSelectedInfo);
    }

  if (!aExternalSynthesizerIsForced)
    {
      if (aState == ORALUX_Start)
	{
	  // Welcome to Oralux + release
	  printf("\n");
#ifdef ORALUXGOLD
	  sayForce(WelcomeToOraluxGold);
#else
	  sayForce(WelcomeToOralux);
#endif
	  sayForce(oraluxRelease);
	}
      stopAUI(1);

      // TBD: desktop shell 
      if (aSelectedInfo.myTextToSpeech.myIdentifier==TTS_Flite)
	{ // This is a workaround which avoids the "Process speaker not running" message in emacspeak
	  system("eflite -f eflite_test.txt");
	}
    }
}
