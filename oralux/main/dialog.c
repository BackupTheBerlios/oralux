// ----------------------------------------------------------------------------
// dialog.c
// $Id: dialog.c,v 1.1 2004/09/27 20:30:28 gcasse Exp $
// $Author: gcasse $
// Description: Text based dialog (no graphical layout). 
// $Date: 2004/09/27 20:30:28 $ |
// $Revision: 1.1 $ |
// Copyright (C) 2004 Gilles Casse (gcasse@oralux.org)
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
#include "constants.h"


static char* myTitle=NULL;
static char* myBackTitle=NULL;

// Menu
struct t_item {
  char* myName;
  char* myLabel;
};
static char* myMenuText=NULL;
#define MAX_ITEM 50
static struct t_item myMenuItem[MAX_ITEM];
static int myMenuItemNumber=0;

// Input box
struct t_inputBox {
  char* myText;
  char* myDefault;
};
static struct t_inputBox myInputBox={NULL, NULL};

// Radio list
struct t_radio {
  char* myName;
  char* myLabel;
  int myDefaultStatus;
  int myStatus;
};
static char* myRadioText=NULL;
static struct t_radio myRadio[MAX_ITEM];
static int myRadioNumber=0;

typedef struct t_radio (t_radioArray)[MAX_ITEM];


// Check list
static char* myCheckText=NULL;
static struct t_radio myCheck[MAX_ITEM];
static int myCheckNumber=0;

// YesNo
static char* myYesNoText=NULL;

// MsgBox
static char* myMsgBoxText=NULL;

// Gauge
static char* myGaugeText=NULL;

//
enum t_state {
  UNDEFINED,
  TITLE,
  BACKTITLE,
  MENU_TEXT,
  MENU_NUM1,
  MENU_NUM2,
  MENU_NUM3,
  MENU_NAME,
  MENU_LABEL,
  INPUT_BOX_TEXT,
  INPUT_BOX_NUM1,
  INPUT_BOX_NUM2,
  INPUT_BOX_INIT, // Option
  RADIO_TEXT,
  RADIO_NUM1,
  RADIO_NUM2,
  RADIO_NUM3,
  RADIO_NAME,
  RADIO_LABEL,
  RADIO_STATUS,
  YESNO_TEXT,
  YESNO_NUM1,
  YESNO_NUM2,
  MSGBOX_TEXT,
  MSGBOX_NUM1,
  MSGBOX_NUM2,
  CHECK_TEXT,
  CHECK_NUM1,
  CHECK_NUM2,
  CHECK_NUM3,
  CHECK_NAME,
  CHECK_LABEL,
  CHECK_STATUS,
  GAUGE_TEXT,
  GAUGE_NUM1,
  GAUGE_NUM2,
  GAUGE_PERCENT, // Option
  END,
};

// Internal buffer
#define MAXBUF 1000
static char myBuffer[MAXBUF];

static enum t_state getKeyword(char* theWord)
{
  ENTER("getKeyword");

  enum t_state aState=UNDEFINED;
  if (strcmp( theWord, "--title") == 0)
    {
      aState=TITLE;
    }
  else if (strcmp( theWord, "--backtitle") == 0)
    {
      aState=BACKTITLE;
    }
  else if (strcmp( theWord, "--menu") == 0)
    {
      aState=MENU_TEXT;
    }
  else if ((strcmp( theWord, "--inputbox") == 0)
	   || (strcmp( theWord, "--passwordbox") == 0))
    {
      aState=INPUT_BOX_TEXT;
    }
  else if (strcmp( theWord, "--radiolist") == 0)
    {
      aState=RADIO_TEXT;
    }
  else if (strcmp( theWord, "--yesno") == 0)
    {
      aState=YESNO_TEXT;
    }
  else if (strcmp( theWord, "--msgbox") == 0)
    {
      aState=MSGBOX_TEXT;
    }
  else if (strcmp( theWord, "--checklist") == 0)
    {
      aState=CHECK_TEXT;
    }
  else if (strcmp( theWord, "--gauge") == 0)
    {
      aState=GAUGE_TEXT;
    }

  SHOW3("theWord=%s, aState=%d\n",theWord,aState);  
  return aState;
}

static void parse(int argc, char *argv[])
{
  ENTER("parse");
  int i=0;
  enum t_state aState=UNDEFINED;

  for (i=0; (i<argc) && (aState!=END); i++)
    {
      SHOW3("->argv[%d]=%s\n",i,argv[i]);
      switch(aState)
	{
	case UNDEFINED:
	  aState=getKeyword(argv[i]);
	  break;

	case TITLE:
	  myTitle=argv[i];
	  aState=UNDEFINED;
	  break;

	case BACKTITLE:
	  myBackTitle=argv[i];
	  aState=UNDEFINED;
	  break;

	case MENU_TEXT:
	  myMenuText=argv[i];
	  aState=MENU_NUM1;
	  break;

	case MENU_NUM1:
	  aState=MENU_NUM2;
	  break;

	case MENU_NUM2:
	  aState=MENU_NUM3;
	  break;

	case MENU_NUM3:
	  aState=MENU_NAME;
	  myMenuItemNumber=0;
	  break;

	case MENU_NAME:
	  myMenuItem[ myMenuItemNumber].myName=argv[i];
	  myMenuItem[ myMenuItemNumber].myLabel=NULL;
	  aState=MENU_LABEL;
	  break;

	case MENU_LABEL:
	  myMenuItem[ myMenuItemNumber].myLabel=argv[i];
	  aState=MENU_NAME;
	  myMenuItemNumber++;
	  if (myMenuItemNumber > MAX_ITEM)
	    {
	      aState=END;
	    }
	  break;

	case INPUT_BOX_TEXT:
	  myInputBox.myText=argv[i];
	  myInputBox.myDefault=NULL;
	  aState=INPUT_BOX_NUM1;
	  break;

	case INPUT_BOX_NUM1:
	  aState=INPUT_BOX_NUM2;
	  break;

	case INPUT_BOX_NUM2:
	  aState=INPUT_BOX_INIT;
	  break;

	case INPUT_BOX_INIT:
	  myInputBox.myDefault=argv[i];
	  aState=END;
	  break;

	case RADIO_TEXT:
	  myRadioText=argv[i];
	  myRadioNumber=0;
	  aState=RADIO_NUM1;
	  break;

	case RADIO_NUM1:
	  aState=RADIO_NUM2;
	  break;

	case RADIO_NUM2:
	  aState=RADIO_NUM3;
	  break;

	case RADIO_NUM3:
	  aState=RADIO_NAME;
	  myRadioNumber=0;
	  break;

	case RADIO_NAME:
	  myRadio[ myRadioNumber].myName=argv[i];
	  myRadio[ myRadioNumber].myLabel=NULL;
	  myRadio[ myRadioNumber].myDefaultStatus=0;
	  aState=RADIO_LABEL;
	  break;

	case RADIO_LABEL:
	  myRadio[ myRadioNumber].myLabel=argv[i];
	  aState=RADIO_STATUS;
	  break;

	case RADIO_STATUS:
	  if ((strcmp(argv[i],"ON")==0) || (strcmp(argv[i],"on")==0))
	    {
	      myRadio[ myRadioNumber].myDefaultStatus=1;
	    }
	  aState=RADIO_NAME;
	  myRadioNumber++;
	  if (myRadioNumber > MAX_ITEM)
	    {
	      aState=END;
	    }
	  break;

	case YESNO_TEXT:
	  myYesNoText=argv[i];
	  aState=YESNO_NUM1;
	  break;

	case YESNO_NUM1:
	  aState=YESNO_NUM2;
	  break;

	case YESNO_NUM2:
	  aState=END;
	  break;

	case MSGBOX_TEXT:
	  myMsgBoxText=argv[i];
	  aState=MSGBOX_NUM1;
	  break;

	case MSGBOX_NUM1:
	  aState=MSGBOX_NUM2;
	  break;

	case MSGBOX_NUM2:
	  aState=END;
	  break;

	case CHECK_TEXT:
	  myCheckText=argv[i];
	  myCheckNumber=0;
	  aState=CHECK_NUM1;
	  break;

	case CHECK_NUM1:
	  aState=CHECK_NUM2;
	  break;

	case CHECK_NUM2:
	  aState=CHECK_NUM3;
	  break;

	case CHECK_NUM3:
	  aState=CHECK_NAME;
	  myCheckNumber=0;
	  break;

	case CHECK_NAME:
	  myCheck[ myCheckNumber].myName=argv[i];
	  myCheck[ myCheckNumber].myLabel=NULL;
	  myCheck[ myCheckNumber].myDefaultStatus=0;
	  aState=CHECK_LABEL;
	  break;

	case CHECK_LABEL:
	  myCheck[ myCheckNumber].myLabel=argv[i];
	  aState=CHECK_STATUS;
	  break;

	case CHECK_STATUS:
	  if ((strcmp(argv[i],"ON")==0) || (strcmp(argv[i],"on")==0))
	    {
	      myCheck[ myCheckNumber].myDefaultStatus=1;
	    }
	  aState=CHECK_NAME;
	  myCheckNumber++;
	  if (myCheckNumber > MAX_ITEM)
	    {
	      aState=END;
	    }
	  break;

	case GAUGE_TEXT:
	  myGaugeText=argv[i];
	  aState=GAUGE_NUM1;
	  break;

	case GAUGE_NUM1:
	  aState=GAUGE_NUM2;
	  break;

	case GAUGE_NUM2:
	  aState=GAUGE_PERCENT;
	  break;

	case GAUGE_PERCENT:
	  aState=END;
	  break;
	}
    }  
}

static void filterChars( char* theBuffer)
{
  ENTER("filterChars");
  if (theBuffer==NULL) 
    {
      return;
    }

  int aLength=strlen(theBuffer);
  if (aLength && (theBuffer[ aLength-1]=='\n'))
    {
      theBuffer[ aLength-1]=0;
    }
}

// theNumbers: string of numbers (e.g. " 1 2  8 ")
// theOffset: number - theOffset gives an index
// theArray: the myStatus field will be set to 1 if the array index is found.
// theMaxIndex: the found index must be lower than theMaxIndex
// Return value: 1 if ok
// 
// Warning: theNumbers is overwritten
static int updateItems( char* theNumbers, int theOffset, t_radioArray* theArray, int theMaxIndex)
{
  ENTER("updateItems");
  int i=0;
  char* aToken=NULL;
  char* aSeparator=" ";
  aToken=strtok( theNumbers, aSeparator);

  // Init
  for (i=0; i<theMaxIndex; i++)
    {
      (*theArray)[i].myStatus=0;
    }

  int aNumber=1;
  while( aToken && aNumber)
    {
      int aLength=strlen(aToken);
      if (aLength==0)
	{
	  aNumber=0;
	}
      else
	{
	  for (i=0; i<aLength; i++)
	    {
	      if (!isdigit(aToken[i]))
		{
		  aNumber=0;
		  break;
		}
	    }
	}

      SHOW2("token=%s=\n",aToken);

      if (aNumber)
	{
	  int aIndex = atoi(aToken) - theOffset;
	  SHOW2("index=%d\n",aIndex);
	  if ((0 <= aIndex) && (aIndex < theMaxIndex))
	    {
	      (*theArray)[aIndex].myStatus=1;
	      aToken=strtok( NULL, aSeparator);
	    }
	  else
	    {
	      aNumber=0;
	    }
	}
    }
  SHOW2("aNumber=%d\n",aNumber);
  return aNumber;
}

int dialog(int argc, char *argv[])
{
  ENTER("dialog");

  // 0: ok
  // 1: cancel
  // 255: esc
  int aStatus=0;

  parse( argc, argv);

  if (myBackTitle)
    {
      printf("%s\n", myBackTitle);
    }

  if (myTitle)
    {
      printf("%s\n", myTitle);
    }

  if (myMenuText)
    {
      unsigned int i=0;
      printf("%s\n", myMenuText);
      int anExpectedAnswer=1;
      
      while (anExpectedAnswer)
	{
	  // TBD
	  printf("Please, enter your choice \n");

	  for (i=0; i<myMenuItemNumber; i++)
	    {
	      printf("%d. %s: %s\n", i+1, myMenuItem[i].myName, myMenuItem[i].myLabel);
	    }

	  myBuffer[0]=0;
	  fgets(myBuffer, MAXBUF, stdin);
	  i=atoi(myBuffer);
	  SHOW2("i=%d\n", i);
	  if ((0 < i) && (i <= myMenuItemNumber))
	    {
	      anExpectedAnswer=0;
	      fprintf(stderr,"%s", myMenuItem[i-1].myName);
	    }
	}
    }

  else if (myInputBox.myText)
    {
      printf("%s\n", myInputBox.myText);

      // TBD
      printf("Please, enter your answer");
      if (myInputBox.myDefault)
	{
	  printf(" (default %s)", myInputBox.myDefault);
	}
      printf(": \n");

      fgets(myBuffer, MAXBUF, stdin);
      
      filterChars(myBuffer);

      if ((strlen(myBuffer)==0) && (myInputBox.myDefault))
	{
	  strcpy(myBuffer, myInputBox.myDefault);
	}

      fprintf(stderr,"%s", myBuffer);
    }

  else if (myRadioText)
    {
      printf("%s\n", myRadioText);
      int i=0;
      int aDefaultChoice=myRadioNumber;
      int anExpectedAnswer=1;

      while (anExpectedAnswer)
	{
	  for (i=0; i<myRadioNumber; i++)
	    {
	      char* aNotice=" ";
	      if (myRadio[i].myDefaultStatus)
		{
		  aDefaultChoice=i;
		  aNotice=" (selected)"; // TBD
		}
	      printf("%d. %s%s: %s\n", i+1, myRadio[i].myName, aNotice, myRadio[i].myLabel);
	    }

	  // TBD
	  printf("Please, enter your choice:\n");
	  myBuffer[0]=0;
	  fgets(myBuffer, MAXBUF, stdin);

	  if ((myBuffer[0]=='\n') && (aDefaultChoice!=myRadioNumber))
	    {
	      i=aDefaultChoice+1;
	    }
	  else
	    {
	      i=atoi(myBuffer);
	    }
	  SHOW2("i=%d\n", i);

	  if ((0 < i) && (i <= myRadioNumber))
	    {
	      anExpectedAnswer=0;
	      fprintf(stderr,"%s", myRadio[i-1].myName);
	    }
	}
    }

  else if (myYesNoText)
    {
      int i=-1;
      printf("%s\n", myYesNoText);
      int anExpectedAnswer=1;
      
      while (anExpectedAnswer)
	{
	  // TBD
	  printf("Please, press 0 for yes, 1 for no:\n");

	  myBuffer[0]=0;
	  fgets(myBuffer, MAXBUF, stdin);

	  if (strcmp(myBuffer,"0\n")==0)
	    {
	      i=0;
	    }
	  else if (strcmp(myBuffer,"1\n")==0)
	    {
	      i=1;
	    }

	  if ((i==0) || (i==1))
	    {
	      aStatus=i;
	      anExpectedAnswer=0;
	    }
	}
    }

  else if (myMsgBoxText)
    {
      int i=-1;
      printf("%s\n", myMsgBoxText);
      
      // TBD
      printf("Please, press a key to continue:\n");
      
      myBuffer[0]=0;
      fgets(myBuffer, MAXBUF, stdin);
    }

  else if (myCheckText)
    {
      printf("%s\n", myCheckText);
      int i=0;
      int anExpectedAnswer=1;

      while (anExpectedAnswer)
	{
	  for (i=0; i<myCheckNumber; i++)
	    {
	      char* aNotice=" ";
	      if (myCheck[i].myDefaultStatus)
		{
		  aNotice=" (selected)";
		}
	      printf("%d. %s%s: %s\n", i+1, myCheck[i].myName, aNotice, myCheck[i].myLabel);
	    }

	  // TBD
	  printf("Please, enter one or several choices separated by a space:\n");
	  myBuffer[0]=0;
	  fgets(myBuffer, MAXBUF, stdin);
	  filterChars(myBuffer);

	  if (myBuffer[0]==0)
	    {
	      for (i=0; i<myCheckNumber; i++)
		{
		  myCheck[i].myStatus=myCheck[i].myDefaultStatus;
		}
	      anExpectedAnswer=0;
	    }
	  else if (updateItems( myBuffer, 1, &myCheck, myCheckNumber))
	    {
	      anExpectedAnswer=0;
	    }
	}
      
      char* aSeparator="";
      for (i=0; i<myCheckNumber; i++)
	{
	  if (myCheck[i].myStatus)
	    {
	      fprintf(stderr,"%s%s", aSeparator, myCheck[i].myName);
	      aSeparator=" ";
	    }
	}
    }

  else if (myGaugeText)
    {
      printf("%s\n", myGaugeText);

      while(1)
	{
	  if (!fgets(myBuffer, MAXBUF, stdin))
	    {
	      break;
	    }
	  else if (strcmp(myBuffer,"XXX\n")!=0)
	    {
	      printf("%s", myBuffer);
	    }
	}
    }

  return aStatus;
}
