// ----------------------------------------------------------------------------
// minimenu.c
// $Id: minimenu.c,v 1.1 2004/09/27 20:30:28 gcasse Exp $
// $Author: gcasse $
// Description: a very simple menu to easily call text-based programs.
// $Date: 2004/09/27 20:30:28 $ |
// $Revision: 1.1 $ |
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
#include <stdlib.h>

#include "constants.h"
#include "getnchar.h"

static char TheLine[BUFSIZE];
static int pf=0;

#define QUIT_MINIMENU "quitYASR"
#define MINIMENU "/tmp/minimenu.tmp"

#ifdef BID
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
#endif

struct item {
  char* myTitle;
  char* myCommand;
  char* myTutorial;
};

struct item TheItems[]=
  {
    {
      "YASR documentation",
      "ne /usr/share/oralux/doc/README.yasr",
      NULL,
    },
    {
      "N E, the nice editor",
      "ne",
      NULL,
    },
    {
      "Links, web browser with javascript capabilities",
      "links /usr/share/oralux/doc/htm/faq.htm",
      NULL,
    },
    {
      "zsh, the Z shell",
      "zsh",
      NULL,
    },
    {
      "Calculator",
      "python",
      NULL,
    },
    {
      "Quit YASR",
      QUIT_MINIMENU,
      NULL,
    },
  };

struct item TheFrenchItems[]=
  {
    {
      "La documentation de YASR",
      "ne /usr/share/oralux/doc/README-fr.yasr",
      NULL,
    },
    {
      "N E, un éditeur de texte plutôt intuitif",
      "ne",
      NULL,
    },
    {
      "Links, navigateur web avec un support javascript",
      "links /usr/share/oralux/doc/htm/faq.htm",
      NULL,
    },
    {
      "zsh, le shell Z",
      "zsh",
      NULL,
    },
    {
      "Calculatrice",
      "python",
      NULL,
    },
    {
      "Quitter YASR",
      QUIT_MINIMENU,
      NULL,
    },
  };

#define MaxItems sizeof(TheItems)/sizeof(TheItems[0])

typedef struct item (typeArrayItems)[];


enum SelectedCommand {
  Yes,
  No,
  Previous,
  Next,
};


// getAnswer
enum SelectedCommand getAnswer()
{
  ENTER("getAnswer");
  enum SelectedCommand aMenuAnswer=Yes;

  enum GNC_Answer aStatus=GNC_Ok;
  char* aPressedKey=getnchar(pf, 1, 0, 0, &aStatus, NULL);

  switch(aStatus)
    {
    case GNC_UpArrowKey:
      aMenuAnswer = Previous;
      break;

    case GNC_DownArrowKey:
      aMenuAnswer = Next;
      break;	

    case GNC_Ok:
    default:
      if (strcmp(aPressedKey, "\n")==0)
	{
	  aMenuAnswer = Yes;
	}
      else
	{
	  aMenuAnswer = No;
	}
      break;
    }
  return (aMenuAnswer);
}

// main
// Entry point
int main(int argc, char *argv[])
{
  ENTER("main");
  if (argc!=2)
    {
      SHOW("oralux current_tty\n");
      exit(1);
    }

  char *portname = argv[1];

  if (!initAUI(NULL, 0, portname))
    {
      return (1);
    }

  //  readMenu(aMenu);
  int aIndex=0;
  typeArrayItems* aArray=&TheItems;
  
  if (strcmp(getenv("LANGUAGE"),"fr")==0)
    {
      aArray=&TheFrenchItems;
      fprintf(stderr, "Pour sélectionner un logiciel, appuyez s'il vous plait, sur la flèche vers le haut ou vers le bas.\n");
      fprintf(stderr, "Ensuite appuyez sur entrée, pour lancer votre logiciel.\n");
    }
  else
    {
      fprintf(stderr, "Please press the Up or Down arrow keys, to select your software.\n");
      fprintf(stderr, "Then, press return to start your software.\n");
    }

  while( aIndex < MaxItems)
    {
      beepLoudSpeaker();
      SHOW2("aIndex=%d\n",aIndex);
      printf("%s\n", (*aArray)[aIndex].myTitle);
      switch( getAnswer())
	{
	case Previous:
	  aIndex = (aIndex==0) ? MaxItems-1 : aIndex-1;
	  break;
	  
	case Next:
	  aIndex = (aIndex+1==MaxItems) ? 0 : aIndex+1;
	  break;
	  
	case Yes:
	  SHOW1("Yes\n");
	  if (strcmp((*aArray)[aIndex].myCommand, QUIT_MINIMENU)==0)
	    {
	      stopAUI(0);
	      // Create a temporary file: correct exit
	      FILE* fd=fopen(MINIMENU,"w");
	      fclose(fd);
	      return(0);
	    }
	  else
	    {
	      system((*aArray)[aIndex].myCommand);
	    }
	  break;
	  
	default:
	  break;
	}
    }
}
