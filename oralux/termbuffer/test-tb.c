/* 
----------------------------------------------------------------------------
test-tb.c
$Id: test-tb.c,v 1.3 2005/01/30 21:43:51 gcasse Exp $
$Author: gcasse $
Description: test termnbuffer.
$Date: 2005/01/30 21:43:51 $ |
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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "termbuffer.h"

int main(int argc, char *argv[])
{
  FILE* fd=NULL;
  struct t_termbuffer* aTermbuffer=createTermbuffer( LINUX, 48, 128);
  char* aOutput=NULL;
  enum {MAX_LINE=400};
  char* aTest=(char*)malloc(MAX_LINE);
  char* aAbsolutePath=malloc(MAX_LINE);
  int aLength=0;

  getcwd (aAbsolutePath, MAX_LINE);
  aLength=strlen(aAbsolutePath);

  fd=fopen(argv[1],"r");
  if (fd==NULL)
    {
      printf("File not found: %s\n",argv[1]);
      return 1;
    }
  
  while (fgets(aTest, MAX_LINE, fd))
    {
      FILE* fdtest=NULL;
      if ((*aTest=='#')||(*aTest=='\n'))
	{
	  continue;
	}

      sprintf(aAbsolutePath+aLength, "/testcases/%s", aTest);
      aAbsolutePath[ strlen(aAbsolutePath)-1]=0; /* no final \n */

      fdtest=fopen(aAbsolutePath,"r");
      if (fdtest==NULL)
	{
	  printf("File not found: %s\n",aTest);
	  return 1;
	}
      else
	{
	  printf("*** Test: %s\n",aTest);
	}

      interpretEscapeSequence( aTermbuffer, fdtest, &aOutput);

      fclose(fdtest);
      if (aOutput)
	{
	  printf(">>>Output=%s<<<\n", aOutput);
	  free(aOutput);
	  aOutput=NULL;
	}
    }

  deleteTermbuffer( aTermbuffer);
  fclose(fd);
  free(aTest);
  free(aAbsolutePath);
  return 0;
}
