#ifndef LINUXCONSOLE_H
#define LINUXCONSOLE_H 1
#include "escape2terminfo.h"

extern int getDigit(char* theBuffer, int theLength);
extern void getStyle( struct t_style* theStyle, struct t_style* theDefaultStyle);

#endif
