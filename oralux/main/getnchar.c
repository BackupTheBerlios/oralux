// ----------------------------------------------------------------------------
// main.c
// $Id: getnchar.c,v 1.2 2005/01/30 21:43:51 gcasse Exp $
// $Author: gcasse $
// Description: capturing each key pressed with aural feedback
// $Date: 2005/01/30 21:43:51 $ |
// $Revision: 1.2 $ |
// Copyright (C) 2003, 2004, 2005 Gilles Casse (gcasse@oralux.org)
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// ----------------------------------------------------------------------------
#include <sys/time.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <termios.h>
#include <unistd.h>
#include "audioUserInterface.h"
#include "getnchar.h"

/* These need to have file scope so that we can use them in
 * signal handlers */
/* old port termios settings to restore */
static struct termios pots; 

/* old stdout/in termios settings to restore */
static struct termios sots; 
static void restoreTerminal(int pf, int signal);

static enum GNC_Answer TheLastKeyPressed=GNC_Ok;

static void Exit(int theValue, int pf)
{
  ENTER("Exit");
  /* restore original terminal settings */
  restoreTerminal(pf,0);
  exit(theValue);
}

void initTerminal(int pf, int theDelayInTenthOfMilliSeconds)
{
  ENTER("initTerminal");
  struct termios pts; /* termios settings on port */
  struct termios sts; /* termios settings on stdout/in */
  struct sigaction sact;/* used to initialize the signal handler */

  /* modify the port configuration */
  tcgetattr(pf, &pts);
  pots = pts;

  /* some things we want to set arbitrarily */
  pts.c_lflag &= ~ICANON; 
  pts.c_lflag &= ~(ECHO | ECHOCTL | ECHONL);
//   pts.c_cflag |= HUPCL;
  pts.c_cc[VMIN] = 1;
  pts.c_cc[VTIME] = theDelayInTenthOfMilliSeconds;

  /* Standard CR/LF handling: this is a dumb terminal.
   * Do no translation:
   *  no NL -> CR/NL mapping on output, and
   *  no CR -> NL mapping on input.
   */
  pts.c_oflag &= ~ONLCR;
  pts.c_iflag &= ~ICRNL;

  /* Now deal with the local terminal side */
  tcgetattr(STDIN_FILENO, &sts);
  sots = sts;
  /* again, some arbitrary things */
  sts.c_iflag &= ~(BRKINT | ICRNL);
  sts.c_iflag |= IGNBRK;
  sts.c_lflag &= ~ISIG;
  sts.c_cc[VMIN] = 1;
  sts.c_cc[VTIME] = theDelayInTenthOfMilliSeconds;
  sts.c_lflag &= ~ICANON;
  /* no local echo: allow the other end to do the echoing */
  sts.c_lflag &= ~(ECHO | ECHOCTL | ECHONL);

  /* set the signal handler to restore the old
   * termios handler */
  sact.sa_handler = (void *)restoreTerminal; 
  sigaction(SIGHUP, &sact, NULL);
  sigaction(SIGINT, &sact, NULL);
  sigaction(SIGPIPE, &sact, NULL);
  sigaction(SIGTERM, &sact, NULL);

  /* Now set the modified termios settings */
  tcsetattr(pf, TCSANOW, &pts);
  tcsetattr(STDIN_FILENO, TCSANOW, &sts);
}

static void restoreTerminal(int pf, int signal)
{
  ENTER("restoreTerminal");
  enableSound(1);
  tcsetattr(pf, TCSANOW, &pots);
  tcsetattr(STDIN_FILENO, TCSANOW, &sots);
}

// getLastKeyPressed
enum GNC_Answer getLastKeyPressed()
{
  return TheLastKeyPressed;
}

// getnchar 
// get n char from the tty with an auralfeedback for each typed character
// if theNumberOfChars==0, the max internal possible value is chosen.
// theDelayInTenthOfMilliSeconds is useful to have at timeout. 0 means no time-out.
// theCharMustBeSpelled: 1 if true
// status
char* getnchar(int pf, int theNumberOfChars, int theDelayInTenthOfMilliSeconds, int theCharMustBeSpelled, enum GNC_Answer* theStatus, HANDLER theCallback)
{
  ENTER("getnchar");
  static char aResult[BUFSIZE];

  *theStatus=TheLastKeyPressed=GNC_Ok;

  enableSound(theCharMustBeSpelled);

  if (pf < 0)
  {
    fprintf(stderr, "pb whith the pf arg %x\n", pf);
    Exit(1, pf);
  }

  if (theNumberOfChars==0)
  {
    theNumberOfChars=BUFSIZE-1;
  }
  else if (theNumberOfChars<=0)
  {     
    fprintf(stderr, "getnchar: theNumberOfChars=%x", theNumberOfChars);
    Exit(1, pf);
  }

  initTerminal(pf, theDelayInTenthOfMilliSeconds);

  // Reading each char and pronouncing it (or beep if unexpected).
  int anIndex=0;
  int done = 0;
   
  do {
    static char buf[20];
    fd_set ready; /* used for select */

    FD_ZERO(&ready);
    FD_SET(STDIN_FILENO, &ready);
    FD_SET(pf, &ready);
    select(pf+1, &ready, NULL, NULL, NULL);
    if (!FD_ISSET(pf, &ready))
    {
      continue;
    }

    /* pf has characters for us */
    int i = read(pf, buf, BUFSIZE);
    SHOW2("i=%d\n",i);

    buf[i]=0;
    if (i==0)
    {
      done = 1;
      continue;
    }

    if ((i > 1) || !isascii(buf[0]))
    {
      // Left and Down arrow keys: immediate stop
      if ((i==3)
	  &&(buf[0]==0x1b)
	  &&(buf[1]==0x5b)
	  &&((buf[2]==0x41)||(buf[2]==0x42)))
	{
	  *theStatus = TheLastKeyPressed = (buf[2]==0x41) ? GNC_UpArrowKey : GNC_DownArrowKey;
	  if (theCallback)
	    {
	      (theCallback)(buf);
	    }
	  break;
	}

      if (theCallback)
	{
	  (theCallback)(buf);
	}
      continue;
    }
    // For any new pressed key, the hook is called
    if (theCallback)
      {
	(theCallback)(buf);
      }

    SHOW2("buf[0]=%x\n",buf[0]);

    switch(buf[0])
    {
    case 0x7F: // DEL
    case 0x08: // BS
      if (anIndex > 0)
      {
        anIndex--;
        say(Delete);
        sayChar(aResult[ anIndex ]);
      }
      break;
    case 0x03: // ^C
    case 0x04: // ^D
      /* restore original terminal settings */
      Exit(0, pf);
      break;
    case 0x0D:
    case 0x0A:
        aResult[ anIndex ]='\n';
        anIndex++;
        done=1;
      break;
    default:
      if (iscntrl(buf[0]))
      {
        beep();
      }
      else
      {
        aResult[ anIndex ]=buf[0];
        anIndex++;
        sayChar(buf[0]);
      }
      break;
    } //switch
  } while (!done && (anIndex<theNumberOfChars));

  /* restore original terminal settings */
  restoreTerminal(pf,0);
  aResult[ anIndex ]=0;
  return aResult;
}
