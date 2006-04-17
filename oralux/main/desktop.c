// ----------------------------------------------------------------------------
// desktop.c
// $Id: desktop.c,v 1.6 2006/04/17 09:11:42 gcasse Exp $
// $Author: gcasse $
// Description: Serial ports 
// $Date: 2006/04/17 09:11:42 $ |
// $Revision: 1.6 $ |
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

struct desktopItem
{
  char* myLabel;
  enum sentence myName;
  enum desktopIdentifier myIdentifier;
};

static struct desktopItem myDesktops[]=
  {
    {"Emacspeak",sayEmacspeak,Emacspeak},
    {"Yasr",sayYasr,Yasr},
    {"Speakup",saySpeakup,Speakup},
  };

#define MaxDesktop (sizeof(myDesktops)/sizeof(myDesktops[0]))

enum desktopIdentifier getEnumDesktop(char* theDesktop)
{
  enum desktopIdentifier aDesktop=Emacspeak;
  int i=0;

  if (theDesktop)
    {
      for (i=0; i<MaxDesktop; i++)
	{
	  if (strcmp( myDesktops[i].myLabel, theDesktop)==0)
	    {
	      aDesktop=myDesktops[i].myIdentifier;
	      break;
	    }
	}
    }
  return aDesktop;
}

char* desktopGetString( enum desktopIdentifier theValue)
{
  char* aLabel=myDesktops[0].myLabel;
  int i;

  for (i=0; i<MaxDesktop; i++)
    {
      if (myDesktops[i].myIdentifier == theValue)
	{
	  aLabel=myDesktops[i].myLabel;
	  break;
	}
    }

  return aLabel;
}

enum sentence desktopGetSentence( enum desktopIdentifier theValue)
{
  enum sentence aName=myDesktops[0].myName;
  int i;

  for (i=0; i<MaxDesktop; i++)
    {
      if (myDesktops[i].myIdentifier == theValue)
	{
	  aName=myDesktops[i].myName;
	  break;
	}
    }

  return aName;
}

int setDesktop(enum desktopIdentifier *theDesktop)
{
  ENTER("setDesktop");
  int aDesktopRequest=1;
  int i=0;
  enum desktopIdentifier aDesktop = *theDesktop;

  SHOW2("theDesktop=%d\n",*theDesktop);

  if (!theDesktop)
    {
      return 0;
    }

  // Looking for the index which matches theDesktop
  for (i=0; i<MaxDesktop; i++)
    {
      if (myDesktops[i].myIdentifier == *theDesktop)
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
  *theDesktop = getEnumDesktop( myDesktops[i].myLabel);
  SHOW2("NEW: theDesktop=%d\n",*theDesktop);
  return (*theDesktop != aDesktop);
}

