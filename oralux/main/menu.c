// ----------------------------------------------------------------------------
// menu.c
// $Id: menu.c,v 1.4 2004/11/12 21:46:13 gcasse Exp $
// $Author: gcasse $
// Description: introductory menu. 
// $Date: 2004/11/12 21:46:13 $ |
// $Revision: 1.4 $ |
// Copyright (C) 2003, 2004 Gilles Casse (gcasse@oralux.org)
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
    { // Left or right arrow keys: say again the stored sentences 
      // Even if the sound is disabled
      sayAgain();
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
    { // Left or right arrow keys: say again the stored sentences 
      // Even if the sound is disabled
      sayAgain();
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
    { // Left or right arrow keys: say again the stored sentences 
      // Even if the sound is disabled
      sayAgain();
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

static enum language setMenuLanguage(enum language theDefaultLanguage)
{
  ENTER("setMenuLanguage");
  static char aMenu[]={MenuInEnglish, 
		       MenuInFrench,
		       MenuInGerman,
		       MenuInSpanish,
		       MenuInRussian,
		       }; // Index= enum language
  enum language aLanguage=theDefaultLanguage;
  int aLanguageRequest=1;

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
      aLanguage = (aLanguage >= LanguageMax-1) ? 0 : aLanguage+1;
    }

  while(aLanguageRequest)
  {
    setLanguage( aLanguage);
    say( aMenu[ aLanguage]);
    if (aQuestion<LanguageMax)
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
	  aLanguage = (aLanguage > 0) ? aLanguage-1 : LanguageMax-1;
	  break;

	default:
	  aLanguage = (aLanguage >= LanguageMax-1) ? 0 : aLanguage+1;
	  break;
      }
  }
  return aLanguage;
}

/* > */
/* < setKeyborad, +... */

// TheKeyboards: an array useful to sort the keybord labels in alphabetical order (depends on the current language).
static enum keyboard TheKeyboards[MaxKeyboard];
static enum language TheLanguageForSorting=LanguageMax;

int sortKeyboardMessages( const void* theKeyboard_A, const void* theKeyboard_B)
{
  //  ENTER("sortKeyboardMessages");
  char* aMessage_A = getText( *(enum keyboard*)theKeyboard_A);
  char* aMessage_B = getText( *(enum keyboard*)theKeyboard_B);
  return strcmp( aMessage_A, aMessage_B);
} 

static enum keyboard setKeyboard(enum keyboard theDefaultKeyboard, enum language theCurrentLanguage)
{
  ENTER("setKeyboard");
  int aKeyboardRequest=1;
  int i=0;

  say( keyboardIs);
  say( theDefaultKeyboard);

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

  // Looking for the index which matches theDefaultKeyboard
  for (i=0; i<MaxKeyboard; i++)
    {
      if (TheKeyboards[i] == theDefaultKeyboard)
	{
	  break;
	}
    }

  if (i >= MaxKeyboard)
    {
      SHOW("Keyboard: Unexpected value");
      return theDefaultKeyboard;
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

  char* aKeyTable=NULL;
  char* aXKeyboard=NULL;
  getStringKeyboard(TheKeyboards[i], &aKeyTable, &aXKeyboard);

  if (aKeyTable)
    {
      sprintf( TheLine, "loadkeys %s", aKeyTable);
      system( TheLine);
    }
  return TheKeyboards[i];
}

/* > */
/* < setKeyboardFeatures */

static void setKeyboardFeatures(struct keyboardStruct * theKeyboardFeatures)
{
  ENTER("setKeyboardFeatures");
  int aKeyboardRequest=1;
  int aStickyKeyStage=1;


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
/* < setYasr */

// Start Yasr
void setYasr( enum textToSpeech theTextToSpeech, enum language theMenuLanguage)
{
  // The end of the configuration relies on Yasr.
  // We select a software synthesizer possibly compliant with the user preferences.

  enum textToSpeech aYasrSynthesizer=TTS_Flite;
  switch (theTextToSpeech)
    {
    case TTS_Flite:
    case TTS_DECtalk:
    case TTS_ParleMax:
    case TTS_Multispeech:
      aYasrSynthesizer=theTextToSpeech;
      break;

    default:
      // We select the nearest possible synthesizer according to the language
      switch (theMenuLanguage)
	{
	case French:
	  aYasrSynthesizer=TTS_ParleMax;
	  break;
	case English:
	case German:
	case Spanish:
	default:
	  aYasrSynthesizer=TTS_Flite;
	  break;
	}
      break;
    }

  // synthesizer port parameter
  char* aParam=NULL;
  switch( aYasrSynthesizer)
    {
    case TTS_DECtalk:
      aParam="|/usr/bin/tcl /usr/share/emacs/site-lisp/emacspeak/servers/dtk-soft";
      break;
    case TTS_ParleMax:
      aParam="|/usr/local/bin/maxlect";
      break;
    case TTS_Multispeech:
      aParam="|/usr/local/lib/multispeech/speech_server";
      break;
    case TTS_Flite:
    default:
      aParam="|/usr/bin/eflite";
      break;
    }

  sprintf(TheLine, "yasr -s \"emacspeak server\" -p \"%s\" /usr/share/oralux/main/mailMenu.php", aParam);
  system(TheLine);
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

void menu(struct menuInfo* theSelectedInfo)
{
  ENTER("menu");
  enum MENU_State aMenuState=MENU_Volume;

  if (!mustSetPreferences())
    {
      setTextToSpeech( &(theSelectedInfo->myTextToSpeech), 
		       //theSelectedInfo->myLanguage,
		       theSelectedInfo->myDesktop,
		       0);
      aMenuState=MENU_End;
    }

  serialPortInit();

  while(aMenuState != MENU_End)
    {
      switch(aMenuState)
	{
	case MENU_Volume:
	  setVolume();
	  aMenuState = (GNC_UpArrowKey == getLastKeyPressed()) ? MENU_TTS : MENU_Language;
	  break;

	case MENU_Language:
	  theSelectedInfo->myMenuLanguage = setMenuLanguage(theSelectedInfo->myMenuLanguage);
	  aMenuState = (GNC_UpArrowKey == getLastKeyPressed()) ? MENU_Volume : MENU_Keyboard;
	  break;

	case MENU_Keyboard:
	  theSelectedInfo->myKeyboard = setKeyboard(theSelectedInfo->myKeyboard, theSelectedInfo->myMenuLanguage);
	  // The keyboard is available as soon as it is selected.
	  buildI18n( theSelectedInfo->myMenuLanguage, 
		     theSelectedInfo->myTextToSpeech, 
		     theSelectedInfo->myKeyboard,
		     theSelectedInfo->myKeyboardFeatures,
		     theSelectedInfo->myDesktop);

	  aMenuState = (GNC_UpArrowKey == getLastKeyPressed()) ? MENU_Language : MENU_KeyboardFeatures;	  
	  break;

	case MENU_KeyboardFeatures:
	  setKeyboardFeatures(& theSelectedInfo->myKeyboardFeatures);
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
	  setDesktop( &(theSelectedInfo->myDesktop));
	  aMenuState = (GNC_UpArrowKey == getLastKeyPressed()) ? MENU_Braille : MENU_TTS;
	  break;

	case MENU_TTS:
	  setTextToSpeech( &(theSelectedInfo->myTextToSpeech),
			   //theSelectedInfo->myMenuLanguage,
			   theSelectedInfo->myDesktop,
			   1);
	  aMenuState = (GNC_UpArrowKey == getLastKeyPressed()) ? MENU_Desktop : MENU_Internet;
	  break;

	case MENU_Internet:
	  setYasr( theSelectedInfo->myTextToSpeech.myIdentifier, theSelectedInfo->myMenuLanguage);
	  aMenuState = (GNC_UpArrowKey == getLastKeyPressed()) ? MENU_TTS : MENU_End;
	  break;

	default:
	  aMenuState = MENU_End; // Security
	  break;
	}
    }
  clearStoredSentences();
}

/* > */
