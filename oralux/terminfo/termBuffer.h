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
/* < linePortion */
/* 
A line portion describes an horizontal area in the screen.
When a menu is browsed, the selected item and the previously ones are displayed with distinct styles (for example distinct backgrounds).
These two line portions must be particularly managed to distinguish the selected one.
*/
struct t_linePortion
{
  int myLine; /* Line number */
  int myFirstCol; /* First column of the portion */
  int myLastCol; /* Last column of the portion */
  int myContentIsModified; /* equals 1 if at least one char has been modified */
  int myStyleIsModified; /* equals 1 if one of the style has been modified */
};

enum {MAX_LINE_PORTION=2}; /* 2 line portions are expected to distinguish the selected item */
/* > */
/* < t_termBuffer */
struct t_termBuffer
{
  char* myDataBuffer;
  struct t_style* myStyleBuffer;
  struct t_style myDefaultStyle;
  struct t_cursor myCursor;
  struct t_cursor mySavedCursor;
  int myNumberOfLine;
  int myNumberOfCol;
  struct t_linePortion myLinePortion[ MAX_LINE_PORTION];
  int myLinePortionIndex;
};
/* > */

struct t_termBuffer* createTermBuffer( enum termBufferName theName, int theNumberOfLine, int theNumberOfCol);
char* interpretEscapeSequence( struct t_termBuffer* this, FILE* theStream, char** theOutput);
void deleteTermBuffer( struct t_termBuffer* this);

#endif

