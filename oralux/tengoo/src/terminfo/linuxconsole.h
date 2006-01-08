#ifndef LINUXCONSOLE_H
#define LINUXCONSOLE_H 1
#include <glib.h>
#include "escape2terminfo.h"

int getDigit(char* theBuffer, int theLength);
void getStyle( struct t_style* theStyle, struct t_style* theDefaultStyle);
GString* getSGR( struct t_style* theStyle);

#endif
