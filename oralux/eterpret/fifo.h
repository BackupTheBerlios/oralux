#ifndef FIFO_H
#define FIFO_H
// ----------------------------------------------------------------------------
// fifo.h
// $Id: fifo.h,v 1.2 2005/01/30 21:43:51 gcasse Exp $
// $Author: gcasse $
// Description: interprets the Emacspeak commands 
// $Date: 2005/01/30 21:43:51 $ |
// $Revision: 1.2 $ |
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


enum t_typeFifoElement
  {
    ElementIsString, //s [:rate $rate]
    ElementIsRate, //s [:rate $rate]
    ElementIsTone, //t
    ElementIsSoundFile, //a ensuite "\[:play $sound]"
    ElementIsNote, //n proc n, n {instrument note length {target 0} {step 5}
  };

struct t_fifoElement
{
  enum t_typeFifoElement myType;
  void* myValue;
};

void initFifo();

int pushStringToFifo( char* theString);
int pushRateToFifo( int theRate);

struct t_fifoElement* popFifo();

int purgeFifo();

#endif
