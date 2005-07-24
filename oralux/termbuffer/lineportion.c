/* 
----------------------------------------------------------------------------
linePortion.c
$Id: lineportion.c,v 1.1 2005/07/24 20:43:29 gcasse Exp $
$Author: gcasse $
Description: manage line portions.
$Date: 2005/07/24 20:43:29 $ |
$Revision: 1.1 $ |
Copyright (C) 2005 Gilles Casse (gcasse@oralux.org)

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
----------------------------------------------------------------------------
*/
#include <stdlib.h>
#include "lineportion.h"
#include "debug.h"

/* < linePortion */
linePortion* createLinePortion (int theLine, int theCol, style* theStyle, chartype* theString)
{
    linePortion* this = (linePortion*) malloc(sizeof(linePortion));

    ENTER("createLinePortion");

    this->myLine = theLine;
    this->myFirstCol = theCol;
    this->myLastCol = theCol+strlen(theString);
    copyStyle(&(this->myStyle), theStyle);
    this->myString = g_string_new(theString);
    SHOW2("theLine=%d\n", theLine);
    SHOW2("theCol=%d\n", theCol);
    SHOW2("theString=%s\n", theString);
    DISPLAY_STYLE(theStyle);
    return this;
}

void deleteLinePortion( linePortion* this)
{
    ENTER("deleteLinePortion");
    if (this)
      {
	g_string_free( this->myString, 1);
	free(this);
      }
}
/* > */

/* < linePortionGroup */
static void deleteLinePortionHook( gpointer theLinePortion, gpointer theFoo)
{
  ENTER("deleteLinePortionHook");
  deleteLinePortion( (linePortion*)theLinePortion);
}

void deleteLinePortionGroup( GList* this)
{
  ENTER("deleteLinePortionGroup");
  if (this)
    {
      g_list_foreach( this, (GFunc)deleteLinePortionHook, NULL);
    }
}

enum styleWeightStatus {WEIGHT_UNDEFINED, WEIGHT_AVAILABLE, STYLE_DUPLICATED};
struct t_styleWeight 
{
  int myWeight; 
  enum styleWeightStatus myStatus; 
  linePortion* myLinePortion; 
};
typedef struct t_styleWeight styleWeight;

static void initStyleWeightHook( gpointer theLinePortion, gpointer theStyleWeight)
{
  styleWeight** w = (styleWeight**)theStyleWeight;

  ENTER("initStyleWeightHook");

  (*w)->myWeight = 0; 
  (*w)->myStatus = WEIGHT_UNDEFINED; 
  (*w)->myLinePortion = (linePortion*)theLinePortion;
  SHOW2("str=%s\n",((linePortion*)theLinePortion)->myString->str);
  (*w)++;
}

int getFeaturesLinePortionGroup( GList* this, linePortion* theFeatures)
{
  int aResultIsAvailable=0;
  int aCount = 0;

  ENTER("getFeaturesLinePortionGroup");

  if (this &&
      /* affectation */
      (aCount = g_list_length(this)))
    {
      styleWeight* aWeight = (styleWeight*)malloc( aCount * sizeof(styleWeight));
      styleWeight* w = aWeight;
      int j,k;

      /* < main style */
      g_list_foreach( this, (GFunc)initStyleWeightHook, &w);

      for ( j=0; j < aCount; j++)
	{
	  if (aWeight[ j].myStatus != WEIGHT_UNDEFINED)
	    {
	      continue;
	    }
	  aWeight[ j].myWeight = strlen( aWeight[ j].myLinePortion->myString->str);
	  aWeight[ j].myStatus = WEIGHT_AVAILABLE;
	    
	  for (k=j+1; k < aCount; k++)
	    {
	      if (aWeight[ k].myStatus != WEIGHT_UNDEFINED)
		{
		  continue;
		}
	      if (compareStyle( &(aWeight[ j].myLinePortion->myStyle), 
				&(aWeight[ k].myLinePortion->myStyle)))
		{
		  aWeight[ j].myWeight += strlen( aWeight[ k].myLinePortion->myString->str);
		  aWeight[ k].myStatus = STYLE_DUPLICATED;
		}
	    }
	}
	
      /* Max weight */
      k=0; 
      for (j=0; j < aCount; j++)
	{
	  if (aWeight[ j].myStatus != WEIGHT_AVAILABLE)
	    {
	      continue;
	    }
	  if (aWeight[ j].myWeight > aWeight[ k].myWeight)
	    {
	      k=j;
	    }
	}
      copyStyle( &(theFeatures->myStyle), &(aWeight[ k].myLinePortion->myStyle));
      /* > */
      /* < concatenate string */
      theFeatures->myString = g_string_new( getStringFromGList( this));

      for ( j=1; j < aCount; j++)
	{
	  g_string_append( theFeatures->myString, aWeight[ j].myLinePortion->myString->str);
	}
      /* > */

      theFeatures->myLine = aWeight[ 0].myLinePortion->myLine;
      theFeatures->myFirstCol = aWeight[ 0].myLinePortion->myFirstCol;
      theFeatures->myLastCol = aWeight[ aCount-1].myLinePortion->myLastCol;

      free(aWeight);

      aResultIsAvailable=1;
    }
  return aResultIsAvailable;
}
/* > */
