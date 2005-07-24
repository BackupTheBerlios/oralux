#ifndef LINEPORTION_H
#define LINEPORTION_H

#include <glib.h>
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
  GString* myString;
};
typedef struct t_linePortion linePortion;

linePortion* createLinePortion (int theLine, int theCol, style* theStyle, chartype* theString);
void deleteLinePortion( linePortion* this);

/* > */
/* < linePortionGroup */

/* a "LinePortionGroup" is a list grouping contiguous LinePortions (e.g. a label including a hotkey). 
*/
void deleteLinePortionGroup( GList* this);

#define getLineFromGList(a) (((linePortion*)(a->data))->myLine)
#define getFirstColFromGList(a) (((linePortion*)(a->data))->myFirstCol)
#define getLastColFromGList(a) (((linePortion*)(a->data))->myLastCol)
#define getStringFromGList(a) (((linePortion*)(a->data))->myString->str)
#define getStyleAddressFromGList(a) &(((linePortion*)(a->data))->myStyle)

int getFeaturesLinePortionGroup( GList* this, linePortion* theFeatures);

/* > */

#endif
