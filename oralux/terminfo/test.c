#include <stdio.h>
#include <stdlib.h>
#include "termBuffer.h"

int main()
{
  enum {MAXSIZE=200};
  char* aString=(char*)malloc(MAXSIZE);
  FILE* fd=fopen("test/1e.txt","r");
  void* aTermBuffer=createTermBuffer( LINUX, 30, 30);
  char* aOutput=NULL;


  interpretEscapeSequence( aTermBuffer, fd, &aOutput);

  printf(">>>Output=%s<<<\n", aOutput);
  free(aOutput);

  deleteTermBuffer( aTermBuffer);
  free(aString);
  return 0;
}
