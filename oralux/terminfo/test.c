#include <stdio.h>
#include <stdlib.h>
#include "termBuffer.h"

int main(int argc, char *argv[])
{
  FILE* fd=NULL;
  struct t_termBuffer* aTermBuffer=createTermBuffer( LINUX, 30, 30);
  char* aOutput=NULL;

  fd=fopen("test/highlightedItem/1.txt","r");
  interpretEscapeSequence( aTermBuffer, fd, &aOutput);
  if (aOutput)
    {
      printf(">>>Output=%s<<<\n", aOutput);
      free(aOutput);
      aOutput=NULL;
    }
  fclose(fd);

  fd=fopen("test/highlightedItem/2.txt","r");
  interpretEscapeSequence( aTermBuffer, fd, &aOutput);
  if (aOutput)
    {
      printf(">>>Output=%s<<<\n", aOutput);
      free(aOutput);
      aOutput=NULL;
    }
  fclose(fd);

  fd=fopen("test/highlightedItem/3.txt","r");
  interpretEscapeSequence( aTermBuffer, fd, &aOutput);
  if (aOutput)
    {
      printf(">>>Output=%s<<<\n", aOutput);
      free(aOutput);
      aOutput=NULL;
    }
  fclose(fd);

  deleteTermBuffer( aTermBuffer);
  return 0;
}
