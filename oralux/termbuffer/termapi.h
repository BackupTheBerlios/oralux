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
};
typedef struct t_linePortion linePortion;
/* > */

typedef int (*isLineContentModifiedHandler)( linePortion theLinePortion, const chartype* theNewData);
typedef int (*isLineStyleModifiedHandler)( linePortion theLinePortion, const style* theNewStyle);
typedef int (*getBackgroundStyleHandler)( linePortion theLinePortion, style* theStyle);

struct t_termapi
{
  isLineContentModifiedHandler isLineContentModified;
  isLineStyleModifiedHandler isLineStyleModified;
  getBackgroundStyleHandler getBackgroundStyle;
};
typedef struct t_termapi termapi;

#endif
