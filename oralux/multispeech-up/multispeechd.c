// ----------------------------------------------------------------------------
// multispeechd.c
// $Id: multispeechd.c,v 1.1 2006/04/17 09:11:43 gcasse Exp $
// $Author: gcasse $
// Description: multispeech daemon. 
// $Date: 2006/04/17 09:11:43 $ |
// $Revision: 1.1 $ |
// Copyright (C) 2006 Gilles Casse <gcasse@oralux.org>
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
#include "multispeechd.h"

int main(int argc, char *argv[])
{
  int fd=0;
  int flags;
  struct stat aFileInfo;

  if ((stat( myFifoInName, &aFileInfo) == -1)
      && (mkfifo( myFifoInName, 0600) == -1))
    {
      perror("mkfifo");
      goto exit_main;
    }

  fd = open( myFifoInName, O_RDWR);
  if (fd == -1)
    {
      perror("open");
      goto exit_main;
    }

  /* i = STDIN_FILENO or STDOUT_FILENO */
  if (dup2( fd, 0) == -1)
    {
      perror("dup2");
      goto exit_main;
    }

  {
    char * array[2];
    array[0] = myBinary;
    array[1] = NULL;
    execv( array[0], array); 
    perror("execv");
  }

 exit_main:
  exit(1);
}
