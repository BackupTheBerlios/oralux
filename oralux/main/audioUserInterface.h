#ifndef AUDIOMESSAGES
#define AUDIOMESSAGES 1
#include "constants.h"
#include "i18n.h"

// Keyboards to add, info required; today, they are assigned to the american keyboard.
/*   chineseSaidSimplified, */
/*   chineseSaidTraditionnal, */
/*   hebrewSaid, */
/*   japaneseSaid, */
/*   netherlandSaid, */
 
enum sentence {
  americanSaid, // same order as in enum keyboard
  belgianSaid,
  britishSaid,
  bulgarianSaid,
  czechSaid,
  danskSaid,
  finnishSaid,
  frenchSaid,
  germanSaid,
  italianSaid, 
  polishSaid,
  russianSaid,
  slovakSaid,
  spanishSaid,
  swissSaid,
  turkishSaid,
  Delete, // sentence 0x10
  WelcomeToOralux,
  DoYouWantToInstallDECtalk,
  PleasePressKey,
  ProblemWithPHPStatus,
  DectalkCantBeFound,
  MakefileCantBeFound,
  TheTcldtkLibraryIsNotBuilt,
  EmacspeakVariableNotDefined,
  SoftwareDECtalkNotFound,
  DECtalkDirectoryNotFound,
  PleaseEnterYourSerialNumber,
  SorrySerialNumberNotCorrect,
  IfYouWantToTryAgainPress,
  IfYouWantToStopThisStage,
  MenuInEnglish,
  MenuInFrench, // 0x20
  y, // the 'y' character
  leftAngleBracket, // <
  leftCurlyBracket, // { 
  leftParenthesis, // (
  leftSquareBracket, // [ 
  verticalLine, // |
  space, //
  at, // @
  rightCurlyBracket, // }
  whichKeyboard,
  choosingVolume,
  changeKeyboard,
  keyboardIs,
  enterAgainMenu,
  ToEjectCD,
  ThenReturnOnceEjected, // 0x30
  MenuInGerman,
  MenuInSpanish,
  DoYouWantEFM,
  DoYouWantFlite,
  SayYes,
  SayNo,
  Alva,
  BrailleLite,
  BrailleNote,
  CombiBraille,
  EcoBraille,
  EuroBraille,
  HandyTech,
  LogText,
  MultiBraille,
  Say_MDV, // 0x40
  MiniBraille,
  Papenmeier,
  Say_TSI,
  Vario,
  VideoBraille,
  VarioHT,
  Voyager,
  VisioBraille,
  French_cbifs,
  NABCC,
  NorwegianGerman,
  Norwegian,
  Swedish,
  Swedish2,
  Vietnamese,
  selectBraille, // 0x50
  whichBraille,
  FirstSerialPort,
  SecondSerialPort,
  whichTable,
  whichPort,
  changeBraille,
  brailleIs,
  changeTable,
  tableIs,
  changePort,
  portIs,
  startBraille,
  AccentSA,
  BrailleLiteTTS,
  BrailleNSpeak,
  Ciber232,
  Ciber232Plus,
  DECtalkExpress,
  DECtalkMultivoice,
  DECtalk3,
  DolphinApollo,
  DoubleTalkLT,
  DoubleTalkPC,
  LiteTalk,
  PcHabladoNotebook,
  TypeNSpeak,
  selectExternalSynth,
  whichExternalSynth,
  externalSynthIs,
  changeExternalSynth,
  sayEmacspeak,
  sayYasr,
  desktopIs,
  changeDesktop,
  whichDesktop,
  DoYouWantParleMax,
  DoYouWantToReboot,
  DoYouWantToShutdown,
  DoYouWantMultispeech,
  NoBraille,
  MenuInRussian,
  oraluxRelease,
  changeKeyboardFeatures,
  noStickyKey,
  stickyKey,
  repeatKey,
  noRepeatKey,
  setUpInternet,
  configureExternalSerialModem,
  installAdslUsbModem,
  setupMail,
  MaxSentence, // the last enum! Place any new enum before it, please.
};

// init the audio user interface
// return 1 if ok
int initAUI(char* theOggDirectory, enum language theLanguage, char* thePortName);

int restartAUI();

void stopAUI(int theSoundMustBeFinished);

// 1 enable, 0: disable
void enableSound(int the_sound_is_enable);

// Beep! From the sound device.
void beep();

// Beep! From the loud speaker.
void beepLoudSpeaker();

// Clear the stored sentences
void clearStoredSentences();

// Get the text related to the enumerate
char* getText(enum sentence theSentence);

// Get the ogg filename related to the enumerate
char* getOggFilename(enum sentence theSentence);
char* getOggFilenameForThisLanguage(enum sentence theSentence, enum language theLanguage);

// Get the ogg filename for the beep
char* getOggFilenameBeep();

// Get the ogg filename related to the supplied character
char* getOggFilenameChar(char c);

// play an ogg vorbis file
void playOGG(char *theFilename);

// Say the sentence
void say(enum sentence theSentence);

// Say the sentence (Yes or No) whatever the sound is enable or disabled.
// This is a synchronous call
void sayForce(enum sentence theSentence);

// Say the stored sentences
void sayAgain();

// Pronounce the character
void sayChar(char c);

// Set the current language for all the other operations
void setLanguage( enum language theLanguage);

// tty descriptor
void setTTY(int the_port);
int getTTY();

#endif
