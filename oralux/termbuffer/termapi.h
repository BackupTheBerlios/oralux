#ifndef TERMAPI_H
#define TERMAPI_H

#include "escape2terminfo.h"

/* < linePortion */
/* 
A line portion describes an horizontal area in the screen.
When a menu is browsed, the selected item and the previously ones are displayed with distinct styles (for example distinct backgrounds).
Two line portions must be particularly managed to distinguish the selected one.
*/
typedef char chartype; /* TBD: add support for multi-bytes characters */

struct t_linePortion
{
  int myLine; /* Line number */
  int myFirstCol; /* First column of the portion */
  int myLastCol; /* Last column of the portion */
  style myStyle;
  chartype* myString;
};
typedef struct t_linePortion linePortion;

void createLinePortion (linePortion* theLinePortion, int theLine, int theCol, int theLastCol, style* theStyle, chartype* theString);
/* > */
/* < cursor */
struct t_cursor
{
  int myLine;
  int myCol;
  style myStyle;
};
typedef struct t_cursor cursor;

#define copyCursor( theDestination, theSource) memcpy( theDestination, theSource, sizeof(struct t_cursor))
/* > */

typedef int (*IS_LINE_CONTENT_MODIFIED_HANDLER)( linePortion theLinePortion, const chartype* theNewData);
typedef int (*IS_LINE_STYLE_MODIFIED_HANDLER)( linePortion theLinePortion, const style* theNewStyle);
typedef int (*GET_BACKGROUND_STYLE_HANDLER)( linePortion theLinePortion, style* theStyle);
typedef int (*GET_CURSOR_HANDLER)( cursor* theCursor);
typedef int (*GET_DIM_HANDLER)( int* theNumberOfLine, int* theNumberOfCol);

struct t_termapi
{
  IS_LINE_CONTENT_MODIFIED_HANDLER isLineContentModified;
  IS_LINE_STYLE_MODIFIED_HANDLER isLineStyleModified;
  GET_BACKGROUND_STYLE_HANDLER getBackgroundStyle;
  GET_CURSOR_HANDLER getCursor;
  GET_DIM_HANDLER getDim;
};
typedef struct t_termapi termapi;

#endif
