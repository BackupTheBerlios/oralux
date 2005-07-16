#ifndef TERMINFOINTERPRETER_H
#define  TERMINFOINTERPRETER_H 1
#include "terminfo2list.h"

void terminfointerpreter_init( cursor* theCursor);
void terminfointerpreter( gpointer theEntry, gpointer userData);
cursor* terminfointerpreter_getCursor();

#endif

