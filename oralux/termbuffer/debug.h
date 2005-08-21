#ifndef DEBUG_H
#define DEBUG_H

#include "escape2terminfo.h"

void displayRawBuffer( unsigned char* theBuffer, int theSize);

#ifdef DEBUG
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
static char* TheENTERFilename="  ";
/* #define OUTPUT_FILENAME "stdout" */
#define OUTPUT_FILENAME "/tmp/tengoo"

extern FILE* TheOutputStream;
void debugBegin();
void debugEnd();

#define DEBUG_BEGIN debugBegin();
#define DEBUG_END debugEnd();

#define ENTER(a) \
if (strcmp(TheENTERFilename,__FILE__)!=0) \
{\
  TheENTERFilename=__FILE__;\
  fprintf(TheOutputStream,"\nFILE %s\n",__FILE__);\
}\
fprintf(TheOutputStream,"== ENTER %s (%d)\n",a,__LINE__)

#define SHOW(a) fprintf(TheOutputStream,"%s\n",a)
#define SHOWn(s,n) fprintf(TheOutputStream,"%*s\n",n,s)
#define SHOW1(a) fprintf(TheOutputStream,"%s\n",a)
#define SHOW2(a,b) fprintf(TheOutputStream,a,b)
#define SHOW3(a,b,c) fprintf(TheOutputStream,a,b,c)
#define SHOW4(a,b,c,d) fprintf(TheOutputStream,a,b,c,d)
#define SHOW5(a,b,c,d,e) fprintf(TheOutputStream,a,b,c,d,e)
#define LEAVE(a) fprintf(TheOutputStream,"LEAVE %s\n",a)
#define LEAVE2(a,b) fprintf(TheOutputStream,"LEAVE %s (%d)\n",a,b)

void displayColor( char* theLabel, int theColor);
void displayStyle(struct t_style* theStyle);
void displayCapacity( enum StringCapacity theCapacity);
void displayBuffer( char *theDataBuffer, struct t_style* theStyleBuffer, int theMaxLine, int theMaxCol);

#define DISPLAY_COLOR(a,b) displayColor(a,b)
#define DISPLAY_STYLE(a) displayStyle(a)
#define DISPLAY_CAPACITY(a) displayCapacity(a)
#define DISPLAY_BUFFER(a,b,c,d) displayBuffer(a,b,c,d)
#define DISPLAY_RAW_BUFFER(buf, size) displayRawBuffer(buf, size)


#define SHOW_TIME(a) \
{\
  struct timeval tv;\
  gettimeofday( &tv, NULL);\
  fprintf(TheOutputStream,"%s > time: %d.%d\n", a, (int)(tv.tv_sec)%10, (int)tv.tv_usec);\
}

#else /* DEBUG */

#define DEBUG_BEGIN 
#define DEBUG_END 
#define ENTER(a)
#define SHOW(a)
#define SHOW1(a)
#define SHOW2(a,b)
#define SHOW3(a,b,c)
#define SHOW4(a,b,c,d)
#define SHOW5(a,b,c,d,e)
#define LEAVE(a)
#define LEAVE2(a,b)
#define DISPLAY_COLOR(a,b)
#define DISPLAY_STYLE(a)
#define DISPLAY_CAPACITY(a)
#define DISPLAY_BUFFER(a,b,c,d)
#define SHOW_TIME(a)
/* #define DISPLAY_RAW_BUFFER(buf, size) */
#define DISPLAY_RAW_BUFFER(buf, size) displayRawBuffer(buf, size)

#endif /* DEBUG */

#endif /* DEBUG_H */
