#ifndef TERMAPI_H
#define TERMAPI_H

/* < terminalColor */
enum terminalColor 
  {
    TERM_COLOR_BLACK=0,
    TERM_COLOR_RED=1,
    TERM_COLOR_GREEN=2,
    TERM_COLOR_YELLOW=3,
    TERM_COLOR_BLUE=4,
    TERM_COLOR_MAGENTA=5,
    TERM_COLOR_CYAN=6,
    TERM_COLOR_WHITE=7
  };
typedef enum terminalColor terminalColor;
/* > */
/* < style */
struct t_style 
{
  unsigned int myBackgroundColor : 3;
  unsigned int myForegroundColor : 4;
  unsigned int isStandout : 1;
  unsigned int isUnderline : 1;
  unsigned int isReverse : 1;
  unsigned int isBlink : 1;
  unsigned int isDim : 1;
  unsigned int isBold : 1;
  unsigned int isBlank : 1;
  unsigned int isProtect : 1;
  unsigned int isAlternate : 1;
};
typedef struct t_style style;
/* > */
/* < cell */
struct t_cell
{
  int myLine;
  int myCol;
};
typedef struct t_cell cell;
/* > */
/* < cursor */
struct t_cursor
{
  cell myCell;
  style myStyle;
};
typedef struct t_cursor cursor;
/* > */
/* < line */
struct t_cursor
{
  cell myCell;
  style myStyle;
};
typedef struct t_cursor cursor;
/* > */

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

typedef int isLineContentModifiedHandler( linePortion theLinePortion, const chartype* theNewData);
typedef int isLineStyleModifiedHandler( linePortion theLinePortion, const style* theNewStyle);
typedef int getBackgroundStyleHandler( linePortion theLinePortion, style* theStyle);

struct termapi

struct t_termapi
{
  isLineContentModifiedHandler isLineContentModified;
  isLineStyleModifiedHandler isLineStyleModified;
  getBackgroundStyleHandler getBackgroundStyle;
};
typedef struct t_termapi termapi;

#endif
