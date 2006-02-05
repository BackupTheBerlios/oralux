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

char* getStringBoolean(int theValue);
int getIntBoolean(char* theValue);

void setConsoleFont(enum language theLanguage);

void buildI18n( enum language theMenuLanguage, 
		struct textToSpeechStruct theTextToSpeech, 
		enum keyboard theKeyboard,
		struct keyboardStruct theKeyboardFeatures,
		enum desktopIdentifier theDesktop,
		int theUserConfIsKnown);

void getLanguageVariable( enum language theWishedLanguage,
			  char** theCharset, char** theCountry,
			  char** theLang, char** theLanguage);

enum keyboard getProbableKeyboard( enum language theLanguage);

#endif
