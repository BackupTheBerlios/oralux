// ----------------------------------------------------------------------------
// fifo.c
// $Id: fifo.c,v 1.1 2004/09/27 20:29:58 gcasse Exp $
// $Author: gcasse $
// Description: fifo
// $Date: 2004/09/27 20:29:58 $ |
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
#include <stdlib.h>
#include <string.h>
#include "fifo.h"
#include "helpEmul.h"

/* {{{ data */
static int myFifoReadIndex=0;
static int myFifoWriteIndex=0;
#define MAXFIFO 50
struct t_fifoElement myFifo[ MAXFIFO ];

/* }}} */
/* {{{ initFifo */
void initFifo()
{
  ENTER("initFifo");
  int i=0;
  for (i=0; i<MAXFIFO; i++)
    {
      myFifo[ i].myValue=NULL;
    }
  myFifoWriteIndex=0;
  myFifoReadIndex=0;
}
/* }}} */
/* {{{ pushToFifo */

static int pushToFifo( enum t_typeFifoElement theType, void* theValue)
{
  ENTER("pushToFifo");
  int aStatus=0;
  if (myFifoReadIndex!=0)
    {
      SHOW("FATAL ERROR: myFifoReadIndex!=0");
      exit(1);
    }

  if (myFifoWriteIndex < MAXFIFO)
    {
      myFifo[ myFifoWriteIndex].myType = theType;
      myFifo[ myFifoWriteIndex].myValue = theValue;
      myFifoWriteIndex++;
      aStatus=1;
    }
  else
    {
      SHOW("myFifoWriteIndex >= MAXFIFO!");
    }
  return aStatus;
}

int pushStringToFifo( char* theString)
{
  ENTER("pushStringToFifo");
  SHOW2("theString=%s=\n", theString);
  return pushToFifo( ElementIsString, (void*) strdup(theString));
}

int pushRateToFifo( int theRate)
{
  ENTER("pushStringToFifo");
  SHOW2("theRate=%d=\n", theRate);
  int * aRate=malloc(sizeof(int));
  *aRate=theRate;
  return pushToFifo( ElementIsRate, (void*) aRate);
}

/* }}} */
/* {{{ purgeFifo */

int purgeFifo()
{
  ENTER("purgeFifo");
  int i=0;
  for (i=0; i<myFifoWriteIndex; i++)
    {
      free(myFifo[ i].myValue);
    }
  myFifoReadIndex=0;
  myFifoWriteIndex=0;
  return 1;
}
/* }}} */
/* {{{ popFifo */
struct t_fifoElement* popFifo()
{
  ENTER("popFifo");
  void* anElement=NULL;

  if (myFifoReadIndex < myFifoWriteIndex)
    {
      anElement= myFifo + myFifoReadIndex;
      SHOW2("myFifoReadIndex=%d\n", myFifoReadIndex);
      myFifoReadIndex++;
    }
  else
    {
      purgeFifo();
    }
  return anElement;
}

/* }}} */
