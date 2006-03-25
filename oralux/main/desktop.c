// ----------------------------------------------------------------------------
// desktop.c
// $Id: desktop.c,v 1.4 2006/03/25 22:11:55 gcasse Exp $
// $Author: gcasse $
// Description: Serial ports 
// $Date: 2006/03/25 22:11:55 $ |
// $Revision: 1.4 $ |
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
#include "constants.h"
#include "desktop.h"
#include "audioUserInterface.h"
#include "menu.h"

/* struct audioUserInterfaceCheatCode */
/* { */
/*   enum audioUserInterface myIdentifier; */
/*   char* myCode; */
/* }; */

/* static struct audioUserInterfaceCheatCode myInterfaceCheatCode[]={ */
/*   {Emacspeak, "emacspeak"}, */
/*   {Yasr, "yasr"}, */
/* }; */

struct desktopItem
{
  char* myIdentifier;
  enum sentence myName;
};

static struct desktopItem myDesktops[]=
  {
    {"Emacspeak",sayEmacspeak},
    {"Yasr",sayYasr},
    {"Speakup",saySpeakup},
  };

#define MaxDesktop (sizeof(myDesktops)/sizeof(myDesktops[0]))

enum desktopIdentifier getEnumDesktop(char* theDesktop)
{
  enum desktopIdentifier aDesktop=Emacspeak;
  int i=0;

  if (theDesktop)
    {
      for (i=0; i<sizeof(myDesktops)/sizeof(myDesktops[0]); i++)
	{
	  if (strcmp( myDesktops[i].myIdentifier, theDesktop)==0)
	    {
	      aDesktop=i;
	      break;
	    }
	}
    }
  return aDesktop;
}

char* desktopGetString( enum desktopIdentifier theValue)
{
  char* aLabel=myDesktops[0].myIdentifier;
  if (theValue < MaxDesktop)
    {
      aLabel=myDesktops[theValue].myIdentifier;
    }
  return aLabel;
}

enum sentence desktopGetSentence( enum desktopIdentifier theValue)
{
  enum sentence aLabel=myDesktops[0].myName;
  if (theValue < MaxDesktop)
    {
      aLabel=myDesktops[theValue].myName;
    }
  return aLabel;
}

int setDesktop(enum desktopIdentifier *theDesktop)
{
  ENTER("setDesktop");
  int aDesktopRequest=1;
  int i=0;
  enum desktopIdentifier aDesktop = Emacspeak;

  if (!theDesktop)
    {
      return 0;
    }

  // Looking for the index which matches theDesktop
  for (i=0; i<MaxDesktop; i++)
    {
      if (getEnumDesktop(myDesktops[i].myIdentifier) == *theDesktop)
	{
	  break;
	}
    }

  if (i >= MaxDesktop)
    {
      SHOW("Desktop: Unexpected value");
      return 0;
    }

  say( desktopIs);
  say( myDesktops[i].myName);

  say( changeDesktop);
  say( PleasePressKey);

  if (getAnswer() != MENU_Yes)
    {
      aDesktopRequest=0;
    }
  else
    {
      say( whichDesktop);
    }

  while( aDesktopRequest)
  {
    say( myDesktops[i].myName); // works because the enum values are the same!

    switch(getAnswer())
      {
	case MENU_Yes:
	  aDesktopRequest=0;
	  break;

	case MENU_Previous:
	  i = (i > 0) ? i-1 : MaxDesktop-1;
	  break;

	default:
	  i = (i >= MaxDesktop-1) ? 0 : i+1;
	  break;
      }
  }
  *theDesktop = getEnumDesktop( myDesktops[i].myIdentifier);
  return (*theDesktop != aDesktop);
}



/*   if (aResult && aAudioUserInterface) */
/*     { */
/*       int aMaxCheatCode=sizeof(myInterfaceCheatCode)/sizeof(myInterfaceCheatCode[0]); */
/*       int i=0; */

/*       // Looking for the index in the array */
/*       for (i=0; i<aMaxCheatCode; i++) */
/* 	{ */
/* 	  if (strcmp(aAudioUserInterface, myInterfaceCheatCode[i].myCode)==0) */
/* 	    { */
/* 	      *theAudioUserInterface=myInterfaceCheatCode[i].myIdentifier; */
/* 	      aResult=1; */
/* 	      break; */
/* 	    } */
/* 	} */
/*     } */
