#ifndef TEXTTOSPEECH
#define TEXTTOSPEECH 1

#include "constants.h"
//#include "i18n.h"
#include "serialPort.h"
#include "desktop.h"

enum textToSpeech { 
  TTS_Flite,
  TTS_DECtalk,
  TTS_Multispeech,
  TTS_Cicero,
  TTS_ViaVoice,
  TTS_AccentSA,
  TTS_AccentPC,
  TTS_Audapter,
  TTS_BrailleLite,
  TTS_BrailleNSpeak,
  TTS_Ciber232,
  TTS_Ciber232Plus,
  TTS_DECtalkExpress,
  TTS_DECtalkExternal,
  TTS_DECtalkMultivoice,
  TTS_DECtalk3,
  TTS_DolphinApollo,
  TTS_DoubleTalkLT,
  TTS_DoubleTalkPC,
  TTS_KeynoteGoldPC,
  TTS_LiteTalk,
  TTS_PcHabladoNotebook,
  TTS_SpeakOut,
  TTS_Transport,
  TTS_TypeNSpeak,
  TTS_MaxValue,
  TTS_Undefined=TTS_MaxValue,
};

struct textToSpeechStruct {
  enum textToSpeech myIdentifier;
  enum language myLanguage;
  enum serialPortIdentifier myPort;
};

int HasEmacspeakExternalTextToSpeech(struct textToSpeechStruct* theExternalTextToSpeech);

int setTextToSpeech(struct textToSpeechStruct* theTextToSpeech,
		    enum language thePreferredLanguage,
		    enum desktopIdentifier theDesktop,
		    int theUserMustBeAskedFor);

char* getStringSynthesis(enum textToSpeech theValue);
enum textToSpeech getEnumSynthesis(char* theValue);

#endif
