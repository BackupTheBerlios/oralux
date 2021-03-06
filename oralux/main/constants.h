#ifndef CONSTANTS
#define CONSTANTS 1

#define ORALUX_RELEASE "0.7"

#define ORALUXGOLD 1
#define ORALUXGOLD_DIR "/usr/share/oraluxGold"
#define IBMECI_DIR "/usr/share/oraluxGold/ibmeci"
#undef ORALUXGOLD

enum phpStatus {
  PHP_OK,
  PHP_DECTALK_CANT_BE_FOUND,
  PHP_MAKEFILE_CANT_BE_FOUND,
  PHP_TCLDTK_LIB_NOT_BUILT,
  PHP_EMACSPEAK_NOT_DEFINED,
  PHP_SERIAL_NUMBER_IS_EXPECTED,
  PHP_SOFTWARE_DTK_NOT_FOUND,
  PHP_DTK_DIRECTORY_NOT_FOUND,
};

// Default values
#define DEFAULT_LANGUAGE English
#define DEFAULT_KEYBOARD americanKeyboard
#define DEFAULT_TTS TTS_Flite
#define DEFAULT_TTS_LANGUAGE English

// Ramdisk
// The ramdisk name must match with the ramdisk directory required by Knoppix.
// Warning: defined in oralux.conf too
#define RAMDISK "/ramdisk"

// Install tree
// Where most of the DECtalk directories are dynamically copied
#define INSTALL_TREE "/ramdisk/dtk"

// Where the emacspeak files for the DECtalk speech server are built (Makefile, tcldtk.o, tcldtk.so)
// Warning: defined in oralux.conf too
#define DTK_EMACSPEAK "/ramdisk/dtk/emacspeak"

// This file lists the installed tarballs and their features (name, size, last modifcation).
// This is for a future use
// It will be useful to identify a new DECtalk tarball (a new language or an update).
#define DTK_CONF "oralux_dtk.conf"

// Oralux Directory at runtime (in the CD)
#define ORALUX_RUNTIME "/usr/share/oralux"

// Where the emacspeak Makefile and emacspeak.conf are stored
// Useful to build e.g. the tcldtk.so shared library
#define ORALUX_RUNTIME_EMACSPEAK "/usr/share/oralux/install/packages/emacspeak"

// Where the Yasr's configuration files are stored
#define ORALUX_RUNTIME_YASR "/usr/share/oralux/install/packages/yasr"

// Where the pre-recorded audio files are stored
#define ORALUX_RUNTIME_AUDIO "/usr/share/oralux/audio"

// Size of internal buffers
#define BUFSIZE 1024

enum language {
  // --> 
  // Attention: the five following values index TheMessages
  English, /* American English */
  French,
  German,
  Spanish,
  Brazilian,
  // <--
  Russian, 
  MenuLanguageMax=Russian, //Last possible language for the menu

  British,
  CanadianFrench,
  Finnish,
  Italian, 
  Japanese, 
  MandarinChinese, /* Simplified Chinese */
  MexicanSpanish, 
  TaiwaneseMandarin, /* Traditional Chinese */
  SynthesisLanguageMax, //Last value!
};



// Debug
#ifdef DEBUG
extern char* TheENTERFilename;
#define ENTER(a) \
if (strcmp(TheENTERFilename,__FILE__)!=0) \
{\
  TheENTERFilename=__FILE__;\
  printf("\nFILE %s\n",__FILE__);\
}\
printf("== ENTER %s (%d)\n",a,__LINE__)



#define SHOW(a) printf("%s\n",a)
#define SHOW1(a) printf("%s\n",a)
#define SHOW2(a,b) printf(a,b)
#define SHOW3(a,b,c) printf(a,b,c)
#define LEAVE(a) printf("LEAVE %s\n",a)
#define LEAVE2(a,b) printf("LEAVE %s (%d)\n",a,b)
#else
#define ENTER(a)
#define SHOW(a)
#define SHOW1(a)
#define SHOW2(a,b)
#define SHOW3(a,b,c)
#define LEAVE(a)
#define LEAVE2(a,b)
#endif
//

/* // tty descriptor */
/* void setTTY(int the_port); */
/* int getTTY(); */


#endif
