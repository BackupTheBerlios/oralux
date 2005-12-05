#ifndef KEYBOARD
#define KEYBOARD 1

#include "constants.h"
#include "i18n.h"

enum keyboard {
  americanKeyboard,  // Warning: same order as in enum sentence (audiointerface.h)
  belgianKeyboard,
  brazilianKeyboard,
  britishKeyboard,
  bulgarianKeyboard,
/*   chineseKeyboardSimplified,  */
/*   chineseKeyboardTraditionnal,  */
  czechKeyboard,
  danskKeyboard,
  finnishKeyboard,
  frenchKeyboard,
  frenchcanadianKeyboard,
  germanKeyboard,
/*   hebrewKeyboard, */
  italianKeyboard, 
/*   japaneseKeyboard, */
/*   netherlandKeyboard, */
  polishKeyboard,
  russianKeyboard,
  slovakKeyboard,
  spanishKeyboard,
  swissKeyboard,
  turkishKeyboard,
  MaxKeyboard,
};

struct keyboardStruct {
  int myStickyKeysAreAvailable;
  int myRepeatKeysAreAvailable;
};

#endif

