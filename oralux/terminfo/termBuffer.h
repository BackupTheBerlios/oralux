#ifndef TERMBUFFER_H
#define TERMBUFFER_H

#include "escape2terminfo.h"

/* Current supported terminals */
enum termBufferName
  {
    LINUX,
    XTERM
  };

/* < cursor */

struct t_cursor
{
  int myLine;
  int myCol;
  struct t_style myStyle;
};

#define copyCursor( theDestination, theSource) memcpy( theDestination, theSource, sizeof(struct t_cursor))

/* > */
/* < t_termBuffer */

struct t_termBuffer
{
  char* myDataBuffer;
  struct t_style* myStyleBuffer;
  struct t_cursor myCursor;
  struct t_cursor mySavedCursor;
  int myNumberOfLine;
  int myNumberOfCol;
};

/* > */

struct t_termBuffer* createTermBuffer( enum termBufferName theName, int theNumberOfLine, int theNumberOfCol);
char* interpretEscapeSequence( struct t_termBuffer* theTermBuffer, FILE* theStream, char** theOutput);
void deleteTermBuffer( struct t_termBuffer* theTermBuffer);

#endif

