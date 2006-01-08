/* 
----------------------------------------------------------------------------
terminfo2list.c
$Id: terminfo2list.c,v 1.8 2006/01/08 23:51:27 gcasse Exp $
$Author: gcasse $
Description: convert the terminfo entries to a list of commands.
$Date: 2006/01/08 23:51:27 $ |
$Revision: 1.8 $ |
Copyright (C) 2005 Gilles Casse (gcasse@oralux.org)

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
----------------------------------------------------------------------------
*/

/* < include */
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include "escape2terminfo.h"
#include "linuxconsole.h"
#include "terminfo2list.h"
#include "debug.h"
/* > */
/* < global variables */

/* TBD: no static */
static enum StringCapacity myPreviousCapacity=ACSC;
static GList* myList=NULL; 

/* > */
/* < flex definitions */
int yywrap ()
{  
   return 1; 
}
/* > */
/* < handlers type*/

typedef terminfoEntry* (*T_CREATE_ENTRY)(enum StringCapacity theCapacity, void* theData1, void* theData2);

typedef void (*T_DELETE_ENTRY)(terminfoEntry* theEntry);

typedef terminfoEntry* (*T_COPY_ENTRY)(const terminfoEntry* theEntry);



/* > */
/* < createTerminfoEntry */
static terminfoEntry* createTerminfoEntry()
{
  terminfoEntry* this = (terminfoEntry*)malloc(sizeof(terminfoEntry));
  memset(this, 0, sizeof(terminfoEntry));
  return this;
}

/* > */
/* < createEntry, deleteEntry, copyEntry, createExternalEntry */

static terminfoEntry* createEntry(enum StringCapacity theCapacity, void* theData1, void* theData2)
{
  terminfoEntry* anEntry = NULL;
  ENTER("createEntry");

  anEntry = createTerminfoEntry();

  if (anEntry)
    {
      anEntry->myCapacity = theCapacity;
      /* if theData1 is a non NULL pointer, copy its data to myData1 */
      anEntry->myData1 = theData1 ? (void*)(*((int*)theData1)) : NULL;
      anEntry->myData2 = theData2 ? (void*)(*((int*)theData2)) : NULL;
      anEntry->myEscapeSequence = strdup(yytext);
      anEntry->myParent = NULL; /* will be updated once all the entries are created */
      myList = g_list_append(myList, (gpointer)anEntry);
    }

  return anEntry;
}

static void deleteEntry(terminfoEntry* theEntry)
{
  ENTER("deleteEntry");

  if (theEntry)
    {
      /* Warning: myData must be freed according to the capability */
      free(theEntry->myEscapeSequence);
      free(theEntry);
    }
}

static terminfoEntry* copyEntry(const terminfoEntry* theEntry)
{
  terminfoEntry* anEntry=NULL;

  ENTER("copyEntry");
  
  if (theEntry) 
    {
      anEntry = createTerminfoEntry();

      if (anEntry)
	{
	  anEntry->myCapacity = theEntry->myCapacity;
	  /* if theData1 is a non NULL pointer, copy its data to myData1 */
	  anEntry->myData1 = theEntry->myData1;
	  anEntry->myData2 = theEntry->myData2;
	  anEntry->myEscapeSequence = strdup(theEntry->myEscapeSequence);
	}
    }

  return anEntry;
}

static terminfoEntry* createExternalEntry(enum StringCapacity theCapacity, void* theData1, void* theData2, chartyp* theEscapeSequence)
{
  terminfoEntry* anEntry = createTerminfoEntry();
  ENTER("createExternalEntry");

  if (anEntry)
    {
      anEntry->myCapacity = theCapacity;
      /* if theData1 is a non NULL pointer, copy its data to myData1 */
      anEntry->myData1 = theData1 ? (void*)(*((int*)theData1)) : NULL;
      anEntry->myData2 = theData2 ? (void*)(*((int*)theData2)) : NULL;
      anEntry->myEscapeSequence = strdup(theEscapeSequence);
    }

  return anEntry;
}

/* > */
/* < create_SGR_Entry */

static terminfoEntry* create_SGR_Entry(enum StringCapacity theCapacity, void* theData1, void* theData2)
{
  terminfoEntry* anEntry=NULL;
  ENTER("create_SGR_Entry");

  anEntry=createEntry(theCapacity, NULL, NULL);
  anEntry->myData1=(void*)malloc(sizeof(style));
  if (anEntry->myData1 && theData1)
    {
      copyStyle((style*)anEntry->myData1, (style*)theData1);
    }
  return anEntry;
}

static void delete_SGR_Entry(terminfoEntry* theEntry)
{
  ENTER("delete_SGR_Entry");

  if (theEntry)
    {
      free(theEntry->myData1);
      deleteEntry(theEntry);
    }
}

static terminfoEntry* copy_SGR_Entry(const terminfoEntry* theEntry)
{
  terminfoEntry* anEntry=copyEntry( theEntry);

  ENTER("copy_SGR_Entry");

  if (anEntry && anEntry->myData1)
    {
      style* aStyle = (style*)malloc(sizeof(style));
      copyStyle( aStyle, (style*)anEntry->myData1);
      anEntry->myData1 = aStyle;
    }

  return anEntry;
}

/* > */
/* < createText */

static terminfoEntry* createText(enum StringCapacity theCapacity, void* theData1, void* theData2)
{
  static GString* aString=NULL;
  static terminfoEntry* anEntry=NULL;

  ENTER("createText");

  if (myPreviousCapacity==TEXTFIELD)
    {
      /* Add another char to the current gstring */
      g_string_append( aString, yytext);
      SHOW3("g_string_append(%x, %s)", (unsigned int)aString, yytext);
    }
  else
    { /* First char */
      aString = g_string_new( yytext);
      SHOW3("%x=g_string_new(%s)", (unsigned int)aString, yytext);

      /* add the new list element */
      anEntry = createEntry( theCapacity, NULL, NULL);
      anEntry->myData1 = (void*)aString;
    }

  return anEntry;
}

static void deleteText(terminfoEntry* theEntry)
{
  ENTER("deleteText");
  if (theEntry)
    {
      g_string_free( (GString*)theEntry->myData1, 1);
      SHOW3("g_string_free(%x, %d)", (unsigned int)theEntry->myData1, 1);

      deleteEntry(theEntry);
    }
}

static terminfoEntry* copyText(const terminfoEntry* theEntry)
{
  terminfoEntry* anEntry=copyEntry( theEntry);

  ENTER("copyText");

  if (anEntry && anEntry->myData1)
    {
      GString* aText = (GString*)anEntry->myData1;
      if (aText && aText->str)
	{
	  anEntry->myData1 = (void*)g_string_new( aText->str);
	  SHOW3("%x=g_string_new(%s)", (unsigned int)anEntry->myData1, aText->str);
	}
    }

  return anEntry;
}

/* > */
/* < array of entries */
struct t_entryCommands
{
  T_CREATE_ENTRY myCreateEntryHandler;
  void* myData1;
  void* myData2;
  T_DELETE_ENTRY myDeleteEntryHandler;
  enum StringCapacity myCapacity;
  T_COPY_ENTRY myCopyEntryHandler;
};
typedef struct t_entryCommands entryCommands;
#define PARAM0 (void *)(&TheParameter[0])
#define PARAM1 (void *)(&TheParameter[1])

entryCommands TheEntryCommands[]=
{
  {NULL, NULL, NULL, NULL, ACSC, NULL}, /* Undefined */ 
  {NULL, NULL, NULL, NULL, ACSC, NULL}, 
  {NULL, NULL, NULL, NULL, CBT, NULL}, 
  {createEntry, NULL, NULL, deleteEntry, BEL, copyEntry}, 
  {createEntry, NULL, NULL, deleteEntry, CR, copyEntry}, 
  {NULL, NULL, NULL, NULL, CPI, NULL}, 
  {NULL, NULL, NULL, NULL, LPI, NULL}, 
  {NULL, NULL, NULL, NULL, CHR, NULL}, 
  {NULL, NULL, NULL, NULL, CVR, NULL}, 
  {NULL, NULL, NULL, NULL, CSR, NULL}, 
  {NULL, NULL, NULL, NULL, RMP, NULL}, 
  {NULL, NULL, NULL, NULL, TBC, NULL}, 
  {NULL, NULL, NULL, NULL, MGC, NULL}, 
  {createEntry, NULL, NULL, deleteEntry, CLEAR, copyEntry}, 
  {NULL, NULL, NULL, NULL, EL1, NULL}, 
  {createEntry, PARAM0, NULL, deleteEntry, EL, copyEntry}, 
  {createEntry, PARAM0, NULL, deleteEntry, ED, copyEntry}, 
  {createEntry, PARAM0, NULL, deleteEntry, HPA, copyEntry}, 
  {NULL, NULL, NULL, NULL, CMDCH, NULL}, 
  {NULL, NULL, NULL, NULL, CWIN, NULL}, 
  {createEntry, PARAM0, PARAM1, deleteEntry, CUP, copyEntry}, 
  {createEntry, NULL, NULL, deleteEntry, CUD1, copyEntry}, 
  {createEntry, NULL, NULL, deleteEntry, HOME, copyEntry}, 
  {NULL, NULL, NULL, NULL, CIVIS, NULL}, 
  {createEntry, NULL, NULL, deleteEntry, CUB1, copyEntry}, 
  {NULL, NULL, NULL, NULL, MRCUP, NULL}, 
  {NULL, NULL, NULL, NULL, CNORM, NULL}, 
  {createEntry, NULL, NULL, deleteEntry, CUF1, copyEntry}, 
  {NULL, NULL, NULL, NULL, LL, NULL}, 
  {NULL, NULL, NULL, NULL, CUU1, NULL}, 
  {NULL, NULL, NULL, NULL, CVVIS, NULL}, 
  {NULL, NULL, NULL, NULL, DEFC, NULL}, 
  {NULL, NULL, NULL, NULL, DCH1, NULL}, 
  {NULL, NULL, NULL, NULL, DL1, NULL}, 
  {NULL, NULL, NULL, NULL, DIAL, NULL}, 
  {NULL, NULL, NULL, NULL, DSL, NULL}, 
  {NULL, NULL, NULL, NULL, DCLK, NULL}, 
  {NULL, NULL, NULL, NULL, HD, NULL}, 
  {NULL, NULL, NULL, NULL, ENACS, NULL}, 
  {NULL, NULL, NULL, NULL, SMACS, NULL}, 
  {NULL, NULL, NULL, NULL, SMAM, NULL}, 
  {NULL, NULL, NULL, NULL, BLINK, NULL}, 
  {NULL, NULL, NULL, NULL, BOLD, NULL}, 
  {NULL, NULL, NULL, NULL, SMCUP, NULL}, 
  {NULL, NULL, NULL, NULL, SMDC, NULL}, 
  {NULL, NULL, NULL, NULL, DIM, NULL}, 
  {NULL, NULL, NULL, NULL, SWIDM, NULL}, 
  {NULL, NULL, NULL, NULL, SDRFQ, NULL}, 
  {NULL, NULL, NULL, NULL, SMIR, NULL}, 
  {NULL, NULL, NULL, NULL, SITM, NULL}, 
  {NULL, NULL, NULL, NULL, SLM, NULL}, 
  {NULL, NULL, NULL, NULL, SMICM, NULL}, 
  {NULL, NULL, NULL, NULL, SNLQ, NULL}, 
  {NULL, NULL, NULL, NULL, SNRMQ, NULL}, 
  {NULL, NULL, NULL, NULL, PROT, NULL}, 
  {NULL, NULL, NULL, NULL, REV, NULL}, 
  {NULL, NULL, NULL, NULL, INVIS, NULL}, 
  {NULL, NULL, NULL, NULL, SSHM, NULL}, 
  {NULL, NULL, NULL, NULL, SMSO, NULL}, 
  {NULL, NULL, NULL, NULL, SSUBM, NULL}, 
  {NULL, NULL, NULL, NULL, SSUPM, NULL}, 
  {NULL, NULL, NULL, NULL, SMUL, NULL}, 
  {NULL, NULL, NULL, NULL, SUM, NULL}, 
  {NULL, NULL, NULL, NULL, SMXON, NULL}, 
  {createEntry, PARAM0, NULL, deleteEntry, ECH, copyEntry}, 
  {createEntry, NULL, NULL, deleteEntry, RMACS, copyEntry}, 
  {NULL, NULL, NULL, NULL, RMAM, NULL}, 
  {NULL, NULL, NULL, NULL, SGR0, NULL}, 
  {NULL, NULL, NULL, NULL, RMCUP, NULL}, 
  {NULL, NULL, NULL, NULL, RMDC, NULL}, 
  {NULL, NULL, NULL, NULL, RWIDM, NULL}, 
  {NULL, NULL, NULL, NULL, RMIR, NULL}, 
  {NULL, NULL, NULL, NULL, RITM, NULL}, 
  {NULL, NULL, NULL, NULL, RLM, NULL}, 
  {NULL, NULL, NULL, NULL, RMICM, NULL}, 
  {NULL, NULL, NULL, NULL, RSHM, NULL}, 
  {NULL, NULL, NULL, NULL, RMSO, NULL}, 
  {NULL, NULL, NULL, NULL, RSUBM, NULL}, 
  {NULL, NULL, NULL, NULL, RSUPM, NULL}, 
  {NULL, NULL, NULL, NULL, RMUL, NULL}, 
  {NULL, NULL, NULL, NULL, RUM, NULL}, 
  {NULL, NULL, NULL, NULL, RMXON, NULL}, 
  {NULL, NULL, NULL, NULL, PAUSE, NULL}, 
  {NULL, NULL, NULL, NULL, HOOK, NULL}, 
  {NULL, NULL, NULL, NULL, FLASH, NULL}, 
  {NULL, NULL, NULL, NULL, FF, NULL}, 
  {NULL, NULL, NULL, NULL, FSL, NULL}, 
  {NULL, NULL, NULL, NULL, WINGO, NULL}, 
  {NULL, NULL, NULL, NULL, HUP, NULL}, 
  {NULL, NULL, NULL, NULL, IS1, NULL}, 
  {NULL, NULL, NULL, NULL, IS2, NULL}, 
  {NULL, NULL, NULL, NULL, IS3, NULL}, 
  {NULL, NULL, NULL, NULL, IF, NULL}, 
  {NULL, NULL, NULL, NULL, IPROG, NULL}, 
  {NULL, NULL, NULL, NULL, INITC, NULL}, 
  {NULL, NULL, NULL, NULL, INITP, NULL}, 
  {NULL, NULL, NULL, NULL, ICH1, NULL}, 
  {NULL, NULL, NULL, NULL, IL1, NULL}, 
  {NULL, NULL, NULL, NULL, IP, NULL}, 
  {NULL, NULL, NULL, NULL, KA1, NULL}, 
  {NULL, NULL, NULL, NULL, KA3, NULL}, 
  {NULL, NULL, NULL, NULL, KB2, NULL}, 
  {NULL, NULL, NULL, NULL, KBS, NULL}, 
  {NULL, NULL, NULL, NULL, KBEG, NULL}, 
  {NULL, NULL, NULL, NULL, KCBT, NULL}, 
  {NULL, NULL, NULL, NULL, KC1, NULL}, 
  {NULL, NULL, NULL, NULL, KC3, NULL}, 
  {NULL, NULL, NULL, NULL, KCAN, NULL}, 
  {NULL, NULL, NULL, NULL, KTBC, NULL}, 
  {NULL, NULL, NULL, NULL, KCLR, NULL}, 
  {NULL, NULL, NULL, NULL, KCLO, NULL}, 
  {NULL, NULL, NULL, NULL, KCMD, NULL}, 
  {NULL, NULL, NULL, NULL, KCPY, NULL}, 
  {NULL, NULL, NULL, NULL, KCRT, NULL}, 
  {NULL, NULL, NULL, NULL, KCTAB, NULL}, 
  {NULL, NULL, NULL, NULL, KDCH1, NULL}, 
  {NULL, NULL, NULL, NULL, KDL1, NULL}, 
  {NULL, NULL, NULL, NULL, KCUD1, NULL}, 
  {NULL, NULL, NULL, NULL, KRMIR, NULL}, 
  {NULL, NULL, NULL, NULL, KEND, NULL}, 
  {NULL, NULL, NULL, NULL, KENT, NULL}, 
  {NULL, NULL, NULL, NULL, KEL, NULL}, 
  {NULL, NULL, NULL, NULL, KED, NULL}, 
  {NULL, NULL, NULL, NULL, KEXT, NULL}, 
  {NULL, NULL, NULL, NULL, KF0, NULL}, 
  {NULL, NULL, NULL, NULL, KF1, NULL}, 
  {NULL, NULL, NULL, NULL, KF10, NULL}, 
  {NULL, NULL, NULL, NULL, KF11, NULL}, 
  {NULL, NULL, NULL, NULL, KF12, NULL}, 
  {NULL, NULL, NULL, NULL, KF13, NULL}, 
  {NULL, NULL, NULL, NULL, KF14, NULL}, 
  {NULL, NULL, NULL, NULL, KF15, NULL}, 
  {NULL, NULL, NULL, NULL, KF16, NULL}, 
  {NULL, NULL, NULL, NULL, KF17, NULL}, 
  {NULL, NULL, NULL, NULL, KF18, NULL}, 
  {NULL, NULL, NULL, NULL, KF19, NULL}, 
  {NULL, NULL, NULL, NULL, KF2, NULL}, 
  {NULL, NULL, NULL, NULL, KF20, NULL}, 
  {NULL, NULL, NULL, NULL, KF21, NULL}, 
  {NULL, NULL, NULL, NULL, KF22, NULL}, 
  {NULL, NULL, NULL, NULL, KF23, NULL}, 
  {NULL, NULL, NULL, NULL, KF24, NULL}, 
  {NULL, NULL, NULL, NULL, KF25, NULL}, 
  {NULL, NULL, NULL, NULL, KF26, NULL}, 
  {NULL, NULL, NULL, NULL, KF27, NULL}, 
  {NULL, NULL, NULL, NULL, KF28, NULL}, 
  {NULL, NULL, NULL, NULL, KF29, NULL}, 
  {NULL, NULL, NULL, NULL, KF3, NULL}, 
  {NULL, NULL, NULL, NULL, KF30, NULL}, 
  {NULL, NULL, NULL, NULL, KF31, NULL}, 
  {NULL, NULL, NULL, NULL, KF32, NULL}, 
  {NULL, NULL, NULL, NULL, KF33, NULL}, 
  {NULL, NULL, NULL, NULL, KF34, NULL}, 
  {NULL, NULL, NULL, NULL, KF35, NULL}, 
  {NULL, NULL, NULL, NULL, KF36, NULL}, 
  {NULL, NULL, NULL, NULL, KF37, NULL}, 
  {NULL, NULL, NULL, NULL, KF38, NULL}, 
  {NULL, NULL, NULL, NULL, KF39, NULL}, 
  {NULL, NULL, NULL, NULL, KF4, NULL}, 
  {NULL, NULL, NULL, NULL, KF40, NULL}, 
  {NULL, NULL, NULL, NULL, KF41, NULL}, 
  {NULL, NULL, NULL, NULL, KF42, NULL}, 
  {NULL, NULL, NULL, NULL, KF43, NULL}, 
  {NULL, NULL, NULL, NULL, KF44, NULL}, 
  {NULL, NULL, NULL, NULL, KF45, NULL}, 
  {NULL, NULL, NULL, NULL, KF46, NULL}, 
  {NULL, NULL, NULL, NULL, KF47, NULL}, 
  {NULL, NULL, NULL, NULL, KF48, NULL}, 
  {NULL, NULL, NULL, NULL, KF49, NULL}, 
  {NULL, NULL, NULL, NULL, KF5, NULL}, 
  {NULL, NULL, NULL, NULL, KF50, NULL}, 
  {NULL, NULL, NULL, NULL, KF51, NULL}, 
  {NULL, NULL, NULL, NULL, KF52, NULL}, 
  {NULL, NULL, NULL, NULL, KF53, NULL}, 
  {NULL, NULL, NULL, NULL, KF54, NULL}, 
  {NULL, NULL, NULL, NULL, KF55, NULL}, 
  {NULL, NULL, NULL, NULL, KF56, NULL}, 
  {NULL, NULL, NULL, NULL, KF57, NULL}, 
  {NULL, NULL, NULL, NULL, KF58, NULL}, 
  {NULL, NULL, NULL, NULL, KF59, NULL}, 
  {NULL, NULL, NULL, NULL, KF6, NULL}, 
  {NULL, NULL, NULL, NULL, KF60, NULL}, 
  {NULL, NULL, NULL, NULL, KF61, NULL}, 
  {NULL, NULL, NULL, NULL, KF62, NULL}, 
  {NULL, NULL, NULL, NULL, KF63, NULL}, 
  {NULL, NULL, NULL, NULL, KF7, NULL}, 
  {NULL, NULL, NULL, NULL, KF8, NULL}, 
  {NULL, NULL, NULL, NULL, KF9, NULL}, 
  {NULL, NULL, NULL, NULL, KFND, NULL}, 
  {NULL, NULL, NULL, NULL, KHLP, NULL}, 
  {NULL, NULL, NULL, NULL, KHOME, NULL}, 
  {NULL, NULL, NULL, NULL, KICH1, NULL}, 
  {NULL, NULL, NULL, NULL, KIL1, NULL}, 
  {NULL, NULL, NULL, NULL, KCUB1, NULL}, 
  {NULL, NULL, NULL, NULL, KLL, NULL}, 
  {NULL, NULL, NULL, NULL, KMRK, NULL}, 
  {NULL, NULL, NULL, NULL, KMSG, NULL}, 
  {NULL, NULL, NULL, NULL, KMOV, NULL}, 
  {NULL, NULL, NULL, NULL, KNXT, NULL}, 
  {NULL, NULL, NULL, NULL, KNP, NULL}, 
  {NULL, NULL, NULL, NULL, KOPN, NULL}, 
  {NULL, NULL, NULL, NULL, KOPT, NULL}, 
  {NULL, NULL, NULL, NULL, KPP, NULL}, 
  {NULL, NULL, NULL, NULL, KPRV, NULL}, 
  {NULL, NULL, NULL, NULL, KPRT, NULL}, 
  {NULL, NULL, NULL, NULL, KRDO, NULL}, 
  {NULL, NULL, NULL, NULL, KREF, NULL}, 
  {NULL, NULL, NULL, NULL, KRFR, NULL}, 
  {NULL, NULL, NULL, NULL, KRPL, NULL}, 
  {NULL, NULL, NULL, NULL, KRST, NULL}, 
  {NULL, NULL, NULL, NULL, KRES, NULL}, 
  {NULL, NULL, NULL, NULL, KCUF1, NULL}, 
  {NULL, NULL, NULL, NULL, KSAV, NULL}, 
  {NULL, NULL, NULL, NULL, SHIFTKBEG, NULL}, 
  {NULL, NULL, NULL, NULL, SHIFTKCAN, NULL}, 
  {NULL, NULL, NULL, NULL, SHIFTKCMD, NULL}, 
  {NULL, NULL, NULL, NULL, SHIFTKCPY, NULL}, 
  {NULL, NULL, NULL, NULL, SHIFTKCRT, NULL}, 
  {NULL, NULL, NULL, NULL, SHIFTKDC, NULL}, 
  {NULL, NULL, NULL, NULL, SHIFTKDL, NULL}, 
  {NULL, NULL, NULL, NULL, KSLT, NULL}, 
  {NULL, NULL, NULL, NULL, SHIFTKEND, NULL}, 
  {NULL, NULL, NULL, NULL, SHIFTKEOL, NULL}, 
  {NULL, NULL, NULL, NULL, SHIFTKEXT, NULL}, 
  {NULL, NULL, NULL, NULL, KIND, NULL}, 
  {NULL, NULL, NULL, NULL, SHIFTKFND, NULL}, 
  {NULL, NULL, NULL, NULL, SHIFTKHLP, NULL}, 
  {NULL, NULL, NULL, NULL, SHIFTKHOM, NULL}, 
  {NULL, NULL, NULL, NULL, SHIFTKIC, NULL}, 
  {NULL, NULL, NULL, NULL, SHIFTKLFT, NULL}, 
  {NULL, NULL, NULL, NULL, SHIFTKMSG, NULL}, 
  {NULL, NULL, NULL, NULL, SHIFTKMOV, NULL}, 
  {NULL, NULL, NULL, NULL, SHIFTKNXT, NULL}, 
  {NULL, NULL, NULL, NULL, SHIFTKOPT, NULL}, 
  {NULL, NULL, NULL, NULL, SHIFTKPRV, NULL}, 
  {NULL, NULL, NULL, NULL, SHIFTKPRT, NULL}, 
  {NULL, NULL, NULL, NULL, KRI, NULL}, 
  {NULL, NULL, NULL, NULL, SHIFTKRDO, NULL}, 
  {NULL, NULL, NULL, NULL, SHIFTKRPL, NULL}, 
  {NULL, NULL, NULL, NULL, SHIFTKRIT, NULL}, 
  {NULL, NULL, NULL, NULL, SHIFTKRES, NULL}, 
  {NULL, NULL, NULL, NULL, SHIFTKSAV, NULL}, 
  {NULL, NULL, NULL, NULL, SHIFTKSPD, NULL}, 
  {NULL, NULL, NULL, NULL, KHTS, NULL}, 
  {NULL, NULL, NULL, NULL, SHIFTKUND, NULL}, 
  {NULL, NULL, NULL, NULL, KSPD, NULL}, 
  {NULL, NULL, NULL, NULL, KUND, NULL}, 
  {NULL, NULL, NULL, NULL, KCUU1, NULL}, 
  {NULL, NULL, NULL, NULL, RMKX, NULL}, 
  {NULL, NULL, NULL, NULL, SMKX, NULL}, 
  {NULL, NULL, NULL, NULL, LF0, NULL}, 
  {NULL, NULL, NULL, NULL, LF1, NULL}, 
  {NULL, NULL, NULL, NULL, LF10, NULL}, 
  {NULL, NULL, NULL, NULL, LF2, NULL}, 
  {NULL, NULL, NULL, NULL, LF3, NULL}, 
  {NULL, NULL, NULL, NULL, LF4, NULL}, 
  {NULL, NULL, NULL, NULL, LF5, NULL}, 
  {NULL, NULL, NULL, NULL, LF6, NULL}, 
  {NULL, NULL, NULL, NULL, LF7, NULL}, 
  {NULL, NULL, NULL, NULL, LF8, NULL}, 
  {NULL, NULL, NULL, NULL, LF9, NULL}, 
  {NULL, NULL, NULL, NULL, FLN, NULL}, 
  {NULL, NULL, NULL, NULL, RMLN, NULL}, 
  {NULL, NULL, NULL, NULL, SMLN, NULL}, 
  {NULL, NULL, NULL, NULL, RMM, NULL}, 
  {NULL, NULL, NULL, NULL, SMM, NULL}, 
  {NULL, NULL, NULL, NULL, MHPA, NULL}, 
  {NULL, NULL, NULL, NULL, MCUD1, NULL}, 
  {NULL, NULL, NULL, NULL, MCUB1, NULL}, 
  {NULL, NULL, NULL, NULL, MCUF1, NULL}, 
  {NULL, NULL, NULL, NULL, MVPA, NULL}, 
  {NULL, NULL, NULL, NULL, MCUU1, NULL}, 
  {createEntry, NULL, NULL, deleteEntry, NEL, copyEntry}, 
  {NULL, NULL, NULL, NULL, PORDER, NULL}, 
  {NULL, NULL, NULL, NULL, OC, NULL}, 
  {NULL, NULL, NULL, NULL, OP, NULL}, 
  {NULL, NULL, NULL, NULL, PAD, NULL}, 
  {createEntry, PARAM0, NULL, deleteEntry, DCH, copyEntry}, 
  {createEntry, PARAM0, NULL, deleteEntry, DL, copyEntry}, 
  {NULL, NULL, NULL, NULL, CUD, NULL}, 
  {NULL, NULL, NULL, NULL, MCUD, NULL}, 
  {createEntry, PARAM0, NULL, deleteEntry, ICH, copyEntry}, 
  {NULL, NULL, NULL, NULL, INDN, NULL}, 
  {createEntry, PARAM0, NULL, deleteEntry, IL, copyEntry}, 
  {NULL, NULL, NULL, NULL, CUB, NULL}, 
  {NULL, NULL, NULL, NULL, MCUB, NULL}, 
  {NULL, NULL, NULL, NULL, CUF, NULL}, 
  {NULL, NULL, NULL, NULL, MCUF, NULL}, 
  {NULL, NULL, NULL, NULL, RIN, NULL}, 
  {createEntry, PARAM0, NULL, deleteEntry, CUU, copyEntry}, 
  {NULL, NULL, NULL, NULL, MCUU, NULL}, 
  {NULL, NULL, NULL, NULL, PFKEY, NULL}, 
  {NULL, NULL, NULL, NULL, PFLOC, NULL}, 
  {NULL, NULL, NULL, NULL, PFX, NULL}, 
  {NULL, NULL, NULL, NULL, PLN, NULL}, 
  {NULL, NULL, NULL, NULL, MC0, NULL}, 
  {NULL, NULL, NULL, NULL, MC5P, NULL}, 
  {NULL, NULL, NULL, NULL, MC4, NULL}, 
  {NULL, NULL, NULL, NULL, MC5, NULL}, 
  {NULL, NULL, NULL, NULL, PULSE, NULL}, 
  {NULL, NULL, NULL, NULL, QDIAL, NULL}, 
  {NULL, NULL, NULL, NULL, RMCLK, NULL}, 
  {NULL, NULL, NULL, NULL, REP, NULL}, 
  {NULL, NULL, NULL, NULL, RFI, NULL}, 
  {NULL, NULL, NULL, NULL, RS1, NULL}, 
  {NULL, NULL, NULL, NULL, RS2, NULL}, 
  {NULL, NULL, NULL, NULL, RS3, NULL}, 
  {NULL, NULL, NULL, NULL, RF, NULL}, 
  {createEntry, NULL, NULL, deleteEntry, RC, copyEntry}, 
  {createEntry, PARAM0, NULL, deleteEntry, VPA, copyEntry}, 
  {createEntry, NULL, NULL, deleteEntry, SC, copyEntry}, 
  {NULL, NULL, NULL, NULL, IND, NULL}, 
  {NULL, NULL, NULL, NULL, RI, NULL}, 
  {NULL, NULL, NULL, NULL, SCS, NULL}, 
  {create_SGR_Entry, (void*)&TheCurrentStyle, NULL, delete_SGR_Entry, SGR, copy_SGR_Entry}, 
  {NULL, NULL, NULL, NULL, SETB, NULL}, 
  {NULL, NULL, NULL, NULL, SMGB, NULL}, 
  {NULL, NULL, NULL, NULL, SMGBP, NULL}, 
  {NULL, NULL, NULL, NULL, SCLK, NULL}, 
  {NULL, NULL, NULL, NULL, SCP, NULL}, 
  {NULL, NULL, NULL, NULL, SETF, NULL}, 
  {NULL, NULL, NULL, NULL, SMGL, NULL}, 
  {NULL, NULL, NULL, NULL, SMGLP, NULL}, 
  {NULL, NULL, NULL, NULL, SMGR, NULL}, 
  {NULL, NULL, NULL, NULL, SMGRP, NULL}, 
  {NULL, NULL, NULL, NULL, HTS, NULL}, 
  {NULL, NULL, NULL, NULL, SMGT, NULL}, 
  {NULL, NULL, NULL, NULL, SMGTP, NULL}, 
  {NULL, NULL, NULL, NULL, WIND, NULL}, 
  {NULL, NULL, NULL, NULL, SBIM, NULL}, 
  {NULL, NULL, NULL, NULL, SCSD, NULL}, 
  {NULL, NULL, NULL, NULL, RBIM, NULL}, 
  {NULL, NULL, NULL, NULL, RCSD, NULL}, 
  {NULL, NULL, NULL, NULL, SUBCS, NULL}, 
  {NULL, NULL, NULL, NULL, SUPCS, NULL}, 
  {NULL, NULL, NULL, NULL, HT, NULL}, 
  {NULL, NULL, NULL, NULL, DOCR, NULL}, 
  {NULL, NULL, NULL, NULL, TSL, NULL}, 
  {NULL, NULL, NULL, NULL, TONE, NULL}, 
  {NULL, NULL, NULL, NULL, UC, NULL}, 
  {NULL, NULL, NULL, NULL, HU, NULL}, 
  {NULL, NULL, NULL, NULL, U0, NULL}, 
  {NULL, NULL, NULL, NULL, U1, NULL}, 
  {NULL, NULL, NULL, NULL, U2, NULL}, 
  {NULL, NULL, NULL, NULL, U3, NULL}, 
  {NULL, NULL, NULL, NULL, U4, NULL}, 
  {NULL, NULL, NULL, NULL, U5, NULL}, 
  {NULL, NULL, NULL, NULL, U6, NULL}, 
  {NULL, NULL, NULL, NULL, U7, NULL}, 
  {NULL, NULL, NULL, NULL, U8, NULL}, 
  {NULL, NULL, NULL, NULL, U9, NULL}, 
  {NULL, NULL, NULL, NULL, WAIT, NULL}, 
  {NULL, NULL, NULL, NULL, XOFFC, NULL}, 
  {NULL, NULL, NULL, NULL, XONC, NULL}, 
  {NULL, NULL, NULL, NULL, ZEROM, NULL}, 
  {NULL, NULL, NULL, NULL, SCESA, NULL}, 
  {NULL, NULL, NULL, NULL, BICR, NULL}, 
  {NULL, NULL, NULL, NULL, BINEL, NULL}, 
  {NULL, NULL, NULL, NULL, BIREP, NULL}, 
  {NULL, NULL, NULL, NULL, CSNM, NULL}, 
  {NULL, NULL, NULL, NULL, CSIN, NULL}, 
  {NULL, NULL, NULL, NULL, COLORNM, NULL}, 
  {NULL, NULL, NULL, NULL, DEFBI, NULL}, 
  {NULL, NULL, NULL, NULL, DEVT, NULL}, 
  {NULL, NULL, NULL, NULL, DISPC, NULL}, 
  {NULL, NULL, NULL, NULL, ENDBI, NULL}, 
  {NULL, NULL, NULL, NULL, SMPCH, NULL}, 
  {NULL, NULL, NULL, NULL, SMSC, NULL}, 
  {NULL, NULL, NULL, NULL, RMPCH, NULL}, 
  {NULL, NULL, NULL, NULL, RMSC, NULL}, 
  {NULL, NULL, NULL, NULL, GETM, NULL}, 
  {NULL, NULL, NULL, NULL, KMOUS, NULL}, 
  {NULL, NULL, NULL, NULL, MINFO, NULL}, 
  {NULL, NULL, NULL, NULL, PCTRM, NULL}, 
  {NULL, NULL, NULL, NULL, PFXL, NULL}, 
  {NULL, NULL, NULL, NULL, REQMP, NULL}, 
  {NULL, NULL, NULL, NULL, SCESC, NULL}, 
  {NULL, NULL, NULL, NULL, S0DS, NULL}, 
  {NULL, NULL, NULL, NULL, S1DS, NULL}, 
  {NULL, NULL, NULL, NULL, S2DS, NULL}, 
  {NULL, NULL, NULL, NULL, S3DS, NULL}, 
  {NULL, NULL, NULL, NULL, SETAB, NULL}, 
  {NULL, NULL, NULL, NULL, SETAF, NULL}, 
  {NULL, NULL, NULL, NULL, SETCOLOR, NULL}, 
  {NULL, NULL, NULL, NULL, SMGLR, NULL}, 
  {NULL, NULL, NULL, NULL, SLINES, NULL}, 
  {NULL, NULL, NULL, NULL, SMGTB, NULL}, 
  {NULL, NULL, NULL, NULL, EHHLM, NULL}, 
  {NULL, NULL, NULL, NULL, ELHLM, NULL}, 
  {NULL, NULL, NULL, NULL, ELOHLM, NULL}, 
  {NULL, NULL, NULL, NULL, ERHLM, NULL}, 
  {NULL, NULL, NULL, NULL, ETHLM, NULL}, 
  {NULL, NULL, NULL, NULL, EVHLM, NULL}, 
  {NULL, NULL, NULL, NULL, SGR1, NULL}, 
  {NULL, NULL, NULL, NULL, SLENGTH, NULL}, 
  {NULL, NULL, NULL, deleteEntry, TSAR, copyEntry}, 
  {createText, NULL, NULL, deleteText, TEXTFIELD, copyText} 
};
/* > */
/* < initTerminfo2list */
void initTerminfo2List( style* theDefaultStyle)
{
  ENTER("initTerminfo2List");
  copyStyle( &TheDefaultStyle, theDefaultStyle);
  copyStyle( &TheCurrentStyle, theDefaultStyle);
}
/* > */
/* < convertTerminfo2List */

/* GList* convertTerminfo2List( FILE* theStream) */
GList* convertTerminfo2List( char* theBuffer, int theLength)
{
  enum StringCapacity aCapacity;

  ENTER("convertTerminfo2List");

/*   yyin=theStream; */
  initEscape2Terminfo( theBuffer, theLength);

  myList=NULL;
  myPreviousCapacity=ACSC; /* init: anything but textfield.*/

  while((aCapacity=yylex()))
    {
      entryCommands* aCommand=NULL;

      DISPLAY_CAPACITY( aCapacity);
      
      aCommand=TheEntryCommands+aCapacity;
      
      /* check consistency */
      if ((aCapacity > LASTENUM)
	  || (aCommand->myCreateEntryHandler == NULL)
	  || (aCommand->myDeleteEntryHandler == NULL)
	  || (aCommand->myCapacity != aCapacity)
	  || (aCommand->myCopyEntryHandler == NULL)
	  )
	{
	  SHOW("Exit");
	  exit(0);
	}
      
      aCommand->myCreateEntryHandler( aCapacity, aCommand->myData1, aCommand->myData2);
      myPreviousCapacity=aCapacity;
    }

  finishEscape2Terminfo();

  /* update the myParent field */
  {
  GList* aList = myList;
  while(aList)
    {
      ((terminfoEntry*)(aList->data))->myParent = aList;

      SHOW3("myParent=%x, myEscapeSequence=%s\n", (unsigned int)((terminfoEntry*)(aList->data))->myParent, ((terminfoEntry*)(aList->data))->myEscapeSequence);

      aList = aList->next;
    }
  }
  return myList;
}
/* > */
/* < deleteTermInfoList */

static void deleteListEntry(gpointer theEntry, gpointer userData)
{
  ENTER("deleteListEntry");

  if (theEntry)
    {
      entryCommands* aCommand=TheEntryCommands 
	+ ((terminfoEntry*)theEntry)->myCapacity;

      if (aCommand && aCommand->myDeleteEntryHandler)
	{
	  aCommand->myDeleteEntryHandler(theEntry);
	}
    }
}

void deleteTermInfoList(GList* theList)
{
  ENTER("deleteTermInfoList");

  g_list_foreach(theList, (GFunc)deleteListEntry, NULL);
  g_list_free(theList);
}
/* > */
/* < copyTerminfoList */
GList* copyTerminfoList(GList* theList)
{
  GList* aList=NULL;

  ENTER("copyTerminfoList");

  aList=g_list_copy( theList);

  if (aList)
    {
      GList* aListItem = aList;

      do {
	terminfoEntry* anEntry = (terminfoEntry*)aListItem->data;
	entryCommands* aCommand = TheEntryCommands 
	  + ((terminfoEntry*)anEntry)->myCapacity;

	if (aCommand && aCommand->myCopyEntryHandler)
	  {
	    aListItem->data = aCommand->myCopyEntryHandler( anEntry);
	  }

	aListItem = g_list_next( aListItem);

      } while( aListItem);
    }

  return aList;
}

/* > */
/* < convertList2Terminfo */

static void concatByteArray( gpointer theEntry, gpointer theByteArray)
{
  ENTER("concatByteArray");

  if (theEntry && theByteArray)
    {
      terminfoEntry* anEntry = (terminfoEntry*)theEntry;
      chartyp* anEscapeSequence=NULL;
      
      if (anEntry->myCapacity == TEXTFIELD)
	{
	  anEscapeSequence = ((GString*)(anEntry->myData1))->str;
	}
      else
	{
	  anEscapeSequence = anEntry->myEscapeSequence;
	}

      g_byte_array_append( (GByteArray*) theByteArray, 
			   (guint8*) anEscapeSequence, 
			   (guint) strlen(anEscapeSequence));
    }
}

GByteArray* convertList2Terminfo( GList* theList)
{
  GByteArray* aByteArray=g_byte_array_new();

  ENTER("convertList2Terminfo");

   g_list_foreach( theList, (GFunc)concatByteArray, aByteArray);

   return aByteArray;
}

/* > */
/* < addSequence */
GList* addSequence( chartyp* theFirstSequence, GList* theFirstElement, GList* theLastElement, chartyp* theLastSequence)
{
  ENTER("addSequence");

  if (theFirstElement && theLastElement)
    {
      terminfoEntry* anEntry = NULL;
      int aPosition = 0;
      
      /* < First TSAR terminfo */
      anEntry = createExternalEntry( TSAR, NULL, NULL, theFirstSequence);
      aPosition = g_list_position (myList, theFirstElement);
      myList = g_list_insert( myList, 
			      (gpointer) anEntry, 
			      aPosition);

      anEntry->myParent = g_list_nth( myList, aPosition);
      /* > */

      /* < Second terminfo */
      anEntry = createExternalEntry( TSAR, NULL, NULL, theLastSequence);
      aPosition = 1 + g_list_position (myList, theLastElement);
      myList = g_list_insert( myList, 
			      (gpointer) anEntry, 
			      aPosition);
      anEntry->myParent = g_list_nth( myList, aPosition);
      /* > */
    }

  free( theFirstSequence);
  free( theLastSequence);

  return myList;
}
/* > */
/* < setRendering */
terminfoEntry* get_TSAR_Sequence( int theVolume, int theVoice)
{
  /* TSAR format: 
     volume: range 0..9;
     voice: range 0..9
  */
  #define TSAR_FORMAT "\x1B[1%d;%dt"

  int aVolume = (theVolume+5)/10;
  int aVoice = 0;
  char aSequence[ sizeof( TSAR_FORMAT)];
  terminfoEntry* anEntry = NULL;

  ENTER("get_TSAR_Sequence");

  aVolume = CLAMP(aVolume, 0, 9);
  aVoice = CLAMP(theVoice, 0, 9);

  sprintf( aSequence, TSAR_FORMAT, aVolume, aVoice);
  anEntry = createExternalEntry( TSAR, NULL, NULL, aSequence);

  SHOW4("theVolume=%d, theVoice=%d, anEntry=%x\n", theVolume, theVoice, (int)anEntry);

  return anEntry;
}
/* > */
/* < getTextEntry */
terminfoEntry* getTextEntry( char* theText)
{
  GString* aString = g_string_new( theText);
  char *aSequence = strdup( theText);
  terminfoEntry* anEntry = NULL;

  ENTER("getTextEntry");

  anEntry = createExternalEntry( TEXTFIELD, NULL, NULL, aSequence);

  if (anEntry)
    {
      anEntry->myData1 = (void*)aString;
    }
  else
    {
      g_string_free( aString, 1);
      free( aSequence);
    }

  return anEntry;
}
/* > */
/* < getPositionEntry */
terminfoEntry* getPositionEntry( int theLine, int theCol)
{
  #define CUP_FORMAT "\x1B[%d;%dH"
  char *aSequence = malloc( sizeof(CUP_FORMAT) + 10);
  terminfoEntry* anEntry = NULL;
  int aLine = (theLine+1) % 1000;
  int aCol = (theCol+1) % 1000;

  ENTER("getPositionEntry");

  if (!aSequence)
      {
	return NULL;
      }

  sprintf( aSequence, CUP_FORMAT, aLine, aCol);
  anEntry = createExternalEntry( CUP, NULL, NULL, aSequence);

  if (anEntry)
    {
      anEntry->myData1 = (void*)(aLine-1);
      anEntry->myData2 = (void*)(aCol-1);
    }
  else
    {
      free(aSequence);
    }

  return anEntry;
}
/* > */
/* < getStyleEntry */
terminfoEntry* getStyleEntry( style* theStyle)
{
  terminfoEntry* anEntry = NULL;
  GString* aString = NULL;
  style* aStyle = NULL;
  char* aSequence = NULL;

  ENTER("getStyleEntry");

  /* < build the escape sequence */
  aString = getSGR( theStyle);
  if (!aString)
    {
      goto exit_getStyleEntry;
    }

  aSequence = aString->str;
  if (!aSequence)
    {
      goto exit_getStyleEntry;
    }
  /* > */

  aStyle = (style*)malloc(sizeof(style));
  if (!aStyle)
    {
      goto exit_getStyleEntry;
    }

  copyStyle( aStyle, theStyle);

  anEntry = createExternalEntry( SGR, NULL, NULL, aSequence);
  
 exit_getStyleEntry:
  if (aString)
    {
      g_string_free( aString, 0);
    }

  if (anEntry)
    {
      anEntry->myData1 = (void*)aStyle;
    }
  else
    {
      free(aSequence);
      free(aStyle);
    }

  return anEntry;
}
/* > */

/* 
Local variables:
folded-file: t
folding-internal-margins: nil
*/
