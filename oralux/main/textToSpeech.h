#ifndef TEXTTOSPEECH
#define TEXTTOSPEECH 1

#include "constants.h"
//#include "i18n.h"
#include "serialPort.h"
#include "desktop.h"

// The order is important (textToSpeech.c)
enum textToSpeech { 
  TTS_Flite,
  TTS_DECtalk,
  TTS_EFM,
  TTS_ParleMax,
  TTS_Multispeech,
  TTS_AccentSA,
  TTS_BrailleLite,
  TTS_BrailleNSpeak,
  TTS_Ciber232,
  TTS_Ciber232Plus,
  TTS_DECtalkExpress,
  TTS_DECtalkMultivoice,
  TTS_DECtalk3,
  TTS_DolphinApollo,
  TTS_DoubleTalkLT,
  TTS_DoubleTalkPC,
  TTS_LiteTalk,
  TTS_PcHabladoNotebook,
  TTS_TypeNSpeak,
  TTS_MaxValue,
  TTS_Undefined=TTS_MaxValue,
};

struct textToSpeechStruct {
  enum textToSpeech myIdentifier;
  enum language myLanguage;
  enum serialPortIdentifier myPort;
};

int HasExternalTextToSpeech(struct textToSpeechStruct* theExternalTextToSpeech);

void setTextToSpeech(struct textToSpeechStruct* theTextToSpeech,
		     enum language thePreferredLanguage,
		     enum desktopIdentifier theDesktop,
		     int theUserMustBeAskedFor);

char* getStringSynthesis(enum textToSpeech theValue);
enum textToSpeech getEnumSynthesis(char* theValue);
int isExternalSynth( enum textToSpeech theIdentifier);

#endif
