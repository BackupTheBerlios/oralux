// ----------------------------------------------------------------------------
// serialPort.c
// $Id: serialPort.c,v 1.3 2005/03/31 09:16:54 gcasse Exp $
// $Author: gcasse $
// Description: Serial ports 
// $Date: 2005/03/31 09:16:54 $ |
// $Revision: 1.3 $ |
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
#include <string.h>
#include "constants.h"
#include "serialPort.h"
#include "audioUserInterface.h"
#include "menu.h"

#define SP_PRESENT 1
#define SP_RESERVED 2
static int ttyS[MaxSerialPort];

struct serialPortItem 
{
  char* myIdentifier;
  enum sentence myName;
};

static struct serialPortItem myPorts[]=
  {
    {"/dev/ttyS0",FirstSerialPort},
    {"/dev/ttyS1",SecondSerialPort},
  };

void serialPortInit()
{
  ENTER("serialPortInit");
  char* aBuffer=malloc(BUFSIZE);
  FILE* fd=fopen("/proc/tty/driver/serial","r");
  int i=0;

  for (i=0; i<MaxSerialPort; i++)
    {
      ttyS[i]=0;
    }

  while(fgets(aBuffer, 1024, fd))
  {
    unsigned int aPort=*aBuffer-'0';
    if (aPort < MaxSerialPort)
      {
	ttyS[ aPort]=SP_PRESENT;
      }
  }

  fclose(fd);
  free(aBuffer);
}

static void serialPortSetReserved(int thePort)
{
  ENTER("serialPortSetReserved");
  if (thePort<MaxSerialPort)
    {
      ttyS[thePort]=SP_RESERVED;
    }
}

int serialPortIsAvailable(int thePort)
{
  ENTER("serialPortIsAvailable");
  return ((thePort & SP_PRESENT) && !(thePort & SP_RESERVED));
}

void serialPortChoose( enum serialPortIdentifier * thePort)
{
  ENTER("serialPortChoose");
  int aRequest=0;
  int aChoice=*thePort;
  int aQuestion=1;

  if (aChoice >= sizeof(myPorts)/sizeof(myPorts[0]))
    {
      aChoice=0;
    }

  say(portIs);

  say(myPorts[aChoice].myName);
  say(changePort);
  say(PleasePressKey);

  if (MENU_Yes == getAnswer())
    {
      aRequest=1;
      say(whichPort);
    }

  while(aRequest)
    {
      say(myPorts[aChoice].myName);

      if (aQuestion)
	{
	  say( PleasePressKey);
	  aQuestion=0;
	}

      switch(getAnswer())
	{
	case MENU_Yes:
	  aRequest=0;
	  serialPortSetReserved( aChoice);
	  break;
	    
	case MENU_Previous:
	  aChoice = (aChoice > 0) ? --aChoice : MaxSerialPort-1;
	  break;
	    
	default:
	  aChoice = (aChoice >= MaxSerialPort-1) ? 0 : ++aChoice;
	  break;
	}
    }
  *thePort=aChoice;
}


void serialOrUSBPortChoose( enum serialPortIdentifier * thePort, int* theUSB)
{
  ENTER("serialOrUSBPortChoose");
  int aRequest=0;
  int aChoice=*thePort;
  int aQuestion=1;

  static struct serialPortItem myPortsUSB[]=
    {
      {"/dev/ttyS0",FirstSerialPort},
      {"/dev/ttyS1",SecondSerialPort},
      {"usb",usbPort},
    };

  enum {MaxPortsUSB=sizeof(myPortsUSB)/sizeof(myPortsUSB[0]),
	UsbPortIndex=2}; /* index in myPortsUSB */

  if (aChoice >= MaxPortsUSB)
    {
      aChoice=0;
    }

  if (*theUSB)
    {
      aChoice=UsbPortIndex;
    }

  say(portIs);
  say(myPortsUSB[aChoice].myName);
  say(changePort);
  say(PleasePressKey);

  if (MENU_Yes == getAnswer())
    {
      aRequest=1;
      say(whichPort);
    }

  while(aRequest)
    {
      say(myPortsUSB[aChoice].myName);

      if (aQuestion)
	{
	  say( PleasePressKey);
	  aQuestion=0;
	}

      switch(getAnswer())
	{
	case MENU_Yes:
	  aRequest=0;
	  if (aChoice != UsbPortIndex)
	    {
	      serialPortSetReserved( aChoice);
	    }
	  break;

	case MENU_Previous:
	  aChoice = (aChoice > 0) ? --aChoice : MaxPortsUSB-1;
	  break;

	default:
	  aChoice = (aChoice >= MaxPortsUSB-1) ? 0 : ++aChoice;
	  break;
	}
    }
  if (aChoice==UsbPortIndex)
    {
      *theUSB=1;
    }
  else
    {
      *theUSB=0;
      *thePort=aChoice;
    }
}


enum serialPortIdentifier getEnumSerialPort(char* thePort)
{
  enum serialPortIdentifier aPort=TTYS0;
  int i=0;

  if (thePort)
    {
      for (i=0; i<sizeof(myPorts)/sizeof(myPorts[0]); i++)
	{
	  if (strcmp( myPorts[i].myIdentifier, thePort)==0)
	    {
	      aPort=i;
	      break;
	    }
	}
    }
  return aPort;
}

char* serialPortGetString( enum serialPortIdentifier theValue)
{
  char* aLabel=myPorts[0].myIdentifier;
  if (theValue < MaxSerialPort)
    {
      aLabel=myPorts[theValue].myIdentifier;
    }
  return aLabel;
}

enum sentence serialPortGetSentence( enum serialPortIdentifier theValue)
{
  enum sentence aLabel=myPorts[0].myName;
  if (theValue < MaxSerialPort)
    {
      aLabel=myPorts[theValue].myName;
    }
  return aLabel;
}
