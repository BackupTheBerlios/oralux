// ----------------------------------------------------------------------------
// menu.c
// $Id: menu.c,v 1.14 2006/02/05 00:42:15 gcasse Exp $
// $Author: gcasse $
// Description: introductory menu. 
// $Date: 2006/02/05 00:42:15 $ |
// $Revision: 1.14 $ |
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
#include "textToSpeech.h"
#include "getnchar.h"
#include "menu.h"
#include "braille.h"
#include "keyboard.h"
#include "serialPort.h"

static char TheLine[BUFSIZE];
static int pf=0;

/* < keyPressedCallback */
// When setting the volume, the user presses either the space or the return key.
// If the left arrow key is pressed, the previous sentences might be still said.
// If another key is pressed, the volume is set, and the previous sentence might be cleared.
// When entering the DECtalk serial number, only the return key implies that the sentences are cleared.
//

// keyPressedCallback
// This function is called for any key pressed
// The Enter key means 'Yes'
// Any other key means "No"
// The left and right arrow keys means: repeat last sentences. These are 
// the only keys which do not clear the stored sentences.
//
static void keyPressedCallback(char* theKeyPressed)
{
  ENTER("keyPressedCallback");

  if ((theKeyPressed[0]==0x1b)
      &&(theKeyPressed[1]==0x5b)
      &&((theKeyPressed[2]==0x43) || ((theKeyPressed[2]==0x44)))
      &&(theKeyPressed[3]==0))
    { // Left or right arrow key: say again the stored sentences 
      // Even if the sound is disabled
      sayAgain();
    }
  else if ((theKeyPressed[0]==0x1b)
      &&(theKeyPressed[1]==0x5b)
      &&((theKeyPressed[2]==0x41) || ((theKeyPressed[2]==0x42)))
      &&(theKeyPressed[3]==0))
    { // Up or down arrow key
      sayForce( (theKeyPressed[2]==0x41) ? SayPrevious:SayNext);
    }
  else
    {
      switch (theKeyPressed[0])
	{
	case 0x03:
	case 0x04:
	  stopAUI(0);
	  break;
	case '\n':
	case '\r':
	  sayForce( SayYes);
	  break;
	default:
	  sayForce( SayNo);
	  break;
	}
    }
}

/* > */
/* < keyPressedCallback2 */

// keyPressedCallback2: same than keyPressedCallback
// Useful to set the volume with the return and space keys.
// The sentences are cleared if the key is distinct of a space or a return key.
//
static void keyPressedCallback2(char* theKeyPressed)
{
  ENTER("keyPressedCallback2");

  if ((theKeyPressed[0]==0x1b)
      &&(theKeyPressed[1]==0x5b)
      &&((theKeyPressed[2]==0x43) || ((theKeyPressed[2]==0x44)))
      &&(theKeyPressed[3]==0))
    { // Left or right arrow key: say again the stored sentences 
      // Even if the sound is disabled
      sayAgain();
    }
  else if ((theKeyPressed[0]==0x1b)
      &&(theKeyPressed[1]==0x5b)
      &&((theKeyPressed[2]==0x41) || ((theKeyPressed[2]==0x42)))
      &&(theKeyPressed[3]==0))
    { // Up or down arrow key
      sayForce( (theKeyPressed[2]==0x41) ? SayPrevious:SayNext);
    }
  else
    {
      switch (theKeyPressed[0])
	{
	case 0x03:
	case 0x04:
	  stopAUI(0);
	  break;
	case '\n':
	case '\r':
	case ' ':
	  break;
	default:
	  sayForce( SayYes);
	  break;
	}
    }
}

/* > */
/* < keyPressedCallback3 */

// keyPressedCallback3: same than keyPressedCallback
// The sentences are cleared if the key equals a return key
void keyPressedCallback3(char* theKeyPressed)
{
  ENTER("keyPressedCallback3");

  if ((theKeyPressed[0]==0x1b)
      &&(theKeyPressed[1]==0x5b)
      &&((theKeyPressed[2]==0x43) || ((theKeyPressed[2]==0x44)))
      &&(theKeyPressed[3]==0))
    { // Left or right arrow key: say again the stored sentences 
      // Even if the sound is disabled
      sayAgain();
    }
  else if ((theKeyPressed[0]==0x1b)
      &&(theKeyPressed[1]==0x5b)
      &&((theKeyPressed[2]==0x41) || ((theKeyPressed[2]==0x42)))
      &&(theKeyPressed[3]==0))
    { // Up or down arrow key
      sayForce( (theKeyPressed[2]==0x41) ? SayPrevious:SayNext);
    }
  else
    {
      switch (theKeyPressed[0])
	{
	case 0x03:
	case 0x04:
	  stopAUI(0);
	  break;
	case '\n':
	case '\r':
	  sayForce( SayYes);
	  break;
	default:
	  break;
	}
    }
}

/* > */
/* < getAnswer */

// getAnswer
enum MenuAnswer getAnswer()
{
  ENTER("getAnswer");
  enum MenuAnswer aMenuAnswer=MENU_Yes;

  enum GNC_Answer aStatus=GNC_Ok;
  char* aPressedKey=getnchar(pf, 1, 0, 0, &aStatus, keyPressedCallback);

  switch(aStatus)
    {
    case GNC_UpArrowKey:
      aMenuAnswer = MENU_Previous;
      break;
      
    case GNC_DownArrowKey:
      aMenuAnswer = MENU_Next;
      break;	
      
    case GNC_Ok:
    default:
      if (strcmp(aPressedKey, "\n")==0)
	{
	  aMenuAnswer = MENU_Yes;
	}
      else
	{
	  aMenuAnswer = MENU_No;
	}
      break;
    }
  return (aMenuAnswer);
}

/* > */
/* < isInstalled */

static int isInstalled()
{
  struct stat buf;
  return (0!=lstat("/KNOPPIX/bin/ash", &buf));
}

/* > */
/* < setVolume */

// setVolume
// Selecting the sound volume
// Return KEY: Higher
// Space Key: Lower
// Any other key: quit
//
// Input: default volume
static void setVolume()
{
  ENTER("setVolume");
  int aVolumeToChoose=1;

  say(choosingVolume);
  beepLoudSpeaker();

  enum GNC_Answer aStatus=GNC_Ok;

  while(aVolumeToChoose)
  {
    beep();
    char* a_char=getnchar(pf, 1, 0, 0, &aStatus, keyPressedCallback2);
    switch(a_char[0])
    {
    case '\n':
      system("aumix -v +10 -w +10"); 
      break;
    case ' ':
      system("aumix -v -10 -w +10"); 
      break;
    default:
      system("aumix -S");
      char* anUser=getenv("USER");
      char* aHome=getenv("HOME");
      sprintf(TheLine,"chown %s:%s %s/.aumixrc", anUser, anUser, aHome);
      system(TheLine);
      aVolumeToChoose=0;
      break;
    }
  }
}

/* > */
/* < setMenuLanguage */

static int setMenuLanguage(enum language *theLanguage)
{
  ENTER("setMenuLanguage");
  static int aMenu[] = {
    MenuInEnglish, 
    MenuInFrench,
    MenuInGerman,
    MenuInSpanish,
    MenuInBrazilian,
    MenuInRussian,
  }; // Index= enum language
  int aLanguageRequest=1;
  int aLanguageMax=sizeof(aMenu)/sizeof(aMenu[0]);
  enum language aLanguage=English;
  int aMenuIsModified=0;
  
  if (!theLanguage)
    {
      return aMenuIsModified;
    }

  aLanguage = *theLanguage;

  setLanguage( aLanguage);
  say( aMenu[ aLanguage]);
  say( PleasePressKey);
  int aQuestion=1;
  
  // Is this language correct ?
  if(getAnswer() != MENU_No) 
    {
      aLanguageRequest=0;
    }
  else
    {
      aLanguage = (aLanguage >= aLanguageMax-1) ? 0 : aLanguage+1;
    }

  while(aLanguageRequest)
  {
    setLanguage( aLanguage);
    say( aMenu[ aLanguage]);
    if (aQuestion<aLanguageMax)
      {
	aQuestion++;
	say( PleasePressKey);
      }
    
    switch(getAnswer())
      {
	case MENU_Yes:
	  aLanguageRequest=0;
	  break;

	case MENU_Previous:
	  aLanguage = (aLanguage > 0) ? aLanguage-1 : aLanguageMax-1;
	  break;

	default:
	  aLanguage = (aLanguage >= aLanguageMax-1) ? 0 : aLanguage+1;
	  break;
      }
  }
  
  aMenuIsModified = (aLanguage != *theLanguage);
  *theLanguage = aLanguage;

  return aMenuIsModified;
}

/* > */
/* < setInternet */
void setInternet( struct menuInfo* theSelectedInfo)
{
  say( setUpInternet);
  say( PleasePressKey);

  if (getAnswer() != MENU_Yes)
    {
      return;
    }


  // This menu requires yasr
  buildConfigurationYasr(&(theSelectedInfo->myTextToSpeech));
  
  // Set the expected LANG
  char* aLang=getStringLanguage( theSelectedInfo->myTextToSpeech.myLanguage);

  char* aCommand=TheLine;
  sprintf(aCommand, "%s/main/netConfig.sh", 
	  ORALUX_RUNTIME);
	    
  runYasr( & (theSelectedInfo->myTextToSpeech), 
	   theSelectedInfo->myMenuLanguage, 
	   aCommand);
}
/* > */

/* < setKeyboard */

// TheKeyboards: an array useful to sort the keybord labels in alphabetical order (depends on the current language).
static enum keyboard TheKeyboards[MaxKeyboard];
static enum language TheLanguageForSorting=SynthesisLanguageMax;

int sortKeyboardMessages( const void* theKeyboard_A, const void* theKeyboard_B)
{
  //  ENTER("sortKeyboardMessages");
  char* aMessage_A = getText( *(enum keyboard*)theKeyboard_A);
  char* aMessage_B = getText( *(enum keyboard*)theKeyboard_B);

  if (aMessage_A==NULL)
    {
      aMessage_A=getTextForThisLanguage(*(enum keyboard*)theKeyboard_A, English);
    }
  if (aMessage_B==NULL)
    {
      aMessage_B=getTextForThisLanguage(*(enum keyboard*)theKeyboard_B, English);
    }

  return strcmp( aMessage_A, aMessage_B);
} 

static void loadkeys( enum keyboard theKeyboard)
{
  char* aKeyTable=NULL;
  char* aXKeyboard=NULL;
  getStringKeyboard(theKeyboard, &aKeyTable, &aXKeyboard);

  if (aKeyTable)
    {
      sprintf( TheLine, "loadkeys %s", aKeyTable);
      system( TheLine);
    }
}

static int setKeyboard(enum keyboard *theKeyboard, enum language theCurrentLanguage)
{
  ENTER("setKeyboard");
  int aKeyboardRequest=1;
  int i=0;
  enum keyboard aKeyboard;
  int aKeyboardIsChanged=0;

  if (!theKeyboard)
    {
      return 0;
    }

  aKeyboard = *theKeyboard;

  say( keyboardIs);
  say( aKeyboard);

  say( changeKeyboard);
  say( PleasePressKey);

  if (getAnswer() != MENU_Yes)
    {
      aKeyboardRequest=0;
    }
  else
    {
      say( whichKeyboard);
    }

  // The distinct keyboards are sorting in alphabetical order
  if (TheLanguageForSorting != theCurrentLanguage)
    {
      for (i=0;i<MaxKeyboard;i++)
	{
	  TheKeyboards[i]=i;
	}

      qsort( TheKeyboards, MaxKeyboard, sizeof(enum keyboard), sortKeyboardMessages);

      TheLanguageForSorting = theCurrentLanguage;
    }

  // Looking for the index which matches aKeyboard
  for (i=0; i<MaxKeyboard; i++)
    {
      if (TheKeyboards[i] == aKeyboard)
	{
	  break;
	}
    }

  if (i >= MaxKeyboard)
    {
      SHOW("Keyboard: Unexpected value");
      return 0;
    }

  while( aKeyboardRequest)
  {
    say( TheKeyboards[i]); // works because the enum values are the same!

    switch(getAnswer())
      {
	case MENU_Yes:
	  aKeyboardRequest=0;
	  break;

	case MENU_Previous:
	  i = (i > 0) ? i-1 : MaxKeyboard-1;
	  break;

	default:
	  i = (i >= MaxKeyboard-1) ? 0 : i+1;
	  break;
      }
  }

  aKeyboardIsChanged = (TheKeyboards[i] != *theKeyboard);
  *theKeyboard = TheKeyboards[i];
  return aKeyboardIsChanged;
}

/* > */
/* < setKeyboardFeatures */

static int setKeyboardFeatures(struct keyboardStruct * theKeyboardFeatures)
{
  ENTER("setKeyboardFeatures");
  int aKeyboardRequest=1;
  int aStickyKeyStage=1;
  struct keyboardStruct aOldKeyboardFeatures;

  if (!theKeyboardFeatures)
    {
      return 0;
    }

  memcpy(&aOldKeyboardFeatures, theKeyboardFeatures, sizeof(struct keyboardStruct));

  say( changeKeyboardFeatures);
  say( PleasePressKey);

  if (getAnswer() != MENU_Yes)
    {
      aKeyboardRequest=0;
    }

  while( aKeyboardRequest)
  {
    if (aStickyKeyStage)
      {

	if (theKeyboardFeatures->myStickyKeysAreAvailable)
	  {
	    say( stickyKey);
	  }
	else
	  {
	    say( noStickyKey);
	  }
	
	say( PleasePressKey);
	
	switch(getAnswer())
	  {
	  case MENU_Yes:
	    break;
	    
	  case MENU_No:
	    theKeyboardFeatures->myStickyKeysAreAvailable=(theKeyboardFeatures->myStickyKeysAreAvailable) ? 0:1;
	    break;
	    
	  case MENU_Previous:
	    aKeyboardRequest=0;
	    break;
	    
	  default:
	    break;
	  }
	aStickyKeyStage=0;
      }
    else
      {
	if (theKeyboardFeatures->myRepeatKeysAreAvailable)
	  {
	    say( repeatKey);
	  }
	else
	  {
	    say( noRepeatKey);
	  }
	
	say( PleasePressKey);
	
	aKeyboardRequest=0;
	switch(getAnswer())
	  {
	  case MENU_Yes:
	    break;
	    
	  case MENU_No:
	    theKeyboardFeatures->myRepeatKeysAreAvailable=(theKeyboardFeatures->myRepeatKeysAreAvailable) ? 0:1;
	    break;
	    
	  case MENU_Previous:
	    aKeyboardRequest=1;
	    aStickyKeyStage=1;
	    break;
	    
	  default:
	    break;
	  }
      }
  }
  return (0 != memcmp(&aOldKeyboardFeatures, theKeyboardFeatures, sizeof(struct keyboardStruct)));
}

/* > */
/* < mustSetPreferences */
// mustSetPreferences
// If the preferences were already set, we ask if the menu must be run again.
//
// Return: 1=yes, 0= false
int mustSetPreferences()
{
  ENTER("mustSetPreferences");
  int aStatus=1;

  // Testing if the .aumixrc is present
  sprintf(TheLine,"%s/.aumixrc",getenv("HOME"));
  FILE* fd=fopen(TheLine,"r");
  if (fd!=0)
    {
      // Preferences were already set.
      // Asking if we must enter again the menu
      say(enterAgainMenu);
      say(PleasePressKey);

      if (getAnswer()!=MENU_Yes)
	{
	  aStatus=0;
	}
      fclose(fd);
    }
  return aStatus;
}
/* > */

/* < askIfShutdownIsRequired */

enum ShutdownStatus askIfShutdownIsRequired()
{
  ENTER("askIfShutdownIsRequired");
  enum ShutdownStatus aStatus=StatusUndefined;

  say(DoYouWantToShutdown);
  say(PleasePressKey);
  if (MENU_Yes == getAnswer()) 
    {
      // RAF GC
      /*       say(ToEjectCD); */
      /*       if (MENU_Yes == getAnswer())  */
      /* 	{ */
      if (!isInstalled())
	{
	  sayForce(ThenReturnOnceEjected);
	}
      aStatus=StatusEjectCDROMAndShutdown;
      SHOW("StatusEjectCDROMAndShutdown");
      /* 	} */
      /*       else */
      /* 	{ */
      /* 	  aStatus=StatusKeepCDROMAndShutdown; */
      /* 	  SHOW("StatusKeepCDROMAndShutdown");	      */
      /* 	} */
    }
  else
    {
      say(DoYouWantToReboot);
      if (MENU_Yes == getAnswer()) 
	{
	  if (!isInstalled())
	    {
	      sayForce(ThenReturnOnceEjected);
	    }
	  aStatus=StatusReboot;
	  SHOW("StatusReboot");	     
	}
    }
  clearStoredSentences();
  return aStatus;
}

/* > */

/* < saveconfig */
void saveconfig( struct menuInfo* theSelectedInfo)
{
  if (isInstalled())
    {
      return;
    }
  say( Saysaveconfig);
  say( PleasePressKey);

  if (getAnswer() != MENU_Yes)
    {
      return;
    }

  // This menu requires yasr
  buildConfigurationYasr(&(theSelectedInfo->myTextToSpeech));
  
  char* aCommand=TheLine;
  //  sprintf(aCommand, "/usr/sbin/saveconfig");
  sprintf(aCommand, "/usr/sbin/knoppix-mkimage");
	    
  runYasr( &(theSelectedInfo->myTextToSpeech), 
	   theSelectedInfo->myMenuLanguage, 
	   aCommand);
}
/* > */

/* < menu */
enum MENU_State {
  MENU_Volume,
  MENU_Language,
  MENU_Keyboard,
  MENU_KeyboardFeatures,
  MENU_TTS,
  MENU_Braille,
  MENU_Desktop,
  MENU_Internet,
  MENU_End,
};


void menu(struct menuInfo* theSelectedInfo, int *theConfHasBeenUpdated)
{
  ENTER("menu");
  enum MENU_State aMenuState=MENU_Volume;
  struct menuInfo aOldInfo;
  
  if (!theSelectedInfo || !theConfHasBeenUpdated)
    {
      return;
    }

  memcpy( &aOldInfo, theSelectedInfo, sizeof(struct menuInfo));

  if (mustSetPreferences())
    {
      *theConfHasBeenUpdated=1;
    }
  else
    {
      setTextToSpeech( &(theSelectedInfo->myTextToSpeech), 
		       theSelectedInfo->myMenuLanguage,
		       theSelectedInfo->myDesktop,
		       0);
      aMenuState=MENU_End;
      *theConfHasBeenUpdated=0;
    }

  serialPortInit();

  while(aMenuState != MENU_End)
    {
      switch(aMenuState)
	{
	case MENU_Volume:
	  setVolume();
	  aMenuState = (GNC_UpArrowKey == getLastKeyPressed()) ? MENU_Internet : MENU_Language;
	  break;

	case MENU_Language:
	  if (setMenuLanguage( &theSelectedInfo->myMenuLanguage))
	    {
	      theSelectedInfo->myUserConfIsKnown=1;
	      if (!aOldInfo.myUserConfIsKnown)
		{ /* The default keyboard must match the selected language */
		  theSelectedInfo->myKeyboard = getProbableKeyboard(theSelectedInfo->myMenuLanguage);
		  loadkeys( theSelectedInfo->myKeyboard);
		}

	      buildI18n( theSelectedInfo->myMenuLanguage, 
			 theSelectedInfo->myTextToSpeech, 
			 theSelectedInfo->myKeyboard,
			 theSelectedInfo->myKeyboardFeatures,
			 theSelectedInfo->myDesktop,
			 theSelectedInfo->myUserConfIsKnown);
	    }
	  aMenuState = (GNC_UpArrowKey == getLastKeyPressed()) ? MENU_Volume : MENU_Keyboard;
	  break;

	case MENU_Keyboard:
	  {
	    if(setKeyboard(&theSelectedInfo->myKeyboard, 
			   theSelectedInfo->myMenuLanguage))
	      {
		theSelectedInfo->myUserConfIsKnown=1;

		// The keyboard is available as soon as it is selected.
		loadkeys( theSelectedInfo->myKeyboard);

		buildI18n( theSelectedInfo->myMenuLanguage, 
			   theSelectedInfo->myTextToSpeech, 
			   theSelectedInfo->myKeyboard,
			   theSelectedInfo->myKeyboardFeatures,
			   theSelectedInfo->myDesktop,
			   theSelectedInfo->myUserConfIsKnown);
	      }
	    aMenuState = (GNC_UpArrowKey == getLastKeyPressed()) ? MENU_Language : MENU_KeyboardFeatures;	  
	  }
	  break;

	case MENU_KeyboardFeatures:
	  if (setKeyboardFeatures(& theSelectedInfo->myKeyboardFeatures))
	    {
	      theSelectedInfo->myUserConfIsKnown=1;
	      buildI18n( theSelectedInfo->myMenuLanguage, 
			 theSelectedInfo->myTextToSpeech, 
			 theSelectedInfo->myKeyboard,
			 theSelectedInfo->myKeyboardFeatures,
			 theSelectedInfo->myDesktop,
			 theSelectedInfo->myUserConfIsKnown);
	    }
	  aMenuState = (GNC_UpArrowKey == getLastKeyPressed()) ? MENU_Keyboard : MENU_Braille;	  
	  break;

	case MENU_Braille:
	  {
	    // The braille driver is installed as soon as it is selected.
	    struct brailleInfo aBrailleInfo;
	    getDefaultConf( theSelectedInfo->myKeyboard, & aBrailleInfo );
	    setBraille( &aBrailleInfo);
	    
	    aMenuState = (GNC_UpArrowKey == getLastKeyPressed()) ? MENU_KeyboardFeatures : MENU_Desktop;
	  }
	  break;

	case MENU_Desktop:
	  if (setDesktop( &(theSelectedInfo->myDesktop)))
	    {
	      theSelectedInfo->myUserConfIsKnown=1;	      
	      buildI18n( theSelectedInfo->myMenuLanguage, 
			 theSelectedInfo->myTextToSpeech, 
			 theSelectedInfo->myKeyboard,
			 theSelectedInfo->myKeyboardFeatures,
			 theSelectedInfo->myDesktop,
			 theSelectedInfo->myUserConfIsKnown);
	    }
	  aMenuState = (GNC_UpArrowKey == getLastKeyPressed()) ? MENU_Braille : MENU_TTS;
	  break;

	case MENU_TTS:
	  if (setTextToSpeech( &(theSelectedInfo->myTextToSpeech),
			       theSelectedInfo->myMenuLanguage,
			       theSelectedInfo->myDesktop,
			       1))
	    {
	      theSelectedInfo->myUserConfIsKnown=1;	      
	      buildI18n( theSelectedInfo->myMenuLanguage, 
			 theSelectedInfo->myTextToSpeech, 
			 theSelectedInfo->myKeyboard,
			 theSelectedInfo->myKeyboardFeatures,
			 theSelectedInfo->myDesktop,
			 theSelectedInfo->myUserConfIsKnown);
	    }
	  aMenuState = (GNC_UpArrowKey == getLastKeyPressed()) ? MENU_Desktop : MENU_Internet;
	  break;

	case MENU_Internet:
	  {
	    setInternet( theSelectedInfo);
	    aMenuState = (GNC_UpArrowKey == getLastKeyPressed()) ? MENU_TTS : MENU_End;
	  }
	  break;

	default:
	  aMenuState = MENU_End; // Security
	  break;
	}
    }
  clearStoredSentences();

  if (!*theConfHasBeenUpdated)
    {
      *theConfHasBeenUpdated=(0!=memcmp( &aOldInfo, theSelectedInfo, sizeof(struct menuInfo)));

/* 	((aOldInfo.myMenuLanguage != theSelectedInfo->myMenuLanguage) */
/* 	 || (aOldInfo.myKeyboard != theSelectedInfo->myKeyboard) */
/* 	 || (aOldInfo.myKeyboardFeatures.myStickyKeysAreAvailable != theSelectedInfo->myKeyboardFeatures.myStickyKeysAreAvailable) */
/* 	 || (aOldInfo.myKeyboardFeatures.myRepeatKeysAreAvailable != theSelectedInfo->myKeyboardFeatures.myRepeatKeysAreAvailable) */
/* 	 || (aOldInfo.myTextToSpeech.myIdentifier != theSelectedInfo->myTextToSpeech.myIdentifier) */
/* 	 || (aOldInfo.myTextToSpeech.myLanguage != theSelectedInfo->myTextToSpeech.myLanguage) */
/* 	 || (aOldInfo.myTextToSpeech.myPort != theSelectedInfo->myTextToSpeech.myPort) */
/* 	 || (aOldInfo.myDesktop != theSelectedInfo->myDesktop)); */
    }
}

/* > */
