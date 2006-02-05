#ifndef MENU
#define MENU 1

#include "constants.h"
#include "i18n.h"
#include "desktop.h"

struct menuInfo{
  enum language myMenuLanguage;
  enum keyboard myKeyboard;
  struct keyboardStruct myKeyboardFeatures;
  struct textToSpeechStruct myTextToSpeech;
  enum desktopIdentifier myDesktop;
  int myUserConfIsKnown; /* 0 if default conf; 1 otherwise */
};

// menu
// Start the introductory menu.
// Update the structure theSelectedInfo
void menu(struct menuInfo* theSelectedInfo, int *theConfHasBeenUpdated);

// getAnswer
// A short function which asks to the user to accept or not an assertion
// If the Return key is pressed, it means that the assertion is accepted.
// The Up or down arrow key means the user wants to jump to the previous or next choice.
// Any other key: means the assertion is not accepted.
//
enum MenuAnswer {
  MENU_Yes,
  MENU_No,
  MENU_Previous,
  MENU_Next,
};

enum MenuAnswer getAnswer();

// keyPressedCallback3: same than keyPressedCallback
// The sentences are cleared if the key is distinct of a return key
void keyPressedCallback3(char* theKeyPressed);

// Shutdown stage

enum ShutdownStatus {
  StatusUndefined,
  StatusReboot,
  StatusEjectCDROMAndShutdown,
  StatusKeepCDROMAndShutdown
};

enum ShutdownStatus askIfShutdownIsRequired();

void saveconfig( struct menuInfo* theSelectedInfo);

#endif
