#ifndef BRAILLE
#define BRAILLE 1

#include "constants.h"
#include "i18n.h"

// Warning: new identifiers will be added at the end (the following order 
// must match static arrays in braille.c)
enum brailleIdentifier
  {
    ALVA,
    BRAILLELITE,
    BRAILLENOTE,
    COMBIBRAILLE,
    ECOBRAILLE,
    EUROBRAILLE,
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
    DA_TABLE,
    DE_TABLE,
    ES_TABLE,
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

struct brailleInfo {
  enum brailleIdentifier myBrailleIdentifier;
  enum brailleTableIdentifier myBrailleTableIdentifier;
  enum serialPortIdentifier myPort;
};

void getDefaultConf( enum keyboard theKeyboard, struct brailleInfo * theInfo);
void setBraille( struct brailleInfo * theInfo);

#endif

