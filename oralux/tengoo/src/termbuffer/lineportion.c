/* 
----------------------------------------------------------------------------
linePortion.c
$Id: lineportion.c,v 1.3 2005/10/02 20:14:57 gcasse Exp $
$Author: gcasse $
Description: manage line portions.
$Date: 2005/10/02 20:14:57 $ |
$Revision: 1.3 $ |
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
/* < createLinePortion */
linePortion* createLinePortion (int theLine, int theCol, style* theStyle, chartyp* theString, GList* theParent)
{
  linePortion* this = NULL;

    ENTER("createLinePortion");

    if (!theStyle  || !theString)
      {
	return NULL;
      }

    this = (linePortion*) malloc(sizeof(linePortion));

    if (this)
      {
	this->myLine = theLine;
	this->myFirstCol = theCol;
	this->myLastCol = theCol+strlen(theString)-1;
	this->myParent = theParent;
	copyStyle(&(this->myStyle), theStyle);
	this->myString = g_string_new(theString);

	if (this->myString == NULL)
	  {
	    free(this);
	    this = NULL;
	  }
      }

    if (this)
      {
	SHOW5("Line=%d, First Col=%d, Last Col=%d, this->myParent=%x\n", this->myLine, this->myFirstCol, this->myLastCol, (unsigned int)this->myParent);
	SHOW2("String=\"%s\"\n", this->myString->str);
	DISPLAY_STYLE( &(this->myStyle));
      }

    return this;
}
/* > */
/* < copyLinePortion */
linePortion* copyLinePortion (linePortion* theSource)
{
  linePortion* this = NULL;
  
  ENTER("copyLinePortion");
  
  if (theSource)
    {
      this = createLinePortion (theSource->myLine, 
				theSource->myFirstCol, 
				&(theSource->myStyle), 
				theSource->myString->str, 
				theSource->myParent);
    }

  if (this)
    {
      SHOW5("Line=%d, First Col=%d, Last Col=%d, this->myParent=%x\n", this->myLine, this->myFirstCol, this->myLastCol, (unsigned int)this->myParent);
      SHOW2("String=\"%s\"\n", this->myString->str);
      DISPLAY_STYLE( &(this->myStyle));
    }
    return this;
}
/* > */
/* < deleteLinePortion */
void deleteLinePortion( linePortion* this)
{
    ENTER("deleteLinePortion");
    if (this)
      {
	g_string_free( this->myString, 1);
	SHOW3("g_string_free(%x, %d)", (unsigned int)this->myString, 1);
	free(this);
      }
}
/* > */
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

  if (w && *w)
    {
      (*w)->myWeight = 0; 
      (*w)->myStatus = WEIGHT_UNDEFINED; 
      (*w)->myLinePortion = (linePortion*)theLinePortion;
      SHOW2("str=%s\n",((linePortion*)theLinePortion)->myString->str);
      SHOW2("First Col=%d\n",((linePortion*)theLinePortion)->myFirstCol);
      SHOW2("Last Col=%d\n",((linePortion*)theLinePortion)->myLastCol);
      (*w)++;
    }
}

int getFeaturesLinePortionGroup( GList* this, linePortion* theFeatures)
{
  int aResultIsAvailable=0;
  int aCount = 0;

  ENTER("getFeaturesLinePortionGroup");

  if (this &&
      /* affectation */
      (aCount = g_list_length(this))
      && theFeatures)
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
	      if (compareStyle( &(aWeight[ j].myLinePortion->myStyle), &(aWeight[ k].myLinePortion->myStyle))==0)
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
      SHOW3("%x=g_string_new(%s)\n", (unsigned int)theFeatures->myString, getStringFromGList( this));

      for ( j=1; j < aCount; j++)
	{
	  g_string_append( theFeatures->myString, aWeight[ j].myLinePortion->myString->str);
	  SHOW3("g_string_append(%x, %s)", (unsigned int)theFeatures->myString, aWeight[ j].myLinePortion->myString->str);
	}
      /* > */

      theFeatures->myLine = aWeight[ 0].myLinePortion->myLine;
      theFeatures->myFirstCol = aWeight[ 0].myLinePortion->myFirstCol;
      theFeatures->myLastCol = aWeight[ aCount-1].myLinePortion->myLastCol;

      SHOW4("theFeatures, myLine=%d, myFirstCol=%d, myLastCol=%d\n", theFeatures->myLine, theFeatures->myFirstCol, theFeatures->myLastCol);

      DISPLAY_STYLE( &(theFeatures->myStyle));

      free(aWeight);

      aResultIsAvailable=1;
    }

  SHOW2("aResultIsAvailable=%d\n",aResultIsAvailable);

  return aResultIsAvailable;
}
/* > */

/* 
Local variables:
folded-file: t
folding-internal-margins: nil
*/
