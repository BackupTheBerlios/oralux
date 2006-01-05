#ifndef TERMAPI_H
#define TERMAPI_H

#include <glib.h>
#include "lineportion.h"

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

/* < termapi type */

typedef int (*GET_COLOR_HANDLER)( int theLine, int theFirstCol, int theLastCol, enum terminalColor* theColor);

typedef GList* (*GET_GROUP_HANDLER)( int theLine, int theFirstCol, int theLastCol);
typedef int (*GET_CURSOR_HANDLER)( cursor* theCursor);
typedef int (*GET_DIM_HANDLER)( int* theNumberOfLine, int* theNumberOfCol);
typedef int (*COUNT_TEXT)( cursor* thePreviousCursor, cursor* theCursor);
typedef int (*GET_TEXT)( cursor* theCursor);


struct t_termAPI
{
  GET_COLOR_HANDLER getBackground;
  GET_COLOR_HANDLER getForeground;
  GET_GROUP_HANDLER getLinePortionGroup;
  GET_CURSOR_HANDLER getCursor;
  GET_DIM_HANDLER getDim;
  COUNT_TEXT countChar;
  COUNT_TEXT countWord;
  GET_TEXT getChar;
  GET_TEXT getWord;
  GET_TEXT getLine;
};
typedef struct t_termAPI termAPI;

/* > */

termAPI* createTermAPI();
void deleteTermAPI( termAPI*);

#endif
