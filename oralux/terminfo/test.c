#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "termBuffer.h"

int main(int argc, char *argv[])
{
  FILE* fd=NULL;
  struct t_termBuffer* aTermBuffer=createTermBuffer( LINUX, 30, 30);
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

      sprintf(aAbsolutePath+aLength, "/test/%s", aTest);
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

      interpretEscapeSequence( aTermBuffer, fdtest, &aOutput);

      fclose(fdtest);
      if (aOutput)
	{
	  printf(">>>Output=%s<<<\n", aOutput);
	  free(aOutput);
	  aOutput=NULL;
	}
    }

  deleteTermBuffer( aTermBuffer);
  fclose(fd);
  free(aTest);
  free(aAbsolutePath);
  return 0;
}
