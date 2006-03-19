#ifndef KEYBOARD
#define KEYBOARD 1

#include "constants.h"
#include "i18n.h"

enum keyboard {
  americanKeyboard,  // Warning: same order as in enum sentence (audiointerface.h)
  belgianKeyboard,
  brazilianKeyboard,
  britishKeyboard,
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
  slovakKeyboard,
  spanishKeyboard,
  swissKeyboard,
  turkishKeyboard,
  MaxKeyboard,

  FirstCyrillicKeyboard=MaxKeyboard,
  belarusianKeyboard=FirstCyrillicKeyboard,
  bulgarian_BDS_Keyboard,
  bulgarianPhoneticKeyboard,
  kazakhAltKeyboard,
  kazakhGostKeyboard,
  macedonianKeyboard,
  mongolianKeyboard,
  russianKeyboard,
  russianWinkeysKeyboard,
  serbianKeyboard,
  ukrainianKeyboard,
  ukrainianWinkeysKeyboard,
  MaxCyrillicKeyboard,
};

struct keyboardStruct {
  int myStickyKeysAreAvailable;
  int myRepeatKeysAreAvailable;
};

#endif

