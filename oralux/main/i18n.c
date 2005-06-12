// ----------------------------------------------------------------------------
// i18n.c
// $Id: i18n.c,v 1.5 2005/06/12 20:54:01 gcasse Exp $
// $Author: gcasse $
// Description: Internationalization. 
// $Date: 2005/06/12 20:54:01 $ |
// $Revision: 1.5 $ |
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
#include "i18n.h"
#include "constants.h"

static char TheLine[BUFSIZE];

// presently defined as US keyboards in Knoppix
/*     case chineseKeyboardSimplified: */
/*     case chineseKeyboardTraditionnal: */
/*     case hebrewKeyboard: */
/*     case japaneseKeyboard: */
/*     case netherlandKeyboard: */

struct t_keyboard{
  int myID;
  char* myKeytable;
  char* myXKeyboard;
};

typedef struct t_keyboard T_KEYBOARD;

static T_KEYBOARD TheKeyboard[]={
  {americanKeyboard, // default value
   "us",
   "us"},
  {belgianKeyboard,
   "be2-latin1",
   "be"},
  {britishKeyboard,
   "uk",
   "uk"},
  {bulgarianKeyboard,
   "bg",
   "bg"},
  {czechKeyboard,
   "cz-lat2",
   "cs"},
  {danskKeyboard,
   "dk",
   "dk"},
  {finnishKeyboard,
   "fi",
   "fi"},
  {frenchKeyboard,
   "fr-pc",
   "fr"},
  {frenchcanadianKeyboard,
   "cf",
   "cf"},
  {germanKeyboard,
   "de-latin1-nodeadkeys",
   "de"},
  {italianKeyboard,
   "it",
   "it"},
  {polishKeyboard,
   "pl",
   "pl"},
  {russianKeyboard,
   "ru4",
   "ru"},
  {slovakKeyboard,
   "sk-qwerty",
   "sk"},
  {spanishKeyboard,
   "es",
   "es"},
  {swissKeyboard,
   "sg-latin1",
   "de_CH"},
  {turkishKeyboard,
   "tr_q-latin5",
   "tr"},
  };

// Spoken language

struct t_idLabel{
  int myID;
  char* myLabel;
};

typedef struct t_idLabel IDLABEL;

static IDLABEL TheSpokenLanguage[]={
  {English,
   "us"},
  {French,
   "fr"},
  {German,
   "de"},
  {Russian,
   "ru"},
  {Spanish,
   "es"},
  };

// functions

void getStringKeyboard(enum keyboard theValue, char** theKeytable, char** theXKeyboard)
{
  *theKeytable=TheKeyboard[0].myXKeyboard;
  *theXKeyboard=TheKeyboard[0].myXKeyboard;

  int i=0;
  for (i=0;i<sizeof(TheKeyboard)/sizeof(TheKeyboard[0]);i++)
    {
      if (TheKeyboard[i].myID==theValue)
	{
	  *theKeytable=TheKeyboard[i].myKeytable;
	  *theXKeyboard=TheKeyboard[i].myXKeyboard;
	}
    }
}

enum keyboard getEnumKeyboard(char* theKeytable)
{
  int aValue=TheKeyboard[0].myID;

  int i=0;
  for (i=0;i<sizeof(TheKeyboard)/sizeof(TheKeyboard[0]);i++)
    {
      if (strcmp(TheKeyboard[i].myKeytable,theKeytable)==0)
	{
	  aValue=TheKeyboard[i].myID;
	}
    }
  return aValue;
}

char* getStringLanguage(int theValue)
{
  char* aValue="us";
  int i=0;
  for (i=0;i<sizeof(TheSpokenLanguage)/sizeof(TheSpokenLanguage[0]);i++)
    {
      if (TheSpokenLanguage[i].myID==theValue)
	{
	  aValue=TheSpokenLanguage[i].myLabel;
	}
    }
  return aValue;  
}

enum language getEnumLanguage(char* theValue)
{
  int aValue=TheSpokenLanguage[0].myID;
  int i=0;
  for (i=0;i<sizeof(TheSpokenLanguage)/sizeof(TheSpokenLanguage[0]);i++)
    {
      if (strcmp(TheSpokenLanguage[i].myLabel,theValue)==0)
	{
	  aValue=TheSpokenLanguage[i].myID;
	}
    }
  return aValue;  
}

static char* getConsoleFont(enum language theLanguage)
{
  static char* aFont=NULL;
  switch(theLanguage)
    {
    case Russian:
      aFont="/usr/share/consolefonts/koi8-r.psf.gz";
      break;

    default:
      aFont="/usr/share/consolefonts/lat9w-16.psf.gz";
      break;
    }
  return aFont;
}

void setConsoleFont(enum language theLanguage)
{
  static char* aPreviousFont=NULL;
  char* aFont=getConsoleFont(theLanguage);

  if ((aPreviousFont==NULL)
      || (strcmp(aFont, aPreviousFont) != 0))
    {
      sprintf(TheLine, "consolechars -f %s", aFont);
      system(TheLine);
      aPreviousFont=aFont;
    }
}

// buildI18n
// From to the input argument, the following information are known
// enum language myMenuLanguage:
// give the language of the Oralux services (ogg files)
//
// The following files are created under /etc/sysconfig:
// i18n, keyboard, desktop, knoppix
//
void buildI18n( enum language theMenuLanguage, 
		struct textToSpeechStruct theTextToSpeech, 
		enum keyboard theKeyboard,
		struct keyboardStruct theKeyboardFeatures,
		enum desktopIdentifier theDesktop)
{
  char* CHARSET="";
  char* COUNTRY="";
  char* DESKTOP="";
  char* KEYTABLE="";
  char* LANG="";
  char* LANGUAGE="";
  char* ORALUXTTSLANG="";
  char* ORALUXSTICKYKEYS=(theKeyboardFeatures.myStickyKeysAreAvailable) ? "1":"0";
  char* ORALUXREPEATKEYS=(theKeyboardFeatures.myRepeatKeysAreAvailable) ? "1":"0";
  char* XKEYBOARD="";
  char* XMODIFIERS="";
  char* CONSOLEFONT=getConsoleFont(theMenuLanguage);
        
  //  char* TZ="";

  ORALUXTTSLANG=getStringLanguage(theMenuLanguage);
  DESKTOP=desktopGetString(theDesktop);

  getLanguageVariable( theTextToSpeech.myLanguage,
		       &CHARSET, &COUNTRY,
		       &LANG, &LANGUAGE);

  getStringKeyboard(theKeyboard, &KEYTABLE, &XKEYBOARD);

  // File i18n
  char* aFilename="/etc/sysconfig/i18n";
  unlink(aFilename);
  FILE* fd=fopen(aFilename,"w");
  if (!fd)
    {
      printf("Access rights for sysconfig!");
      return;
    }

  fprintf(fd,"CHARSET=\"%s\"\n",CHARSET);
  fprintf(fd,"COUNTRY=\"%s\"\n",COUNTRY);
  fprintf(fd,"LANG=\"%s\"\n",LANG);
  fprintf(fd,"LANGUAGE=\"%s\"\n",LANGUAGE);
  fprintf(fd,"ORALUXTTSLANG=\"%s\"\n",ORALUXTTSLANG);
  fprintf(fd,"XMODIFIERS=\"%s\"\n",XMODIFIERS);
  fprintf(fd,"CONSOLEFONT=\"%s\"\n",CONSOLEFONT);
  fclose(fd);

  // Default Keyboard layout for console and X
  aFilename="/etc/sysconfig/keyboard";
  unlink(aFilename);
  fd=fopen(aFilename,"w");
  fprintf(fd,"KEYTABLE=\"%s\"\n",KEYTABLE);
  fprintf(fd,"XKEYBOARD=\"%s\"\n",XKEYBOARD);
  fclose(fd);

  // Desired desktop
  aFilename="/etc/sysconfig/desktop";
  unlink(aFilename);
  fd=fopen(aFilename,"w");
  fprintf(fd,"DESKTOP=\"%s\"\n",DESKTOP);
  fclose(fd);
	 							  
  // Write all, including non-standard variables, to /etc/sysconfig/knoppix
  aFilename="/etc/sysconfig/knoppix";
  unlink(aFilename);
  fd=fopen(aFilename,"w");
  fprintf(fd,"LANG=\"%s\"\n",LANG);
  fprintf(fd,"COUNTRY=\"%s\"\n",COUNTRY);
  fprintf(fd,"LANGUAGE=\"%s\"\n",LANGUAGE);
  fprintf(fd,"CHARSET=\"%s\"\n",CHARSET);
  fprintf(fd,"KEYTABLE=\"%s\"\n",KEYTABLE);
  fprintf(fd,"XKEYBOARD=\"%s\"\n",XKEYBOARD);
  fprintf(fd,"DESKTOP=\"%s\"\n",DESKTOP);
  //  fprintf(fd,"TZ=\"%s\"\n",TZ);
  fprintf(fd,"ORALUXTTSLANG=\"%s\"\n",ORALUXTTSLANG);
  fprintf(fd,"ORALUXSTICKYKEYS=\"%s\"\n",ORALUXSTICKYKEYS);
  fprintf(fd,"ORALUXREPEATKEYS=\"%s\"\n",ORALUXREPEATKEYS);
  fprintf(fd,"ORALUXRELEASE=\"%s\"\n",ORALUX_RELEASE);
  fprintf(fd,"EMACSPEAKTTS=\"%s\"\n",getStringSynthesis( theTextToSpeech.myIdentifier));
  fprintf(fd,"EMACSPEAKTTSPORT=\"%s\"\n",serialPortGetString( theTextToSpeech.myPort));

  fclose(fd);
}

void getLanguageVariable( enum language theWishedLanguage,
			  char** theCharset, char** theCountry,
			  char** theLang, char** theLanguage)
{
  ENTER("getLanguageVariable");
  enum language aPossibleLanguage=English;

  SHOW2("theWishedLanguage=%d\n", theWishedLanguage);

  switch(theWishedLanguage)
    {
    case French:
      *theCharset="iso8859-15";
      *theCountry="fr";
      *theLang="fr_FR@euro";
      *theLanguage="fr";
      break;

    case German:
      *theCharset="iso8859-15";
      *theCountry="de";
      *theLang="de_DE@euro";
      *theLanguage="de";
      break;

    case Russian:
      *theCharset="koi8-r";
      *theCountry="ru";
      *theLang="ru_RU.KOI8-R";
      *theLanguage="ru";
      //TZ="Europe/Moscow"
      break;

    case Spanish:
      *theCharset="iso8859-15";
      *theCountry="es";
      *theLang="es_ES@euro";
      *theLanguage="es";
      break;

    case English:
    default:
      *theCharset="iso8859-1";
      *theCountry="us";
      *theLang="C";
      *theLanguage="en"; // TBD (en/aspell)
      break;
    }

  SHOW2("LANG=%s\n",*theLang);
  SHOW2("LANGUAGE=%s\n", *theLanguage);
}
