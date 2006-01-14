#ifndef DEBUG_H
#define DEBUG_H

#include "escape2terminfo.h"

void displayRawBuffer( unsigned char* theBuffer, int theSize);

#ifdef DEBUG
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
/* #define OUTPUT_FILENAME "stdout" */
#define OUTPUT_FILENAME "/tmp/tengoo"

extern FILE* TheOutputStream;
void debugBegin();
void debugEnd();

#define DEBUG_BEGIN debugBegin();
#define DEBUG_END debugEnd();

#define ENTER(a) \
fprintf(TheOutputStream,"%s: == ENTER %s (%d)\n",__FILE__, a,__LINE__)

#define SHOW(a) fprintf(TheOutputStream,"%s: " "%s\n",__FILE__,a)
#define SHOWn(s,n) fprintf(TheOutputStream,"%*s\n",n,s)
#define SHOW1(a) fprintf(TheOutputStream,"%s: " "%s\n",__FILE__,a)
#define SHOW2(a,b) fprintf(TheOutputStream,"%s: " a,__FILE__,b)
#define SHOW3(a,b,c) fprintf(TheOutputStream,"%s: " a,__FILE__,b,c)
#define SHOW4(a,b,c,d) fprintf(TheOutputStream,"%s: " a,__FILE__,b,c,d)
#define SHOW5(a,b,c,d,e) fprintf(TheOutputStream,"%s: " a,__FILE__,b,c,d,e)
#define LEAVE(a) fprintf(TheOutputStream,"LEAVE %s\n",a)
#define LEAVE2(a,b) fprintf(TheOutputStream,"LEAVE %s (%d)\n",a,b)

void displayColor( char* theLabel, int theColor, char* theFile);
void displayTree( void* theNode);
void displayStyle(struct t_style* theStyle, char* theFile);
void displayCapacity( enum StringCapacity theCapacity, char* theFile);
void displayBuffer( char *theDataBuffer, struct t_style* theStyleBuffer, int theMaxLine, int theMaxCol);

#define DISPLAY_COLOR(a,b) displayColor(a,b,__FILE__)
#define DISPLAY_STYLE(a) displayStyle(a,__FILE__)
#define DISPLAY_CAPACITY(a) displayCapacity(a,__FILE__)
#define DISPLAY_BUFFER(a,b,c,d) displayBuffer(a,b,c,d)
#define DISPLAY_RAW_BUFFER(buf, size) displayRawBuffer(buf, size)
#define DISPLAY_TREE(a) displayTree(a)


#define SHOW_TIME(a) \
{\
  struct timeval tv;\
  gettimeofday( &tv, NULL);\
  fprintf(TheOutputStream,"%s: " "%s > time: %d.%d\n", __FILE__, a, (int)(tv.tv_sec)%10, (int)tv.tv_usec);\
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
#define DISPLAY_TREE(a)
#define DISPLAY_STYLE(a)
#define DISPLAY_CAPACITY(a)
#define DISPLAY_BUFFER(a,b,c,d)
#define SHOW_TIME(a)
/* #define DISPLAY_RAW_BUFFER(buf, size) */
#define DISPLAY_RAW_BUFFER(buf, size) displayRawBuffer(buf, size)

#endif /* DEBUG */

#endif /* DEBUG_H */
