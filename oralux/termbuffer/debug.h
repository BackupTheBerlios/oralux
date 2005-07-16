#ifndef DEBUG_H
#define DEBUG_H

#include "escape2terminfo.h"

#ifdef DEBUG
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
static char* TheENTERFilename="  ";

#define ENTER(a) \
if (strcmp(TheENTERFilename,__FILE__)!=0) \
{\
  TheENTERFilename=__FILE__;\
  printf("\nFILE %s\n",__FILE__);\
}\
printf("== ENTER %s (%d)\n",a,__LINE__)

#define SHOW(a) printf("%s\n",a)
#define SHOW1(a) printf("%s\n",a)
#define SHOW2(a,b) printf(a,b)
#define SHOW3(a,b,c) printf(a,b,c)
#define LEAVE(a) printf("LEAVE %s\n",a)
#define LEAVE2(a,b) printf("LEAVE %s (%d)\n",a,b)

void displayColor( char* theLabel, int theColor);
void displayStyle(struct t_style* theStyle);
void displayCapacity( enum StringCapacity theCapacity);
void displayBuffer( char *theDataBuffer, struct t_style* theStyleBuffer, int theMaxLine, int theMaxCol);

#define DISPLAY_COLOR(a,b) displayColor(a,b)
#define DISPLAY_STYLE(a) displayStyle(a)
#define DISPLAY_CAPACITY(a) displayCapacity(a)
#define DISPLAY_BUFFER(a,b,c,d) displayBuffer(a,b,c,d)

#define SHOW_TIME(a) \
{\
  struct timeval tv;\
  gettimeofday( &tv, NULL);\
  printf("%s > time: %d.%d\n", a, (int)(tv.tv_sec)%10, (int)tv.tv_usec);\
}

#else /* DEBUG */

#define ENTER(a)
#define SHOW(a)
#define SHOW1(a)
#define SHOW2(a,b)
#define SHOW3(a,b,c)
#define LEAVE(a)
#define LEAVE2(a,b)
#define DISPLAY_COLOR(a,b)
#define DISPLAY_STYLE(a)
#define DISPLAY_CAPACITY(a)
#define DISPLAY_BUFFER(a,b,c,d)
#define SHOW_TIME(a)

#endif /* DEBUG */

#endif /* DEBUG_H */
