#ifndef I18N
#define I18N 1

#include "constants.h"
#include "textToSpeech.h"
#include "desktop.h"
#include "keyboard.h"

void getStringKeyboard(enum keyboard theValue, char** theKeytable, char** theXKeyboard);
enum keyboard getEnumKeyboard(char* theKeytable);

char* getStringLanguage(int theValue);
enum language getEnumLanguage(char* theValue);

void setConsoleFont(enum language theLanguage);

void buildI18n( enum language theMenuLanguage, 
		struct textToSpeechStruct theTextToSpeech, 
		enum keyboard theKeyboard,
		struct keyboardStruct theKeyboardFeatures,
		enum desktopIdentifier theDesktop);

#endif
