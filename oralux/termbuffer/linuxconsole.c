#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linuxconsole.h"

/* < getDigit */int getDigit(char* theBuffer, int theLength){  int aValue=0;  int i;  for (i=0; i<theLength; i++)    {      aValue = aValue*10 + theBuffer[i]-'0';    }  return aValue;}/* > */

/* < getStyle */

/* Each integer is extracted from the SGR string and converted in the related bit */
void getStyle( struct t_style* theStyle)
{
  char* aString = NULL;
  char* aToken = NULL;
  int aValue=0;
	  
  aString = strdup(yytext + 2); /* + 2: forget ESC [ */
  aToken=strtok( aString,";m");
  
  while( aToken)
    {
      switch( aValue=atoi(aToken))
	{
	case 0:
	  clearStyle(theStyle);
	  theStyle->myBackgroundColor=myDefaultBackgroundColor;
	  theStyle->myForegroundColor=myDefaultForegroundColor;
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
	case 8:
	  theStyle->isBlank=1;
	  break;
	case 11:
	  theStyle->isAlternate=1;
	  break;
	default: 
	  if ((aValue >= 30) && (aValue <= 39))
	    {
	      theStyle->myForegroundColor = aValue - 30;
	    }	
	  else if ((aValue>=40) && (aValue<=49))
	    {
	      theStyle->myBackgroundColor = aValue - 40;
	    }	

	  /* MODE_PROTECT: not defined */
	  break;
	}
      aToken=strtok( NULL,";m");
    }
  free( aString);
}

/* > */

