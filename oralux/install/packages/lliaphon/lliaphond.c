// ----------------------------------------------------------------------------
// lliaphond.c
// $Id: lliaphond.c,v 1.1 2005/12/18 23:36:31 gcasse Exp $
// $Author: gcasse $
// Description: lliaphon daemon. 
// $Date: 2005/12/18 23:36:31 $ |
// $Revision: 1.1 $ |
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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "lliaphond.h"

int main(int argc, char *argv[])
{
  int i=0;
  int fd[2];
  char * array[2];
  char* aHome = getenv("HOME");
  char* aFifoName = malloc( strlen(myFifoInName) + strlen(myFifoOutName) + strlen( aHome));

  array[0] = myFifoInName; /* i = STDIN_FILENO */
  array[1] = myFifoOutName; /* i = STDOUT_FILENO */
  fd[0] = 0;
  fd[1] = 0;

  for (i=0; i<2; i++)
    {
      int flags;
      struct stat aFileInfo;

      sprintf( aFifoName, "%s/%s", aHome, array[i]);

      if ((stat( aFifoName, &aFileInfo) == -1)
	  && (mkfifo( aFifoName, 0600) == -1))
	{
	  perror("mkfifo");
	  goto exit_ko;
	}

      fd[i] = open( aFifoName, O_RDWR);
      if (fd[i] == -1)
	{
	  perror("open");
	  goto exit_ko;
	}

      /* i = STDIN_FILENO or STDOUT_FILENO */
      if (dup2( fd[i], i) == -1)
	{
	  perror("dup2");
	  goto exit_ko;
	}

      flags = fcntl( fd[i], F_GETFL);
      if (flags == -1)
	{
	  perror("fcntl");
	  goto exit_ko;
	}

      flags = fcntl( fd[i], F_SETFL, flags | O_NONBLOCK);
      if (flags == -1)
	{
	  perror("fcntl");
	  goto exit_ko;
	}
    }

  free( aFifoName);
  aFifoName = NULL;

  array[0] = myBinary;
  array[1] = NULL;
  execv( array[0], array); 
  perror("execv");

 exit_ko:
  free( aFifoName);
  exit(1);
}
