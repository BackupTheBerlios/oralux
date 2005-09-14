#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linuxconsole.h"
#include "debug.h"

/* < getDigit */
int getDigit(char* theBuffer, int theLength)
{
  int aValue=0;
  int i;
  ENTER("getDigit");

  for (i=0; i<theLength; i++)
    {
      aValue = aValue*10 + theBuffer[i] - '0';    
    }

  SHOW2("aValue=%d\n",aValue);

  return aValue;
}
/* > */

/* < getStyle */

/* Each integer is extracted from the SGR string and converted in the related bit */
void getStyle( struct t_style* theStyle, struct t_style* theDefaultStyle)
{
  char* aString = NULL;
  char* aToken = NULL;
  int aValue=0;
	  
  ENTER("getStyle");

  aString = strdup(yytext + 2); /* + 2: forget ESC [ */
  aToken=strtok( aString,";m");

  SHOW("The original style:\n");
  DISPLAY_STYLE(theStyle);
  
  while( aToken)
    {
      switch( aValue=atoi(aToken))
	{
	case 0:
	  {
	    int aAlternateStyle=theStyle->isAlternate;
	    copyStyle(theStyle, theDefaultStyle);
	    theStyle->isAlternate=aAlternateStyle;
	  }
	  break;   
	case 1:
	  theStyle->isBold=1;
	  break;
	case 2:
	  theStyle->isDim=1;
	  break;
	case 4:
	  theStyle->isUnderline=1;
	  break;
	case 5:
	  theStyle->isBlink=1;
	  break;
	case 7:
	  theStyle->isReverse=1;
	  break;
	  /* not found in console.c */
/* 	case 8: */
/* 	  theStyle->isBlank=1; */
/* 	  break; */
	case 10:
	  theStyle->isAlternate=0;
	  break;
	case 11:
	  theStyle->isAlternate=1;
	  break;
	case 12:
	  /* TBD: secondary primary font */
	  theStyle->isAlternate=1;
	  break;
	case 21:
	case 22:
	  theStyle->isDim=0;
	  theStyle->isBold=0;
	  break;
	case 24:
	  theStyle->isUnderline=0;
	  break;
	case 25:
	  theStyle->isBlink=0;
	  break;
	case 27:
	  theStyle->isReverse=0;
	  break;
	case 38:
	  theStyle->myForegroundColor=theDefaultStyle->myForegroundColor;
	  theStyle->isUnderline=1;
	  break;
	case 39:
	  theStyle->myForegroundColor=theDefaultStyle->myForegroundColor;
	  theStyle->isUnderline=0;
	  break;
	case 49:
	  theStyle->myBackgroundColor=theDefaultStyle->myBackgroundColor;
	  break;
	default: 
	  if ((aValue >= 30) && (aValue <= 37))
	    {
	      theStyle->myForegroundColor = aValue - 30;
	    }	
	  else if ((aValue>=40) && (aValue<=47))
	    {
	      theStyle->myBackgroundColor = aValue - 40;
	    }	

	  /* MODE_PROTECT: not defined */
	  break;
	}
      SHOW2("aValue=%d\n",aValue);
      DISPLAY_STYLE(theStyle);
      aToken=strtok( NULL,";m");
    }
  free( aString);
}

/* > */

/* 
Local variables:
folded-file: t
folding-internal-margins: nil
*/
