#ifndef BRAILLE
#define BRAILLE 1

#include "constants.h"
#include "i18n.h"

// Warning: new identifiers will be added at the end (the following order 
// must match static arrays in braille.c)
enum brailleIdentifier
  {
    ALBATROSS,
    ALVA,
    BRAILLELITE,
    BRAILLENOTE,
    COMBIBRAILLE,
    ECOBRAILLE,
    EUROBRAILLE,
    FREEDOMSCIENTIFIC,
    HANDYTECH,
    LOGTEXT,
    MULTIBRAILLE,
    MDV,
    MINIBRAILLE,
    PAPENMEIER,
    TSI,
    VARIO,
    VIDEOBRAILLE,
    VARIOHT,
    VOYAGER,
    VISIOBRAILLE,
    MaxBrailleIdentifier,
  };

enum brailleTableIdentifier
  {
    BRF_TABLE,
    CZ_TABLE,
    DA_TABLE,
    DE_TABLE,
    ES_TABLE,
    FI1_TABLE,
    FI2_TABLE,
    FR_TABLE,
    FR_CBIFS_TABLE,
    IT_TABLE,
    NABCC_TABLE,
    NO_H_TABLE,
    NO_P_TABLE,
    PL_TABLE,
    RU_TABLE,
    SE1_TABLE,
    SE2_TABLE,
    UK_TABLE,
    US_TABLE,
    VNI_TABLE,
    MaxBrailleTableIdentifier,
  };


enum brailleContractionTableIdentifier
  {
    noContraction,
    big5,
    compress,
    en_us_g2,
    fr_abrege,
    fr_integral,
    MaxBrailleContractionTableIdentifier,
  };

struct brailleInfo {
  enum brailleIdentifier myBrailleIdentifier;
  enum brailleTableIdentifier myBrailleTableIdentifier;
  enum brailleContractionTableIdentifier myBrailleContractionTableIdentifier;
  enum serialPortIdentifier myPort;
  int myPortIsUSB;
};

void getDefaultConf( enum keyboard theKeyboard, struct brailleInfo * theInfo);
void setBraille( struct brailleInfo * theInfo);

#endif

