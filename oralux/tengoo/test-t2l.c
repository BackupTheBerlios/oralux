/* 
----------------------------------------------------------------------------
test-tb.c
$Id: test-t2l.c,v 1.1 2005/09/08 18:42:53 gcasse Exp $
$Author: gcasse $
Description: test terminfo2list.
$Date: 2005/09/08 18:42:53 $ |
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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "terminfo2list.h"
#include "terminfointerpreter.h"
#include "tifilter2l.h"
#include "debug.h"


/* static void displayList(gpointer theEntry, gpointer userData) */
/* { */
/*   DISPLAY_CAPACITY( ((terminfoEntry*)theEntry)->myCapacity); */
/* } */

int main(int argc, char *argv[])
{
  FILE* fd=NULL;
  enum {MAX_LINE=400};
  char* aTest=(char*)malloc(MAX_LINE);
  char* aAbsolutePath=malloc(MAX_LINE);
  int aLength=0;
  cursor aCursor;
  termAPI* aTermAPI=createTermAPI();
  GList* aList=NULL;

  aTermAPI->getCursor( &aCursor);

  getcwd (aAbsolutePath, MAX_LINE);
  aLength=strlen(aAbsolutePath);

  fd=fopen(argv[1],"r");
  if (fd==NULL)
    {
      printf("File not found: %s\n",argv[1]);
      return 1;
    }
  
  terminfointerpreter_init( &aCursor);

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

      SHOW_TIME("convertTerminfo2List");

      aList = convertTerminfo2List( fdtest);
      fclose(fdtest);

      SHOW_TIME("terminfointerpreter");
      g_list_foreach(aList, (GFunc)terminfointerpreter, NULL);

      SHOW_TIME("terminfofilter2lines");
      aList = terminfofilter2lines( aList, aTermAPI, 0);

      SHOW_TIME("deleteList");
      deleteTermInfoList( aList);

      SHOW_TIME("The end");
    }

  deleteTermAPI( aTermAPI);

  fclose(fd);
  free(aTest);
  free(aAbsolutePath);
  return 0;
}
