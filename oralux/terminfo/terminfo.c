#include <stdio.h>
#include "escape2terminfo.h"

extern int yylex (void);
extern char *yytext;
extern int yyleng;

int yywrap ()
{  
   return 1; 
}

/* < array myStringCapacity */

char* myStringCapacity[]={
  "",
  "ACSC",
  "CBT",
  "BEL",
  "CR",
  "CPI",
  "LPI",
  "CHR",
  "CVR",
  "CSR",
  "RMP",
  "TBC",
  "MGC",
  "CLEAR",
  "EL1",
  "EL",
  "ED",
  "HPA",
  "CMDCH",
  "CWIN",
  "CUP",
  "CUD1",
  "HOME",
  "CIVIS",
  "CUB1",
  "MRCUP",
  "CNORM",
  "CUF1",
  "LL",
  "CUU1",
  "CVVIS",
  "DEFC",
  "DCH1",
  "DL1",
  "DIAL",
  "DSL",
  "DCLK",
  "HD",
  "ENACS",
  "SMACS",
  "SMAM",
  "BLINK",
  "BOLD",
  "SMCUP",
  "SMDC",
  "DIM",
  "SWIDM",
  "SDRFQ",
  "SMIR",
  "SITM",
  "SLM",
  "SMICM",
  "SNLQ",
  "SNRMQ",
  "PROT",
  "REV",
  "INVIS",
  "SSHM",
  "SMSO",
  "SSUBM",
  "SSUPM",
  "SMUL",
  "SUM",
  "SMXON",
  "ECH",
  "RMACS",
  "RMAM",
  "SGR0",
  "RMCUP",
  "RMDC",
  "RWIDM",
  "RMIR",
  "RITM",
  "RLM",
  "RMICM",
  "RSHM",
  "RMSO",
  "RSUBM",
  "RSUPM",
  "RMUL",
  "RUM",
  "RMXON",
  "PAUSE",
  "HOOK",
  "FLASH",
  "FF",
  "FSL",
  "WINGO",
  "HUP",
  "IS1",
  "IS2",
  "IS3",
  "IF",
  "IPROG",
  "INITC",
  "INITP",
  "ICH1",
  "IL1",
  "IP",
  "KA1",
  "KA3",
  "KB2",
  "KBS",
  "KBEG",
  "KCBT",
  "KC1",
  "KC3",
  "KCAN",
  "KTBC",
  "KCLR",
  "KCLO",
  "KCMD",
  "KCPY",
  "KCRT",
  "KCTAB",
  "KDCH1",
  "KDL1",
  "KCUD1",
  "KRMIR",
  "KEND",
  "KENT",
  "KEL",
  "KED",
  "KEXT",
  "KF0",
  "KF1",
  "KF10",
  "KF11",
  "KF12",
  "KF13",
  "KF14",
  "KF15",
  "KF16",
  "KF17",
  "KF18",
  "KF19",
  "KF2",
  "KF20",
  "KF21",
  "KF22",
  "KF23",
  "KF24",
  "KF25",
  "KF26",
  "KF27",
  "KF28",
  "KF29",
  "KF3",
  "KF30",
  "KF31",
  "KF32",
  "KF33",
  "KF34",
  "KF35",
  "KF36",
  "KF37",
  "KF38",
  "KF39",
  "KF4",
  "KF40",
  "KF41",
  "KF42",
  "KF43",
  "KF44",
  "KF45",
  "KF46",
  "KF47",
  "KF48",
  "KF49",
  "KF5",
  "KF50",
  "KF51",
  "KF52",
  "KF53",
  "KF54",
  "KF55",
  "KF56",
  "KF57",
  "KF58",
  "KF59",
  "KF6",
  "KF60",
  "KF61",
  "KF62",
  "KF63",
  "KF7",
  "KF8",
  "KF9",
  "KFND",
  "KHLP",
  "KHOME",
  "KICH1",
  "KIL1",
  "KCUB1",
  "KLL",
  "KMRK",
  "KMSG",
  "KMOV",
  "KNXT",
  "KNP",
  "KOPN",
  "KOPT",
  "KPP",
  "KPRV",
  "KPRT",
  "KRDO",
  "KREF",
  "KRFR",
  "KRPL",
  "KRST",
  "KRES",
  "KCUF1",
  "KSAV",
  "SHIFTKBEG",
  "SHIFTKCAN",
  "SHIFTKCMD",
  "SHIFTKCPY",
  "SHIFTKCRT",
  "SHIFTKDC",
  "SHIFTKDL",
  "KSLT",
  "SHIFTKEND",
  "SHIFTKEOL",
  "SHIFTKEXT",
  "KIND",
  "SHIFTKFND",
  "SHIFTKHLP",
  "SHIFTKHOM",
  "SHIFTKIC",
  "SHIFTKLFT",
  "SHIFTKMSG",
  "SHIFTKMOV",
  "SHIFTKNXT",
  "SHIFTKOPT",
  "SHIFTKPRV",
  "SHIFTKPRT",
  "KRI",
  "SHIFTKRDO",
  "SHIFTKRPL",
  "SHIFTKRIT",
  "SHIFTKRES",
  "SHIFTKSAV",
  "SHIFTKSPD",
  "KHTS",
  "SHIFTKUND",
  "KSPD",
  "KUND",
  "KCUU1",
  "RMKX",
  "SMKX",
  "LF0",
  "LF1",
  "LF10",
  "LF2",
  "LF3",
  "LF4",
  "LF5",
  "LF6",
  "LF7",
  "LF8",
  "LF9",
  "FLN",
  "RMLN",
  "SMLN",
  "RMM",
  "SMM",
  "MHPA",
  "MCUD1",
  "MCUB1",
  "MCUF1",
  "MVPA",
  "MCUU1",
  "NEL",
  "PORDER",
  "OC",
  "OP",
  "PAD",
  "DCH",
  "DL",
  "CUD",
  "MCUD",
  "ICH",
  "INDN",
  "IL",
  "CUB",
  "MCUB",
  "CUF",
  "MCUF",
  "RIN",
  "CUU",
  "MCUU",
  "PFKEY",
  "PFLOC",
  "PFX",
  "PLN",
  "MC0",
  "MC5P",
  "MC4",
  "MC5",
  "PULSE",
  "QDIAL",
  "RMCLK",
  "REP",
  "RFI",
  "RS1",
  "RS2",
  "RS3",
  "RF",
  "RC",
  "VPA",
  "SC",
  "IND",
  "RI",
  "SCS",
  "SGR",
  "SETB",
  "SMGB",
  "SMGBP",
  "SCLK",
  "SCP",
  "SETF",
  "SMGL",
  "SMGLP",
  "SMGR",
  "SMGRP",
  "HTS",
  "SMGT",
  "SMGTP",
  "WIND",
  "SBIM",
  "SCSD",
  "RBIM",
  "RCSD",
  "SUBCS",
  "SUPCS",
  "HT",
  "DOCR",
  "TSL",
  "TONE",
  "UC",
  "HU",
  "U0",
  "U1",
  "U2",
  "U3",
  "U4",
  "U5",
  "U6",
  "U7",
  "U8",
  "U9",
  "WAIT",
  "XOFFC",
  "XONC",
  "ZEROM",
  "SCESA",
  "BICR",
  "BINEL",
  "BIREP",
  "CSNM",
  "CSIN",
  "COLORNM",
  "DEFBI",
  "DEVT",
  "DISPC",
  "ENDBI",
  "SMPCH",
  "SMSC",
  "RMPCH",
  "RMSC",
  "GETM",
  "KMOUS",
  "MINFO",
  "PCTRM",
  "PFXL",
  "REQMP",
  "SCESC",
  "S0DS",
  "S1DS",
  "S2DS",
  "S3DS",
  "SETAB",
  "SETAF",
  "SETCOLOR",
  "SMGLR",
  "SLINES",
  "SMGTB",
  "EHHLM",
  "ELHLM",
  "ELOHLM",
  "ERHLM",
  "ETHLM",
  "EVHLM",
  "SGR1",
  "SLENGTH",
  "TEXT...",
};

/* > */


void clearBuffer( char *theBuffer, int theMaxLine, int theMaxCol)
{
  // Init
  int i=0;
  for (i=0;i<theMaxLine;i++)
    {
      int j=0;
      for (j=0;j<theMaxCol;j++)
	{
	  theBuffer[ theMaxCol*i + j]=0x20;
	}
    }
}

void displayBuffer( char *theBuffer, int theMaxLine, int theMaxCol)
{
  // Init
  int i=0;
  for (i=0;i<theMaxLine;i++)
    {
      char* aLine=theBuffer + theMaxCol*i;
      char c=aLine[theMaxCol-1];
      aLine[theMaxCol-1]=0;
      printf("%s%c\n", aLine, c);
      aLine[theMaxCol-1]=c;
    }
}

void eraseLine( char* theLine, int theLength)
{
  int i=0;
  for (i=0;i<theLength;i++)
    {
      theLine[i]=0x20;
    }
}

void deleteCharacter(char* theLine, int theErasedLength, int theTotalLength)
{
  memmove(theLine, theLine+theErasedLength, theErasedLength);
  eraseLine(&(theLine[theTotalLength-theErasedLength]), theErasedLength);
}

int main()
{
  enum StringCapacity aCapacity;
  int aCol=0; //RAF 0 or 1
  int aLine=0; //RAF 0 or 1
  int aSavedCol=aCol;
  int aSavedLine=aLine;
  char* aBuffer=NULL;
  int aMaxLine=30;
  int aMaxCol=30;

  aBuffer=(char*)malloc(aMaxLine * aMaxCol * sizeof(char*));
  clearBuffer(aBuffer, aMaxLine, aMaxCol);


  while(aCapacity=yylex())
    {
      printf("|%s = %s|\n", myStringCapacity[ aCapacity], yytext);

      switch(aCapacity)
	{
	case CLEAR:
	  aCol=0;
	  aLine=0;
	  clearBuffer(aBuffer, aMaxLine, aMaxCol);
	  break;
	case CUB1:
	  if (aCol!=0)
	    {
	      aCol--;
	    }
	  break;
	case CUD1:
	  aLine++;
	  aCol=0;
	  break;
	case CUF1:
	  aCol++;
	  break;
	case CUP:
	  aCol=myParameters[0];
	  aLine=myParameters[1];
	  break;
	case CUU:
	  aLine-=myParameters[0];
	  break;
	case DCH: // delete characters
	  {
	    int aShift=myParameters[0]; // number of characters to delete, the following chars shift to the left
	    deleteCharacter(&(aBuffer[aLine*aMaxCol+aCol]), aShift, aMaxCol-aShift);
	  }
	  break;
	case DCH1:
	  {
	    int aShift=1;
	    deleteCharacter(&(aBuffer[aLine*aMaxCol+aCol]), aShift, aMaxCol-aShift);
	  }
	  break;
	case DL: // delete lines
	  break;
	case DL1:
	  break;
	case ECH: // Erase characters
	  {
	    int aErasedLength=myParameters[0];
	    eraseLine(&(aBuffer[aLine*aMaxCol+aCol]), aErasedLength);
	  }
	  break;
	case ED: // Clear the display after the cursor
	  break;
	case EL1:
	  break;
	case HOME:
	  aCol=0;
	  aLine=0;
	  break;
	case HPA:
	  aCol=myParameters[0];
	  break;
	case IL: // several lines are added (the content is shifted to the bottom of the screen)
	  break;
	case IL1:
	  break;
	case RC:
	  aCol=aSavedCol;
	  aLine=aSavedLine;
	  break;
	case SC:
	  aSavedCol=aCol;
	  aSavedLine=aLine;
	  break;
	case SETB:
	  break;
	case SETF:
	  break;
	case SGR:
	  break;
	case VPA:
	  aLine=myParameters[0];
	  break;
	default:
	  aBuffer[aLine*aMaxCol+aCol]=*yytext;
	  aCol++;
	}
    }

  displayBuffer(aBuffer, aMaxLine, aMaxCol);

  free(aBuffer);
}

