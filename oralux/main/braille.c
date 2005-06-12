// ----------------------------------------------------------------------------
// braille.c
// $Id: braille.c,v 1.4 2005/06/12 20:54:01 gcasse Exp $
// $Author: gcasse $
// Description: setting for BRLTTY
// $Date: 2005/06/12 20:54:01 $ |
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
#include <stdlib.h>
#include <string.h>
#include "constants.h"
#include "menu.h"
#include "braille.h"
#include "audioUserInterface.h"
#include "getnchar.h"
#include "serialPort.h"
#include <sys/stat.h>

/* {{{ Data */

struct brailleItem 
{
  char* myIdentifier;
  enum sentence myName;
};

// Warning: same order as the brailleIdentifier enumerate
static struct brailleItem myDrivers[]=
  {
    {"at", Albatross},
    {"al", Alva},
    {"bl", BrailleLite},
    {"bn", BrailleNote},
    {"cb", CombiBraille},
    {"ec", EcoBraille},
    {"eu", EuroBraille},
    {"fs", FreedomScientific},
    {"ht", HandyTech},
    {"lt", LogText},
    {"mb", MultiBraille},
    {"md", Say_MDV},
    {"mn", MiniBraille},
    {"pm", Papenmeier},
    {"ts", Say_TSI},
    {"va", Vario},
    {"vd", VideoBraille},
    {"vh", VarioHT},
    {"vo", Voyager},
    {"vs", VisioBraille},
  };

static struct brailleItem myTables[]=
  {
    {"text.brf.tbl" ,brfSaid},
    {"text.cz.tbl" ,czechSaid},
    {"text.da.tbl" ,danskSaid},
    {"text.de.tbl" ,germanSaid},
    {"text.es.tbl" ,spanishSaid},
    {"text.fi1.tbl" ,finnishSaid},
    {"text.fi2.tbl" ,finnish2Said},
    {"text.fr-iso01.tbl" ,frenchSaid},
    {"text.fr-cbifs.tbl" ,French_cbifs},
    {"text.it.tbl" ,italianSaid},
    {"text.nabcc.tbl" ,NABCC},
    {"text.no-h.tbl" ,NorwegianGerman},
    {"text.no-p.tbl" ,Norwegian},
    {"text.pl.tbl" ,polishSaid},
    {"text.ru.tbl" ,russianSaid},
    {"text.se1.tbl" ,Swedish},
    {"text.se2.tbl" ,Swedish2},
    {"text.uk.tbl" ,britishSaid},
    {"text.us.tbl" ,americanSaid},
    {"text.vni.tbl" ,Vietnamese},
  };
#define MaxTables (sizeof(myTables)/sizeof(myTables[0]))

static enum brailleTableIdentifier mySortedTablesSentences[MaxTables];


static struct brailleItem myContractionTables[]=
  {
    {NULL ,noContractionSaid},
    {"big5.ctb" ,big5Said},
    {"compress.ctb" ,compressSaid},
    {"en-us-g2.ctb" ,en_us_g2_Said},
    {"fr-abrege.ctb" ,fr_abregeSaid},
    {"fr-integral.ctb" ,fr_integralSaid},
  };
#define MaxContractionTables (sizeof(myContractionTables)/sizeof(myContractionTables[0]))

static enum brailleContractionTableIdentifier mySortedContractionTablesSentences[ MaxContractionTables];

// Name index of the array TheConfigurationFieldName
enum ConfigurationFieldId {
  FIELD_PORT,
  FIELD_DRIVER,
  FIELD_TABLE,
  FIELD_CONTRACTION_TABLE,
};

static char* myConfigurationFieldName[]={
  "braille-device", 
  "braille-driver", 
  //  "speech-driver",
  "text-table",
  "contraction-table",
};

const char* myConfigurationFile="/etc/brltty.conf";

enum MENU_State {
  MENU_Braille,
  MENU_Table,
  MENU_Contraction_Table,
  MENU_Port,
  MENU_Confirm,
  MENU_End,
};

/* }}} */

/* {{{ setInfo */

static void setInfo( enum ConfigurationFieldId theFieldID, char* theWord, struct brailleInfo * theInfo)
{
  ENTER("setInfo");
  int i=0;

  switch( theFieldID)
    {
    case FIELD_DRIVER:
      theInfo->myBrailleIdentifier=0;
      for (i=0; i<sizeof(myDrivers)/sizeof(myDrivers[0]); i++)
	{
	  if (strcmp( myDrivers[i].myIdentifier, theWord) == 0)
	    {
	      theInfo->myBrailleIdentifier=i;
	      break;
	    }
	}
      break;

    case FIELD_PORT:
      theInfo->myPort=getEnumSerialPort(theWord);
      break;

    case FIELD_TABLE:
      theInfo->myBrailleTableIdentifier=0;
      for (i=0; i<MaxTables; i++)
	{
	  if (strcmp( myTables[i].myIdentifier, theWord)==0)
	    {
	      theInfo->myBrailleTableIdentifier=i;
	      break;
	    }
	}
      break;

    case FIELD_CONTRACTION_TABLE:
      theInfo->myBrailleContractionTableIdentifier=0;
      for (i=0; i<MaxContractionTables; i++)
	{
	  if (myContractionTables[i].myIdentifier // NULL possible
	      && strcmp( myContractionTables[i].myIdentifier, theWord)==0)
	    {
	      theInfo->myBrailleContractionTableIdentifier=i;
	      break;
	    }
	}
      break;

    default:
      break;
    }
}

/* }}} */

/* {{{ Configuration */

static int isConf()
{
  ENTER("isConf");
  struct stat aFileInfo;
  int aStatus = (stat( myConfigurationFile, &aFileInfo) == 0);
  SHOW2("aStatus=%d", aStatus);
  return aStatus;
}

static int getConf( struct brailleInfo * theInfo)
{
  ENTER("getConf");
  char* aLine=malloc(BUFSIZE);

  FILE* fd=fopen(myConfigurationFile, "r");
  if (fd==NULL)
    {
      free(aLine);
      return 0;
    }

  while(fgets(aLine, BUFSIZE, fd))
    {
      int i=0;
      char* aValue=NULL;
      const char* aDelim=" \t\r\n";
      char* aKey=strtok( aLine, aDelim);

      if ((aKey==NULL) || (*aKey=='#'))
	{
	  continue;
	}

      aValue=strtok(NULL, aDelim);
      if ((aValue==NULL) || (*aValue=='#'))
	{
	  continue;
	}

      for (i=0; i < sizeof(myConfigurationFieldName)/sizeof(myConfigurationFieldName[0]); i++)
	{
	  if (strcmp(aKey, myConfigurationFieldName[i])==0)
	    {
	      setInfo(i, aValue, theInfo);
	      break;
	    }
	}
    }
  fclose(fd);
  free(aLine);
  return 1;
}

static int setConf( struct brailleInfo * theInfo)
{
  ENTER("setConf");
  char* aLine=malloc(BUFSIZE);
  FILE* fd=fopen(myConfigurationFile, "w");

  if (fd==NULL)
    {
      return 0;
    }

  fprintf(fd, "# This file was automatically generated by Oralux\n");

  if (theInfo->myPortIsUSB)
    {
      fprintf(fd, "braille-device usb:\n");
    }
  else
    {
      fprintf(fd, "braille-device serial:%s\n", serialPortGetString( theInfo->myPort ));
    }

  fprintf(fd, "braille-driver %s\n", myDrivers[ theInfo->myBrailleIdentifier ].myIdentifier);
  fprintf(fd, "text-table %s\n", myTables[ theInfo->myBrailleTableIdentifier ].myIdentifier);
  // If no contraction table: null pointer
  if (myContractionTables[ theInfo->myBrailleContractionTableIdentifier ].myIdentifier)
    {
      fprintf(fd, "contraction-table %s\n", myContractionTables[ theInfo->myBrailleContractionTableIdentifier ].myIdentifier);
    }
  fclose(fd);
  free(aLine);
  return 1;
}

static void delConf()
{
  ENTER("delConf");
  unlink(myConfigurationFile);    
}

void getDefaultConf( enum keyboard theKeyboard, struct brailleInfo * theInfo)
{
  ENTER("getDefaultConf");

  if (!getConf( theInfo))
    {
      theInfo->myBrailleIdentifier=ALVA;
      theInfo->myPort=TTYS0;
      theInfo->myBrailleContractionTableIdentifier=noContraction;

      switch(theKeyboard)
	{
	case americanSaid:
	  theInfo->myBrailleTableIdentifier=US_TABLE;	  
	  break;

	case belgianSaid:
	  theInfo->myBrailleTableIdentifier=DE_TABLE;	  
	  break;

	case britishSaid:
	  theInfo->myBrailleTableIdentifier=UK_TABLE;	  
	  break;

	case danskSaid:
	  theInfo->myBrailleTableIdentifier=DA_TABLE;
	  break;

	case frenchSaid:
	  theInfo->myBrailleTableIdentifier=FR_CBIFS_TABLE;	  
	  break;

	case frenchcanadianSaid:
	  theInfo->myBrailleTableIdentifier=FR_TABLE;	  
	  break;

	case germanSaid:
	  theInfo->myBrailleTableIdentifier=DE_TABLE;	  
	  break;

	case italianSaid: 
	  theInfo->myBrailleTableIdentifier=IT_TABLE;	  
	  break;

	case polishSaid:
	  theInfo->myBrailleTableIdentifier=PL_TABLE;	  
	  break;

	case russianSaid:
	  theInfo->myBrailleTableIdentifier=RU_TABLE;	  
	  break;

	case spanishSaid:
	  theInfo->myBrailleTableIdentifier=ES_TABLE;	  
	  break;

	case swissSaid:
	  theInfo->myBrailleTableIdentifier=DE_TABLE;	  
	  break;

	case bulgarianSaid:
	case czechSaid:
	case finnishSaid:
	case slovakSaid:
	case turkishSaid:
	default:
	  theInfo->myBrailleTableIdentifier=NABCC_TABLE;
	  break;
	}
    }
}

/* }}} */

/* {{{ Choose braille, table, port; and confirmation */

static void chooseBraille( enum brailleIdentifier * theBrailleIdentifier)
{
  ENTER("chooseBraille");
  int aRequest=0;
  int aChoice=*theBrailleIdentifier;
  int aQuestion=1;

  if (aChoice >= sizeof(myDrivers)/sizeof(myDrivers[0]))
    {
      aChoice=0;
    }

  say(brailleIs);
  say(myDrivers[aChoice].myName);
  say(changeBraille);
  say(PleasePressKey);

  if (MENU_Yes == getAnswer())
    {
      aRequest=1;
      say(whichBraille);
    }

  while(aRequest)
    {
      say(myDrivers[aChoice].myName);

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
	  aChoice = (aChoice > 0) ? --aChoice : MaxBrailleIdentifier-1;
	  break;
	    
	default:
	  aChoice = (aChoice >= MaxBrailleIdentifier-1) ? 0 : ++aChoice;
	  break;
	}
    }

  *theBrailleIdentifier=aChoice;
}

static int sortTables( const void* theTable_A, const void* theTable_B)
{
  ENTER("sortTables");

  enum brailleTableIdentifier * a=(enum brailleTableIdentifier *)theTable_A;
  enum brailleTableIdentifier * b=(enum brailleTableIdentifier *)theTable_B;

  SHOW3("\na=%s; b=%s", getText( myTables[*a].myName ), getText( myTables[*b].myName ));
  return strcmp( getText( myTables[*a].myName ), getText( myTables[*b].myName ));
} 

static int sortContractionTables( const void* theTable_A, const void* theTable_B)
{
  ENTER("sortContractionTables");

  enum brailleContractionTableIdentifier * a=(enum brailleContractionTableIdentifier *)theTable_A;
  enum brailleContractionTableIdentifier * b=(enum brailleContractionTableIdentifier *)theTable_B;

  SHOW3("\na=%s; b=%s", getText( myContractionTables[*a].myName ), getText( myContractionTables[*b].myName ));
  return strcmp( getText( myContractionTables[*a].myName ), getText( myContractionTables[*b].myName ));
} 

static void chooseTable( enum brailleTableIdentifier * theBrailleTableIdentifier)
{
  ENTER("chooseTable");
  int aRequest=0;
  int aChoice=*theBrailleTableIdentifier;
  int aQuestion=1;
  int i=0;

  if (aChoice >= MaxTables)
    {
      aChoice=0;
    }

  say(tableIs);
  say(myTables[aChoice].myName);
  say(changeTable);
  say(PleasePressKey);

  if (MENU_Yes == getAnswer())
    {
      aRequest=1;
      say(whichTable);

      // Sorting the sentences according to the current language
      for (i=0; i<MaxTables; i++)
	{
	  mySortedTablesSentences[i]=i;
	}

      qsort( mySortedTablesSentences, MaxTables, sizeof(mySortedTablesSentences[0]), sortTables);

      // Finding the new index 

      for (i=0; i<MaxTables; i++)
	{
	  if (aChoice == mySortedTablesSentences[i])
	    {
	      break;
	    }
	}
    }

  while(aRequest)
    {
      aChoice=mySortedTablesSentences[i];
      say(myTables[ aChoice ].myName);

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
	  i = (i > 0) ? --i : MaxBrailleTableIdentifier-1;
	  break;

	default:
	  i = (i >= MaxBrailleTableIdentifier-1) ? 0 : ++i;
	  break;
	}
    }
  *theBrailleTableIdentifier=aChoice;
}

static void chooseContractionTable( enum brailleContractionTableIdentifier * theBrailleContractionTableIdentifier)
{
  ENTER("chooseContractionTable");
  int aRequest=0;
  int aChoice=*theBrailleContractionTableIdentifier;
  int aQuestion=1;
  int i=0;

  if (aChoice >= MaxContractionTables)
    {
      aChoice=0;
    }

  say(contractionTableIs);
  say(myContractionTables[aChoice].myName);
  say(changeContractionTable);
  say(PleasePressKey);

  if (MENU_Yes == getAnswer())
    {
      aRequest=1;
      say(whichContractionTable);

      // Sorting the sentences according to the current language
           for (i=0; i<MaxContractionTables; i++)
      	{
      	  mySortedContractionTablesSentences[i]=i;
      	}

           qsort( mySortedContractionTablesSentences, MaxContractionTables, sizeof(mySortedContractionTablesSentences[0]), sortContractionTables);

      // Finding the new index 

      for (i=0; i<MaxContractionTables; i++)
	{
	  if (aChoice == mySortedContractionTablesSentences[i])
	    {
	      break;
	    }
	}
    }

  while(aRequest)
    {
      aChoice=mySortedContractionTablesSentences[i];
      say(myContractionTables[ aChoice ].myName);

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
	  i = (i > 0) ? --i : MaxBrailleContractionTableIdentifier-1;
	  break;

	default:
	  i = (i >= MaxBrailleContractionTableIdentifier-1) ? 0 : ++i;
	  break;
	}
    }
  *theBrailleContractionTableIdentifier=aChoice;
}

void confirmSelection( struct brailleInfo * theInfo)
{
  ENTER("confirmSelection");
  say(brailleIs);
  say(myDrivers[ theInfo->myBrailleIdentifier ].myName);
  
  say(tableIs);
  say(myTables[ theInfo->myBrailleTableIdentifier ].myName);

  say(contractionTableIs);
  say(myContractionTables[ theInfo->myBrailleContractionTableIdentifier ].myName);
  
  say(portIs);

  if (theInfo->myPortIsUSB)
    {
      say(usbPort);
    }
  else
    {
      say(serialPortGetSentence( theInfo->myPort));
    }
  say(startBraille);
  say(PleasePressKey);

  if (MENU_Yes == getAnswer())
    {
      setConf( theInfo);
      system("killall brltty 2>/dev/null; brltty");
      system("cd /etc/rcS.d; cp ../init.d/brltty S25brltty");
    }
}

void confirmNoBraille()
{
  ENTER("confirmNoBraille");
  if (isConf())
    {
      say(NoBraille);
      if (MENU_Yes == getAnswer())
	{
	  delConf();
	  system("killall brltty 2>/dev/null;");
	  system("rm -f /etc/rcS.d/S25brltty");
	}
    }
}

/* }}} */

void setBraille( struct brailleInfo * theInfo)
{
  ENTER("setBraille");

  say(selectBraille);
  say(PleasePressKey);

  switch (getAnswer())
    {
    case MENU_Yes:
      {
	enum MENU_State aMenuState=MENU_Braille;
	while(aMenuState != MENU_End)
	  {
	    switch(aMenuState)
	      {
	      case MENU_Braille:
		chooseBraille( & theInfo->myBrailleIdentifier);
		aMenuState = (GNC_UpArrowKey == getLastKeyPressed()) ? MENU_End : MENU_Table;
		break;
		
	      case MENU_Table:
		chooseTable( & theInfo->myBrailleTableIdentifier);
		aMenuState = (GNC_UpArrowKey == getLastKeyPressed()) ? MENU_Braille : MENU_Contraction_Table;
		break;
		
	      case MENU_Contraction_Table:
		chooseContractionTable( & theInfo->myBrailleContractionTableIdentifier);
		aMenuState = (GNC_UpArrowKey == getLastKeyPressed()) ? MENU_Table : MENU_Port;
		break;

	      case MENU_Port:
		serialOrUSBPortChoose( & theInfo->myPort, 
				       & theInfo->myPortIsUSB);
		aMenuState = (GNC_UpArrowKey == getLastKeyPressed()) ? MENU_Contraction_Table : MENU_Confirm;
		break;
		
	      case MENU_Confirm:
		confirmSelection( theInfo);
		aMenuState = (GNC_UpArrowKey == getLastKeyPressed()) ? MENU_Port : MENU_End;
		break;
		
	      default:
		aMenuState = MENU_End;
		break;
	      }
	  }
      }
      break;

    case MENU_No:
      confirmNoBraille();
      break;

    default:
      break;
    }
}

