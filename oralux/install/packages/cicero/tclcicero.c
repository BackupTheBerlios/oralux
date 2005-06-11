/* tcldtk.c updated for cicero */
/* <copyright*/

/**
 *Copyright (C) 1995 -- 2003, T. V. Raman 
 *All Rights Reserved
 *
 * This file is not part of GNU Emacs, but the same permissions apply.
 *
 * GNU Emacs is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * GNU Emacs is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Emacs; see the file COPYING.  If not, write to
 * the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 */
/*
// ###
// Initial release: Gilles Casse <gcasse@oralux.org> 30 April 2005

L'init crée un processus qui lance en fait tts_shell.py.

Il y a trois principales fonctions d'implémentées:

- Tclcicero_Init: crée un tube (pipe), crée le processus, crée les commandes tcl (Say, Stop, etc...).

- Say: reçoit le texte à prononcer. 
Il convertit le texte en iso8859-1, filtre les commandes en ligne Emacspeak ([:np], etc...) puis écrit le texte dans le tube.

- Stop: Elle écrit un '\n' dans le tube. 

// ###
*/

/* > */
/* <headers*/

#include <tcl.h>
#include <tclExtend.h>
#include <stdlib.h>
#include <string.h>

/* > */

/* debug */
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


/* <defines*/

#define CICERO_TTS "/usr/local/share/cicero/tts_shell.py"

#define PACKAGENAME "tts"
#define PACKAGEVERSION "1.0"

#define EXPORT
extern  EXPORT int Tclcicero_Init(Tcl_Interp *interp);

#define USE_LOG_FILE 0
#define LOG_FILE_NAME "/tmp/tclcicero.log"
#define DEBUG_LEVEL 10

/* > */
/* <prototypes*/

int openLog();
void debugMsg(char *, int, char *);
void TclCiceroFree(ClientData);
int Say(ClientData, Tcl_Interp *, int, Tcl_Obj * CONST []);
int Stop(ClientData, Tcl_Interp *, int, Tcl_Obj * CONST []);
int Set_Language(ClientData, Tcl_Interp *, int, Tcl_Obj * CONST []);

int Synchronize(ClientData, Tcl_Interp *, int, Tcl_Obj * CONST []);
int Pause(ClientData, Tcl_Interp *, int, Tcl_Obj * CONST []);
int Resume(ClientData, Tcl_Interp *, int, Tcl_Obj * CONST []);

/* > */
/* <global variables*/

char *error_msg;
char error_buff[80];
FILE *LOG = NULL;

/* > */
/* <openLog*/

int openLog(void) 
{
   if (USE_LOG_FILE) 
    {
      LOG = fopen(LOG_FILE_NAME, "w");
      setvbuf(LOG, NULL, _IONBF, 0);
      if (LOG == NULL) 
	{
	  return TCL_ERROR;
	}
      return TCL_OK;
    } 
  else
    {
      LOG = stderr;
    }
  return TCL_OK;
}


/* > */
/* <debugMsg*/

void debugMsg(char *id, int level, char *msg) {

  /* return; */
  if (level <= DEBUG_LEVEL) {
    fprintf(LOG, "%s: %s\n", id, msg);
  }
}

/* > */


/* <closing down*/

void TclCiceroFree(ClientData ciceroHandle) 
{
  debugMsg("TclCiceroFree", 10, "Entering function");
  /* RAF */
}

/* > */

static int fd_0_1[2];
static pid_t myProcess1=0;  
extern char **environ;

#include <sys/time.h>
#include <time.h>
static double lastClock=0;

void ciceroInit()
{
  pipe(fd_0_1);

  if ((myProcess1=fork()) == 0) 
    {
      close(fd_0_1[1]);
      dup2(fd_0_1[0], STDIN_FILENO); 
      system(CICERO_TTS);
    }
  else
    {
      /* Current process */
      close(fd_0_1[0]);
    }
}

double getCurrentDelay()
{
  double aDelay;
  struct timeval tv;
  gettimeofday( &tv, NULL);
  aDelay=tv.tv_usec/1000000.0;
  aDelay+=tv.tv_sec;
  fprintf(LOG,"Clock:%f\n", aDelay);
  return aDelay;
}

int delayAchieved()
{
  int aStatus=0;
  double currentClock=getCurrentDelay();
  if (lastClock + 3.0 <= currentClock)
    {
      aStatus=1;
      lastClock=currentClock;
      fprintf(LOG,"delayAchieved: status=%d\n", aStatus);
    }
  return aStatus;
}

/* The following code comes from ParleMax, helpful to filter the not yet useful Emacspeak inline commands */
/*
 * Filtered strings
 */
static char delims_start = '[';
static char delims_end = ']';
static char *delims[4] = { ":np", " :dv", ":nh", " :pu" };
static int ldelims[4] = { sizeof(delims[0])-1, sizeof(delims[1])-1, sizeof(delims[2])-1 };

/*
 * search_np() : looks for an inline string until the ']' character, 
 * and returns the address of the next character.
 * NULL is returned if the string doesn't match the pattern.
 * pcur: points to the first char of the ('[') string
 */
char* search_np(char* pcur)
{
  int idelim = 0;
  char* pdeb=pcur;
  char* pnew=NULL;

  for (idelim = 0; idelim < sizeof(delims) / sizeof(delims[0]); idelim++ ) {
    pcur=pdeb+1; 
    
    /* is the separator found */
    if (strncmp(pcur, delims[idelim], ldelims[idelim]) == 0) {
      pcur += ldelims[idelim];
      
      /* looking for the ']' */
      while (*pcur && (*pcur != delims_end)) {
	++pcur;
      }
      
      if (*pcur == delims_end) {
	pnew=++pcur; /* Ok! */
	break;
      }
    }
  }
  return pnew;
}

/* filter the Emacspeak specific inline strings ([:np],...) */
void strip_np(char * param)
{
  char        *pnew = NULL;
  char        *pcur = param;

  if (! param) {
      return;
  }

  while (*pcur) {
    if ((*pcur == delims_start) 
      && (pnew = search_np(pcur))) {
      memmove(pcur, pnew, strlen(pnew) + 1);

      fprintf(stderr, "paramètre nettoyé : %s\n", param);
      pcur--; /* traiter le nouveau caractere (issu de la recopie).  */
    }
    pcur++;
  }
}



/* <init*/
int Tclcicero_Init(Tcl_Interp *interp) {

  openLog();
  debugMsg("Tclcicero_Init", 10, "Creating Tcl commands");

  ciceroInit();

  /* NULL instead of (ClientData) ciceroHandle*/
  Tcl_CreateObjCommand(interp, "say", Say, NULL, TclCiceroFree);
  Tcl_CreateObjCommand(interp, "synth", Say, NULL, NULL);
  Tcl_CreateObjCommand(interp, "synchronize", Synchronize,
		       NULL, TclCiceroFree);
  Tcl_CreateObjCommand(interp,"stop", Stop, NULL, TclCiceroFree);
  Tcl_CreateObjCommand(interp, "pause", Pause,
		       NULL, TclCiceroFree);
  Tcl_CreateObjCommand(interp, "resume", Resume,
		       NULL, TclCiceroFree);
  Tcl_CreateObjCommand(interp, "set_language", Set_Language, NULL, NULL);
  debugMsg("Tclcicero_Init", 10, "Completed successfully");
  return TCL_OK;
}

/* > */
/* <say*/

int Say(ClientData ciceroHandle, Tcl_Interp *interp, int objc,
		Tcl_Obj *CONST objv[]) {
  int i, length;
  char *txt = NULL;

  debugMsg("Say", 10, "Entering function");

  for (i=1; i<objc; i++) 
    {
      sprintf(error_buff, "For loop - %d. objc = %d", i, objc);
      debugMsg("Say", 6, error_buff);
      txt = Tcl_GetStringFromObj(objv[i], &length);
      sprintf(error_buff, "String length is %d", length);
      debugMsg("Say loop", 6, error_buff);
      sprintf(error_buff, "Tcl obj %d. String = %s\n", i, txt);
      debugMsg("Say", 6, error_buff);
      if (Tcl_StringMatch(txt, "-reset")) 
	{
	  /* RAF */
	} 
      else 
	{
	  #define DESTLEN 4096
	  char *src = Tcl_GetStringFromObj(objv[i], NULL);
	  int srcLen=-1;
	  char *dst=NULL;
	  int srcReadPtr=0;
	  int dstWrotePtr=0;
	  int dstCharsPtr=0;

	  /* Converting from UTF8 to iso-8859-1 */

	  Tcl_Encoding encoding=Tcl_GetEncoding(interp, "iso8859-1");
	  dst=malloc(DESTLEN);
	  Tcl_UtfToExternal(interp, encoding, 
			    src, srcLen, 
			    0, NULL, 
			    dst, DESTLEN,
			    &srcReadPtr, &dstWrotePtr, &dstCharsPtr);
	  
	  /* Removing the Emacspeak inline commands */
	  strip_np(dst);

	  /* Send the text to Cicero */
	  write(fd_0_1[1],dst,strlen(dst));

	  free(dst);
	}
    }
  
  if (Tcl_StringMatch(Tcl_GetStringFromObj(objv[0],NULL), "synth")) 
    {
      debugMsg("Say synth", 6, "Forcing text with TTSSpeak");
      /* RAF */
    }
  debugMsg("Say", 10, "Successful return from function");
  return TCL_OK;
}

/* > */
/* < sync*/

int Synchronize(ClientData ciceroHandle, Tcl_Interp *interp,
                int objc, Tcl_Obj *CONST objv[]) {
 
  debugMsg("Synchronize", 10, "Entering function");
  /* RAF */
  return TCL_OK;
}

/* > */
/* <stop*/

int Stop(ClientData ciceroHandle, Tcl_Interp *interp,
		 int objc, Tcl_Obj *CONST objv[]) {

  debugMsg("Stop", 10, "Entering function");

  /* The following comment causes the issue (speech is delayed) */
  write(fd_0_1[1],"\n",1);

  return TCL_OK;
}

/* > */
/* < pause and resume */

int Pause(ClientData ciceroHandle, Tcl_Interp *interp,
		  int objc, Tcl_Obj *CONST objv[]) {
  static int nb=0;

  if (!nb)
  {
    nb=1;
    return TCL_OK;
  }

  debugMsg("Pause", 10, "Entering function");
  /* RAF */
  return TCL_OK;
}

int Resume(ClientData ciceroHandle, Tcl_Interp *interp,
		   int objc, Tcl_Obj *CONST objv[]) {

  debugMsg("resume", 10, "Entering function");
  /* RAF */
  return TCL_OK;
}

/* > */
/* < Set_Language */

int Set_Language(ClientData ciceroHandle, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]) 
{
  return TCL_OK;
}

/* > */
/* <end of file*/
/* local variables: */
/* folded-file: t */
/* end: */
/* > */
