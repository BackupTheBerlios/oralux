// ----------------------------------------------------------------------------
// lliaphon-client.c
// $Id: lliaphon-client.c,v 1.2 2006/02/11 22:39:48 gcasse Exp $
// $Author: gcasse $
// Description: lliaphon client. 
// $Date: 2006/02/11 22:39:48 $ |
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include "lliaphond.h"

#include <errno.h>

static void client( int fd[2])
{
  #define SIZE_BUF 2048
  char* aPhoneme; 
  char* aText; 
  int aMaxPhoneme=SIZE_BUF;
  int aPhonemeIndex=0;
  int i=0;
  
  aPhoneme = malloc( aMaxPhoneme=SIZE_BUF);
  aPhonemeIndex = 0;
  aText = malloc( SIZE_BUF);

  /* clean the output fifo */
  while((i=read( fd[1], aPhoneme, SIZE_BUF))
	&& (i != -1))
    {}

  while(i=read( 0, aText, SIZE_BUF-1))
    {
      int nextBlock;
      int readBytes=0;

      if (i<0)
	{
	  break;
	}

      aText[i]=0;
      if (write( fd[0], aText, i) == -1)
	{
	  break;
	}

      // Read the block(s) of phonemes from the output fifo.
      // We suppose that the blocks do not have a 50 ms delay 
      // between them (patch lliaphon: util.c).
      nextBlock = 0;
      readBytes = aMaxPhoneme - 1;
      while(i = read( fd[1], aPhoneme+aPhonemeIndex, readBytes))
	{
	  if(i == -1)
	    {
	      if ((errno == EAGAIN) && !nextBlock) 
		{
		  (void) usleep(10000);
		  continue;
		}
	      break;
	    }
	  
	  aPhonemeIndex += i;
	  aPhoneme[ aPhonemeIndex]=0;

	  /* SIZE_BUF/4 could be adjusted as required */
	  if (aPhonemeIndex + SIZE_BUF/4 > aMaxPhoneme)
	    {
	      char* aNewBuffer = NULL;
	      aMaxPhoneme += SIZE_BUF;
	      aNewBuffer = malloc( aMaxPhoneme);
	      memcpy( aNewBuffer, aPhoneme, aPhonemeIndex);
	      free(aPhoneme);
	      aPhoneme = aNewBuffer;
	    }

	  readBytes = aMaxPhoneme - aPhonemeIndex -1;
	  nextBlock = 1;
	}

      if(write( 1, aPhoneme, aPhonemeIndex) == -1)
	{
	  break;
	}
    }
  free(aPhoneme);
  free(aText);
}

int main(int argc, char *argv[])
{
  int i=0;
  int fd[2];
  char * array[2];
  char* aHome = getenv("HOME");
  char* aUser = getenv("USER");
  char* aFifoName = malloc( strlen(myFifoInName) + strlen(myFifoOutName) + strlen( aHome) + strlen(aUser));

  array[0] = myFifoInName; /* i = STDIN_FILENO */
  array[1] = myFifoOutName; /* i = STDOUT_FILENO */
  fd[0] = 0;
  fd[1] = 0;

  for (i=0; i<2; i++)
    {
      int flags;
      struct stat aFileInfo;

      sprintf( aFifoName, "%s/%s.%s", aHome, array[i], aUser);

      if (stat( aFifoName, &aFileInfo) == -1)
	{
	  perror("stat");
	  goto exit_main;
	}

      fd[i] = open( aFifoName, O_RDWR);
      if (fd[i] == -1)
	{
	  perror("open");
	  goto exit_main;
	}

      flags = fcntl( fd[i], F_GETFL);
      if (flags == -1)
	{
	  perror("fcntl");
	  goto exit_main;
	}

      flags = fcntl( fd[i], F_SETFL, flags | O_NONBLOCK);
      if (flags == -1)
	{
	  perror("fcntl");
	  goto exit_main;
	}
    }

  free( aFifoName);
  aFifoName = NULL;

  client( fd);

 exit_main:
  free( aFifoName);
  exit(0);
}
