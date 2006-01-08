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

struct t_termAPI
{
  GET_COLOR_HANDLER getBackground;
  GET_COLOR_HANDLER getForeground;
  GET_GROUP_HANDLER getLinePortionGroup;
  GET_CURSOR_HANDLER getCursor;
  GET_DIM_HANDLER getDim;
};
typedef struct t_termAPI termAPI;

/* > */

termAPI* createTermAPI();
void deleteTermAPI( termAPI*);

#endif
