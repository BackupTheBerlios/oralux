// ----------------------------------------------------------------------------
// eterpret.c
// $Id: eterpret.c,v 1.2 2005/01/30 21:43:51 gcasse Exp $
// $Author: gcasse $
// Description: Interprets the Emacspeak commands and calls a generic voice synthesizer API.
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
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>
#undef DEBUG
#include "helpEmul.h"
#include "eparser.h"

//#include <sys/wait.h>
//#include <sys/stat.h>
//#include <sys/ioctl.h>

extern void eparser();


#define MAXBUF 256
/* {{{  main */

int main(int argc, char *argv[])
{
  ENTER("main");
  static char myLine[MAXBUF]; // TBD GC (malloc)

  HELPEMULINIT;

  initParser();
  int a=1;
  do 
    {
      a=read(STDIN_FILENO, myLine, MAXBUF);

      if (a==0)
	{
	  break;
	}
      myLine[a]=0;

      SHOW4("%3.3f > %.*s\n", timeDiff(&myOriginTime), a, myLine);

      eparser(myLine);

    } while(1);

  stopParser();

  return 1;
}

/* }}} */

