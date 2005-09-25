/* 
----------------------------------------------------------------------------
linuxscreen.c
$Id: linuxscreen.c,v 1.2 2005/09/25 22:17:16 gcasse Exp $
$Author: gcasse $
Description: Read the data from the current screen.
$Date: 2005/09/25 22:17:16 $ |
$Revision: 1.2 $ |
Original Copyright (C) 1995-2004 by The BRLTTY Team. All rights reserved.

August 2005, Gilles Casse (gcasse@oralux.org)

Most of this code comes from BRLTTY 3.6.1 (scr_linux.c) and has been slightly modified.

The original BRLTTY source is under the GNU GENERAL PUBLIC LICENSE, Version 2, June 1991 as published by the Free Software
Foundation. 

The BRLTTY Web Page is http://mielke.cc/brltty/

----------------------------------------------------------------------------
*/

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <linux/vt.h>
#include <linux/kd.h>
#include "linuxscreen.h"
#include "debug.h"

/* < VCSA_Header */

struct t_VCSA_Header
{
  unsigned char myLines;
  unsigned char myCols;
  unsigned char myCol; 
  unsigned char myLine;
};
typedef struct t_VCSA_Header VCSA_Header;

#define HEADER_SIZE sizeof(VCSA_Header)

static VCSA_Header myHeader;

/* > */


/* From "scr.h" */
typedef struct {
  short rows, cols;	/* screen dimensions */
  short posx, posy;	/* cursor position */
  short no;		      /* screen number */
} ScreenDescription;

typedef struct {
  short left, top;	/* top-left corner (offset from 0) */
  short width, height;	/* dimensions */
} ScreenBox;

/* mode argument for readScreen() */
typedef enum {
  SCR_TEXT,		/* get screen text */
  SCR_ATTRIB		/* get screen attributes */
} ScreenMode;



/* #include "scr_linux.h" */
#define LINUX_SCREEN_DEVICES  "/dev/vcsa /dev/vcsa0 /dev/vcc/a"
#define LINUX_CONSOLE_DEVICES "/dev/tty0 /dev/vc/0"

typedef unsigned short int UnicodeNumber;
typedef UnicodeNumber ApplicationCharacterMap[0X100];


static unsigned int debugCharacterTranslationTable = 1;
static unsigned int debugApplicationCharacterMap = 1;
static unsigned int debugScreenFontMap = 1;

/* Copied from linux-2.2.17/drivers/char/consolemap.c: translations[0]
 * 8-bit Latin-1 mapped to Unicode -- trivial mapping
 */
static const ApplicationCharacterMap iso01Map = {
   0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007,
   0x0008, 0x0009, 0x000a, 0x000b, 0x000c, 0x000d, 0x000e, 0x000f,
   0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017,
   0x0018, 0x0019, 0x001a, 0x001b, 0x001c, 0x001d, 0x001e, 0x001f,
   0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027,
   0x0028, 0x0029, 0x002a, 0x002b, 0x002c, 0x002d, 0x002e, 0x002f,
   0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037,
   0x0038, 0x0039, 0x003a, 0x003b, 0x003c, 0x003d, 0x003e, 0x003f,
   0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047,
   0x0048, 0x0049, 0x004a, 0x004b, 0x004c, 0x004d, 0x004e, 0x004f,
   0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057,
   0x0058, 0x0059, 0x005a, 0x005b, 0x005c, 0x005d, 0x005e, 0x005f,
   0x0060, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067,
   0x0068, 0x0069, 0x006a, 0x006b, 0x006c, 0x006d, 0x006e, 0x006f,
   0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077,
   0x0078, 0x0079, 0x007a, 0x007b, 0x007c, 0x007d, 0x007e, 0x007f,
   0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087,
   0x0088, 0x0089, 0x008a, 0x008b, 0x008c, 0x008d, 0x008e, 0x008f,
   0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097,
   0x0098, 0x0099, 0x009a, 0x009b, 0x009c, 0x009d, 0x009e, 0x009f,
   0x00a0, 0x00a1, 0x00a2, 0x00a3, 0x00a4, 0x00a5, 0x00a6, 0x00a7,
   0x00a8, 0x00a9, 0x00aa, 0x00ab, 0x00ac, 0x00ad, 0x00ae, 0x00af,
   0x00b0, 0x00b1, 0x00b2, 0x00b3, 0x00b4, 0x00b5, 0x00b6, 0x00b7,
   0x00b8, 0x00b9, 0x00ba, 0x00bb, 0x00bc, 0x00bd, 0x00be, 0x00bf,
   0x00c0, 0x00c1, 0x00c2, 0x00c3, 0x00c4, 0x00c5, 0x00c6, 0x00c7,
   0x00c8, 0x00c9, 0x00ca, 0x00cb, 0x00cc, 0x00cd, 0x00ce, 0x00cf,
   0x00d0, 0x00d1, 0x00d2, 0x00d3, 0x00d4, 0x00d5, 0x00d6, 0x00d7,
   0x00d8, 0x00d9, 0x00da, 0x00db, 0x00dc, 0x00dd, 0x00de, 0x00df,
   0x00e0, 0x00e1, 0x00e2, 0x00e3, 0x00e4, 0x00e5, 0x00e6, 0x00e7,
   0x00e8, 0x00e9, 0x00ea, 0x00eb, 0x00ec, 0x00ed, 0x00ee, 0x00ef,
   0x00f0, 0x00f1, 0x00f2, 0x00f3, 0x00f4, 0x00f5, 0x00f6, 0x00f7,
   0x00f8, 0x00f9, 0x00fa, 0x00fb, 0x00fc, 0x00fd, 0x00fe, 0x00ff
};

/* Copied from linux-2.2.17/drivers/char/consolemap.c: translations[1]
 * VT100 graphics mapped to Unicode
 */
static const ApplicationCharacterMap vt100Map = {
   0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007,
   0x0008, 0x0009, 0x000a, 0x000b, 0x000c, 0x000d, 0x000e, 0x000f,
   0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017,
   0x0018, 0x0019, 0x001a, 0x001b, 0x001c, 0x001d, 0x001e, 0x001f,
   0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027,
   0x0028, 0x0029, 0x002a, 0x2192, 0x2190, 0x2191, 0x2193, 0x002f,
   0x2588, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037,
   0x0038, 0x0039, 0x003a, 0x003b, 0x003c, 0x003d, 0x003e, 0x003f,
   0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047,
   0x0048, 0x0049, 0x004a, 0x004b, 0x004c, 0x004d, 0x004e, 0x004f,
   0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057,
   0x0058, 0x0059, 0x005a, 0x005b, 0x005c, 0x005d, 0x005e, 0x00a0,
   0x25c6, 0x2592, 0x2409, 0x240c, 0x240d, 0x240a, 0x00b0, 0x00b1,
   0x2591, 0x240b, 0x2518, 0x2510, 0x250c, 0x2514, 0x253c, 0xf800,
   0xf801, 0x2500, 0xf803, 0xf804, 0x251c, 0x2524, 0x2534, 0x252c,
   0x2502, 0x2264, 0x2265, 0x03c0, 0x2260, 0x00a3, 0x00b7, 0x007f,
   0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087,
   0x0088, 0x0089, 0x008a, 0x008b, 0x008c, 0x008d, 0x008e, 0x008f,
   0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097,
   0x0098, 0x0099, 0x009a, 0x009b, 0x009c, 0x009d, 0x009e, 0x009f,
   0x00a0, 0x00a1, 0x00a2, 0x00a3, 0x00a4, 0x00a5, 0x00a6, 0x00a7,
   0x00a8, 0x00a9, 0x00aa, 0x00ab, 0x00ac, 0x00ad, 0x00ae, 0x00af,
   0x00b0, 0x00b1, 0x00b2, 0x00b3, 0x00b4, 0x00b5, 0x00b6, 0x00b7,
   0x00b8, 0x00b9, 0x00ba, 0x00bb, 0x00bc, 0x00bd, 0x00be, 0x00bf,
   0x00c0, 0x00c1, 0x00c2, 0x00c3, 0x00c4, 0x00c5, 0x00c6, 0x00c7,
   0x00c8, 0x00c9, 0x00ca, 0x00cb, 0x00cc, 0x00cd, 0x00ce, 0x00cf,
   0x00d0, 0x00d1, 0x00d2, 0x00d3, 0x00d4, 0x00d5, 0x00d6, 0x00d7,
   0x00d8, 0x00d9, 0x00da, 0x00db, 0x00dc, 0x00dd, 0x00de, 0x00df,
   0x00e0, 0x00e1, 0x00e2, 0x00e3, 0x00e4, 0x00e5, 0x00e6, 0x00e7,
   0x00e8, 0x00e9, 0x00ea, 0x00eb, 0x00ec, 0x00ed, 0x00ee, 0x00ef,
   0x00f0, 0x00f1, 0x00f2, 0x00f3, 0x00f4, 0x00f5, 0x00f6, 0x00f7,
   0x00f8, 0x00f9, 0x00fa, 0x00fb, 0x00fc, 0x00fd, 0x00fe, 0x00ff
};

/* Copied from linux-2.2.17/drivers/char/consolemap.c: translations[2]
 * IBM Codepage 437 mapped to Unicode
 */
static const ApplicationCharacterMap cp437Map = {
   0x0000, 0x263a, 0x263b, 0x2665, 0x2666, 0x2663, 0x2660, 0x2022, 
   0x25d8, 0x25cb, 0x25d9, 0x2642, 0x2640, 0x266a, 0x266b, 0x263c,
   0x25b6, 0x25c0, 0x2195, 0x203c, 0x00b6, 0x00a7, 0x25ac, 0x21a8,
   0x2191, 0x2193, 0x2192, 0x2190, 0x221f, 0x2194, 0x25b2, 0x25bc,
   0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027,
   0x0028, 0x0029, 0x002a, 0x002b, 0x002c, 0x002d, 0x002e, 0x002f,
   0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037,
   0x0038, 0x0039, 0x003a, 0x003b, 0x003c, 0x003d, 0x003e, 0x003f,
   0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047,
   0x0048, 0x0049, 0x004a, 0x004b, 0x004c, 0x004d, 0x004e, 0x004f,
   0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057,
   0x0058, 0x0059, 0x005a, 0x005b, 0x005c, 0x005d, 0x005e, 0x005f,
   0x0060, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067,
   0x0068, 0x0069, 0x006a, 0x006b, 0x006c, 0x006d, 0x006e, 0x006f,
   0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077,
   0x0078, 0x0079, 0x007a, 0x007b, 0x007c, 0x007d, 0x007e, 0x2302,
   0x00c7, 0x00fc, 0x00e9, 0x00e2, 0x00e4, 0x00e0, 0x00e5, 0x00e7,
   0x00ea, 0x00eb, 0x00e8, 0x00ef, 0x00ee, 0x00ec, 0x00c4, 0x00c5,
   0x00c9, 0x00e6, 0x00c6, 0x00f4, 0x00f6, 0x00f2, 0x00fb, 0x00f9,
   0x00ff, 0x00d6, 0x00dc, 0x00a2, 0x00a3, 0x00a5, 0x20a7, 0x0192,
   0x00e1, 0x00ed, 0x00f3, 0x00fa, 0x00f1, 0x00d1, 0x00aa, 0x00ba,
   0x00bf, 0x2310, 0x00ac, 0x00bd, 0x00bc, 0x00a1, 0x00ab, 0x00bb,
   0x2591, 0x2592, 0x2593, 0x2502, 0x2524, 0x2561, 0x2562, 0x2556,
   0x2555, 0x2563, 0x2551, 0x2557, 0x255d, 0x255c, 0x255b, 0x2510,
   0x2514, 0x2534, 0x252c, 0x251c, 0x2500, 0x253c, 0x255e, 0x255f,
   0x255a, 0x2554, 0x2569, 0x2566, 0x2560, 0x2550, 0x256c, 0x2567,
   0x2568, 0x2564, 0x2565, 0x2559, 0x2558, 0x2552, 0x2553, 0x256b,
   0x256a, 0x2518, 0x250c, 0x2588, 0x2584, 0x258c, 0x2590, 0x2580,
   0x03b1, 0x00df, 0x0393, 0x03c0, 0x03a3, 0x03c3, 0x00b5, 0x03c4,
   0x03a6, 0x0398, 0x03a9, 0x03b4, 0x221e, 0x03c6, 0x03b5, 0x2229,
   0x2261, 0x00b1, 0x2265, 0x2264, 0x2320, 0x2321, 0x00f7, 0x2248,
   0x00b0, 0x2219, 0x00b7, 0x221a, 0x207f, 0x00b2, 0x25a0, 0x00a0
};

static char *
vtPath (const char *base, unsigned char vt) {
  if (vt) {
    size_t length = strlen(base);
    char buffer[length+4];
    if (base[length-1] == '0') --length;
    strncpy(buffer, base, length);
    sprintf(buffer+length,  "%u", vt);
    return strdup(buffer);
  }
  return strdup(base);
}

static const char *consolePath=NULL;
static int consoleDescriptor=-1;

/* static int */
/* setConsolePath (void) { */
/*   return setDevicePath(&consolePath, LINUX_CONSOLE_DEVICES, "Console", R_OK|W_OK); */
/* } */

static void
closeConsole (void) {
  if (consoleDescriptor != -1) {
    if (close(consoleDescriptor) == -1) {
      SHOW("Console close");
    }
    SHOW2("Console closed: fd=%d", consoleDescriptor);
    consoleDescriptor = -1;
  }
}

static int
openConsole (unsigned char vt) {
  char *path = vtPath(consolePath, vt);
  if (path) {
    int console = open(path, O_RDWR|O_NOCTTY);
    if (console != -1) {
      closeConsole();
      consoleDescriptor = console;
      SHOW3("Console opened: %s: fd=%d", path, consoleDescriptor);
      free(path);
      return 1;
    }
    free(path);
  }
  return 0;
}

static const char *screenPath=NULL;
static int myScreenDescriptor=-1;
static unsigned char virtualTerminal;

static void
closeScreen (void) {
  if (myScreenDescriptor != -1) {
    if (close(myScreenDescriptor) == -1) {
      SHOW("Screen close");
    }
    SHOW2("Screen closed: fd=%d", myScreenDescriptor);
    myScreenDescriptor = -1;
  }
}

static int
openScreen (unsigned char vt) {
  char *path = vtPath(screenPath, vt);
  if (path) {
    int screen = open(path, O_RDWR);
    if (screen != -1) {
      if (openConsole(vt)) {
        closeScreen();
        myScreenDescriptor = screen;
        SHOW3("Screen opened: %s: fd=%d", path, myScreenDescriptor);
        free(path);
        virtualTerminal = vt;
        return 1;
      }
      close(screen);
    }
    free(path);
  }
  return 0;
}

/* static int */
/* rebindConsole (void) { */
/*   return virtualTerminal? 1: openConsole(0); */
/* } */

static int
controlConsole (int operation, void *argument) {
  int result = ioctl(consoleDescriptor, operation, argument);
  if (result == -1)
    if (errno == EIO)
      if (openConsole(virtualTerminal))
        result = ioctl(consoleDescriptor, operation, argument);
  return result;
}

static ApplicationCharacterMap applicationCharacterMap;
static int (*setApplicationCharacterMap) (int force);

static void
logApplicationCharacterMap (void) {
  if (debugApplicationCharacterMap) {
    char buffer[0X80];
    char *address = NULL;
    unsigned char character = 0;
    while (1) {
      if (!(character % 8)) {
        if (address) {
          SHOW2("%s", buffer);
          if (!character) break;
        }
        address = buffer;
        address += sprintf(address, "acm[%02X]:", character);
      }
      address += sprintf(address, " %4.4X", applicationCharacterMap[character++]);
    }
  }
}

static int
getUserAcm (int force) {
  ApplicationCharacterMap map;
  if (controlConsole(GIO_UNISCRNMAP, &map) != -1) {
    if (force || (memcmp(applicationCharacterMap, map, sizeof(applicationCharacterMap)) != 0)) {
      memcpy(applicationCharacterMap, map, sizeof(applicationCharacterMap));
      if (!force) SHOW("User application character map changed.");
      logApplicationCharacterMap();
      return 1;
    }
  } else {
    SHOW("ioctl GIO_UNISCRNMAP");
  }
  return 0;
}

static int
determineApplicationCharacterMap (int force) {
  const char *name = NULL;
  if (!getUserAcm(force)) return 0;
  {
    unsigned short character;
    for (character=0; character<0X100; ++character) {
      if (applicationCharacterMap[character] != (character | 0XF000)) {
        setApplicationCharacterMap = &getUserAcm;
        name = "user";
        break;
      }
    }
  }
  if (!name) {
    memcpy(applicationCharacterMap, iso01Map, sizeof(applicationCharacterMap));
    setApplicationCharacterMap = NULL;
    logApplicationCharacterMap();
    name = "iso01";
  }
  SHOW2("Application Character Map: %s", name);
  return 1;
}

static int vgaCharacterCount;
static int vgaLargeTable;
static int
setVgaCharacterCount (int force) {
  struct console_font_op cfo;
  int oldCount = vgaCharacterCount;
  vgaCharacterCount = 0X100;
  vgaLargeTable = 0;

  memset(&cfo, 0, sizeof(cfo));
  cfo.op = KD_FONT_OP_GET;
  cfo.height = 32;
  cfo.width = 16;

  if (controlConsole(KDFONTOP, &cfo) != -1) {
    switch (cfo.charcount) {
      default:
        SHOW2("Unexpected VGA character count: %d", cfo.charcount);
      case 0X200:
        vgaLargeTable = 1;
      case 0X100:
        vgaCharacterCount = cfo.charcount;
      case 0X000:
        break;
    }
  } else if (errno != EINVAL) {
    SHOW2("ioctl KDFONTOP[GET]: %s", strerror(errno));
  }

  if (!force)
    if (vgaCharacterCount == oldCount)
      return 0;
  SHOW3("VGA Character Count: %d(%s)",
           vgaCharacterCount,
           vgaLargeTable? "large": "small");
  return 1;
}

static struct unipair *screenFontMapTable;
static unsigned short screenFontMapCount;
static unsigned short screenFontMapSize;
static int
setScreenFontMap (int force) {
  struct unimapdesc sfm;
  unsigned short size = force? 0: screenFontMapCount;
  if (!size) size = 0X100;
  while (1) {
    sfm.entry_ct = size;
    if (!(sfm.entries = (struct unipair *)malloc(sfm.entry_ct * sizeof(*sfm.entries)))) {
      SHOW("Screen font map allocation");
      return 0;
    }
    if (controlConsole(GIO_UNIMAP, &sfm) != -1) break;
    free(sfm.entries);
    if (errno != ENOMEM) {
      SHOW("ioctl GIO_UNIMAP");
      return 0;
    }
    if (!(size <<= 1)) {
      SHOW("Screen font map too big.");
      return 0;
    }
  }
  if (!force) {
    if (sfm.entry_ct == screenFontMapCount) {
      if (memcmp(sfm.entries, screenFontMapTable, sfm.entry_ct*sizeof(sfm.entries[0])) == 0) {
        if (size == screenFontMapSize) {
          free(sfm.entries);
        } else {
          free(screenFontMapTable);
          screenFontMapTable = sfm.entries;
          screenFontMapSize = size;
        }
        return 0;
      }
    }
    free(screenFontMapTable);
  }
  screenFontMapTable = sfm.entries;
  screenFontMapCount = sfm.entry_ct;
  screenFontMapSize = size;
  SHOW2("Screen Font Map Size: %d", screenFontMapCount);
  if (debugScreenFontMap) {
    int i;
    for (i=0; i<screenFontMapCount; ++i) {
      const struct unipair *map = &screenFontMapTable[i];
      SHOW4("sfm[%03u]: unum=%4.4X fpos=%4.4X",
               i, map->unicode, map->fontpos);
    }
  }
  return 1;
}


/* 
 * The virtual screen devices return the actual font positions of the glyphs to
 * be drawn on the screen. The problem is that the font may not have been
 * designed for the character set being used. Most PC video cards have a
 * built-in font defined for the CP437 character set, but Linux users often use
 * the ISO-Latin-1 character set. In addition, the PSF format used by the newer
 * font files, which contains an internal unicode to font-position table,
 * allows the actual font positions to be unrelated to any known character set.
 * The kernel translates each character to be written to the screen from the
 * character set being used into unicode, and then from unicode into the
 * position within the font being used of the glyph to be drawn on the screen.
 * We need to reverse this translation in order to get the character code in
 * the expected character set.
 */
static unsigned char translationTable[0X200];
static int
setTranslationTable (int force) {
  int acmChanged = setApplicationCharacterMap && setApplicationCharacterMap(force);
  int sfmChanged = setScreenFontMap(force);
  int vccChanged = setVgaCharacterCount(force);

  if (acmChanged || sfmChanged || vccChanged) {
    unsigned short directPosition = 0XFF;
    if (vgaLargeTable) directPosition |= 0X100;

    memset(translationTable, '?', sizeof(translationTable));
    {
       int character;
       for (character=0XFF; character>=0; --character) {
         unsigned short unicode = applicationCharacterMap[character];
         int position = -1;
         if (!screenFontMapCount) {
           if (unicode < 0X100) position = unicode;
         } else if ((unicode & ~directPosition) == 0XF000) {
           position = unicode & directPosition;
         } else {
           int first = 0;
           int last = screenFontMapCount-1;
           while (first <= last) {
             int current = (first + last) / 2;
             struct unipair *map = &screenFontMapTable[current];
             if (map->unicode < unicode)
               first = current + 1;
             else if (map->unicode > unicode)
               last = current - 1;
             else {
               if (map->fontpos < vgaCharacterCount) position = map->fontpos;
               break;
             }
           }
         }
         if (position < 0) {
           if (debugCharacterTranslationTable) {
             SHOW3("No character mapping: char=%2.2X unum=%4.4X", character, unicode);
           }
         } else {
           translationTable[position] = character;
           if (debugCharacterTranslationTable) {
             SHOW4("Character mapping: char=%2.2X unum=%4.4X fpos=%2.2X",
                      character, unicode, position);
           }
         }
       }
    }
    if (debugCharacterTranslationTable) {
      const unsigned int count = 0X10;
      int position;
      for (position=0; position<vgaCharacterCount; position+=count) {
        char description[0X20];
        sprintf(description, "c2f[%02X]", position);
        DISPLAY_RAW_BUFFER(&translationTable[position], count);
      }
    }
    return 1;
  }

  return 0;
}


void closeLinuxScreen (void) {
  closeConsole();
  closeScreen();
}

static void
getScreenDescription (ScreenDescription *description) {
  if (lseek(myScreenDescriptor, 0, SEEK_SET) != -1) {
    unsigned char buffer[4];
    int count = read(myScreenDescriptor, buffer, sizeof(buffer));
    if (count == sizeof(buffer)) {
      description->rows = buffer[0];
      description->cols = buffer[1];
      description->posx = buffer[2];
      description->posy = buffer[3];
    } else if (count == -1) {
      SHOW("Screen header read");
    } else {
      long int expected = sizeof(buffer);
      SHOW3("Truncated screen header: expected %ld bytes, read %d.",
               expected, count);
    }
  } else {
    SHOW("Screen seek");
  }
}

static void
getConsoleDescription (ScreenDescription *description) {
  if (virtualTerminal) {
    description->no = virtualTerminal;
  } else {
    struct vt_stat state;
    if (controlConsole(VT_GETSTATE, &state) != -1) {
      description->no = state.v_active;
    } else {
      SHOW("ioctl VT_GETSTATE");
    }
  }
}

static void
describeLinuxScreen (ScreenDescription *description) {
  getScreenDescription(description);
  getConsoleDescription(description);

  /* Periodically recalculate font mapping. I don't know any way to be
   * notified when it changes, and the recalculation is not too
   * long/difficult.
   */
  {
    static int timer = 0;
    if (++timer > 100) {
      setTranslationTable(0);
      timer = 0;
    }
  }
}

int openLinuxScreen()
{
  int aStatus=0;

  ENTER("openLinuxScreen");

  setApplicationCharacterMap = &determineApplicationCharacterMap;

  screenPath="/dev/vcsa";
  consolePath="/dev/tty0";

  if (openScreen(0))
    {
      aStatus = setTranslationTable(1);
    }

  if (aStatus)
    {
      aStatus = (HEADER_SIZE == read( myScreenDescriptor, &myHeader, HEADER_SIZE));
    }

  return aStatus;
}

int getCursorLinuxScreen( int* theLine, int* theCol)
{
  int aStatus=0;

  ENTER("getCursorLinuxScreen");

  if (theLine && theCol 
      && (myScreenDescriptor!=-1) 
      && (lseek( myScreenDescriptor, 0, 0) != -1))
    {
      aStatus = (HEADER_SIZE == read( myScreenDescriptor, &myHeader, HEADER_SIZE));
      *theLine = myHeader.myLine;
      *theCol = myHeader.myCol;
    }
  return aStatus;
}

int readLinuxScreen( int theLine, int theFirstCol, int theLastCol, VCSA_Char** theBuffer, int* theByteNumber)    
{
  int aStatus=0;
  int aElementNumber=0;

  ENTER("readLinuxScreen");

    /* < check consistency */
  if ((theLine >= myHeader.myLines)
      || !theBuffer 
      || !theByteNumber)
    {
      return 0;
    }

  if (theFirstCol >= myHeader.myCols)
    {
      theFirstCol = myHeader.myCols - 1;
    }
  if (theLastCol >= myHeader.myCols)
    {
      theLastCol = myHeader.myCols - 1;
    }
  if (theFirstCol > theLastCol)
    {
      int i=theLastCol;
      theLastCol = theFirstCol;
      theFirstCol = i;
    }
  /* > */

  aElementNumber = (theLastCol - theFirstCol + 1);
  *theByteNumber = aElementNumber * sizeof(VCSA_Char);
  *theBuffer = (VCSA_Char *) malloc( *theByteNumber);

  if (lseek( myScreenDescriptor, 
	 sizeof(VCSA_Header) + sizeof(VCSA_Char) * (theLine * myHeader.myCols + theFirstCol), 
	     0) != -1)
    {
      aStatus = (*theByteNumber == read( myScreenDescriptor, *theBuffer, *theByteNumber));
    }

  if (aStatus)
    {
      int i=0;
      VCSA_Char* v=*theBuffer;
      
      for (i=0; i < aElementNumber; i++)
	{
	  if (vgaLargeTable && (v[i].myAttribute & 0X08))
	    {
	      v[i].myChar |= 0X100;
	    }
	  v[i].myChar = translationTable[ v[i].myChar];
	}
    }

  SHOW3("*theByteNumber=%d, aStatus=%d\n", *theByteNumber, aStatus);

  return aStatus;
}

/* 
Local variables:
folded-file: t
folding-internal-margins: nil
*/
