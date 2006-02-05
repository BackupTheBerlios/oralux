// ----------------------------------------------------------------------------
// audioFifo.c
// $Id: audioFifo.c,v 1.4 2006/02/05 21:03:37 gcasse Exp $
// $Author: gcasse $
// Description: Playing the supplied OggVorbis files
// $Date: 2006/02/05 21:03:37 $ |
// $Revision: 1.4 $ |
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <linux/limits.h>
#include "constants.h"

static pid_t myProcess2=0;  
extern char** environ;
static int myFileToPlay=0;
static int myPlayedFile=0;
static char* mySoundDevice[]=
  {
    "/dev/sound/dsp",
    "/dev/dsp"
  };
static int mySoundDeviceIndex=0;
static int myModeIsSynchronous=0;

#define BID 1
#ifdef BID
#undef ENTER
#define ENTER(a)

#undef LEAVE
#define LEAVE(a)

#undef LEAVE2
#define LEAVE2(a,b)
#endif

enum messageType {
  PLAY_FILE, /* Play the supplied Ogg file */
  PLAY_UNIQUE_FILE, /* Identical and contiguous ogg files will be played but not stored */
  RESAY_ALL, /* Play again the Ogg files */
  CLEAR_ALL /* Clear the list of Ogg files */
};  

enum {MaxFilenameLength = PIPE_BUF-32}; /* Lower than the atomic size for the pipe */
struct messageId
{
  enum messageType myType;
  int myPlayIsSynchronous;
  int myStringLength;
  char myFilename[MaxFilenameLength];
};

static int getFreeFileID()
{
  ENTER ("getFreeFileID");
  int aFreeFile=0;
  if ((1 != myPlayedFile) && (1 != myFileToPlay))
    {
      aFreeFile=1;
    }
  else if ((2 != myPlayedFile) && (2 != myFileToPlay))
    {
      aFreeFile=2;
    }
  LEAVE2 ("getFreeFileID", aFreeFile);
  return aFreeFile;
}

static char* myWavFilename[]=
  {
    "/tmp/fic1.wav",
    "/tmp/fic2.wav",
  };

static void convertOggToWav( char* theFilename, int theDestinationFileID)
{
  ENTER ("convertOggToWav");
  static char buf[1024];

  sprintf(buf,"sox %s %s", theFilename, myWavFilename[ theDestinationFileID-1 ]);
  SHOW2("Creating the WAV file: %s",buf);
  system(buf);

  LEAVE ("convertOggToWav");
}

/* Managing the OggVorbis files */

enum {MaxOgg=60}; // 20 would be too small for the DECtalk serial number
static char* myOgg[ MaxOgg ];
static int myOggWriteIndex=0;
static int myOggReadIndex=0;

static void initOgg()
{
  int i;
  for (i=0; i<MaxOgg; i++)
    {
      myOgg[ i ]=NULL;
    }
  myOggWriteIndex=0;
  myOggReadIndex=0;
}

static void clearOgg()
{
  ENTER ("clearOgg");
  int i;
  for (i=0; i<MaxOgg; i++)
    {
      //      SHOW3("\nfree myOgg[%d]=%x\n", i, myOgg[ i ]);
      free(myOgg[ i ]);
    }
  initOgg();
}

static int storeOgg( char* theFilename, int theLength, int theDoubleIsFiltered)
{
  ENTER ("storeOgg");
  int aResult=0;

  if ((myOggWriteIndex < MaxOgg)
      && (theFilename[theLength]==0)) /* security */
    {
      if (theDoubleIsFiltered 
	  && (myOggWriteIndex>=1)
	  && (strcmp(theFilename, myOgg[ myOggWriteIndex-1 ])==0))	  
	{
	  SHOW( "Double");
	  if (myOggWriteIndex==myOggReadIndex)
	    {
	      myOggReadIndex--;
	      aResult=1;
	    }
	}
      else
	{
	  char* aBloc=malloc(theLength+1);
	  strcpy(aBloc, theFilename);
	  myOgg[ myOggWriteIndex ] = aBloc;
	  SHOW3("\nmyOgg[%d]=%x\n", myOggWriteIndex, aBloc);
	  myOggWriteIndex++;
	  aResult=1;
	}
    }
  else
    {
      SHOW( "Saturation");
      exit(0);
    }
  LEAVE2 ("storeOgg", aResult);
  return aResult;
}

static int getOgg( char** theFilename)
{
  ENTER ("getOgg");
  int aResult=0;

  if (myOggReadIndex < myOggWriteIndex)
    {
      *theFilename  = myOgg[ myOggReadIndex ];
      myOggReadIndex++;
      aResult=1;
    }
  LEAVE2 ("getOgg", aResult);
  return aResult;
}

static void initReadID()
{
  myOggReadIndex = 0;
}

/* == */
static void playFile()
{
  ENTER ("playFile");
  static char* argv[]={"sh","-c","play /tmp/fic1.wav",NULL};
  char* aOggFilename=NULL;

  if ((myFileToPlay==0) && getOgg( &aOggFilename))
  {
    myFileToPlay = getFreeFileID();
    convertOggToWav( aOggFilename, myFileToPlay);
  }

  if (myProcess2 != 0)
    {
      LEAVE("playFile (already one)");
      return;
    }

  if (myFileToPlay)
    {
      myPlayedFile=myFileToPlay;
      myFileToPlay=0;

      if (mySoundDeviceIndex==0)
	{
	  argv[2]=(myPlayedFile==1) ? 
	    "sox /tmp/fic1.wav -t ossdsp /dev/sound/dsp"
	    : "sox /tmp/fic2.wav -t ossdsp /dev/sound/dsp";      
	}
      else
	{
	  argv[2]=(myPlayedFile==1) ? 
	    "sox /tmp/fic1.wav -t ossdsp /dev/dsp"
	    : "sox /tmp/fic2.wav -t ossdsp /dev/dsp";      
	}

      if ((myProcess2=fork()) == 0) 
	{
	  execve("/bin/sh", argv, environ);
	}
    }
  LEAVE("playFile");
}

static int myProcessStopsAfterLastOgg=0;

static void callbackTerminationProcess1 (int theNumber)
{
  ENTER("callbackTerminationProcess1");
  if (myProcess2)
    {
      signal (SIGCHLD, SIG_DFL);
      kill(myProcess2, theNumber);
      wait(NULL);
/*       signal (theNumber, SIG_DFL); */
/*       raise(theNumber); */
    }
  signal (theNumber, SIG_DFL);
  raise(theNumber);
  LEAVE("callbackTerminationProcess1");
}

static void callbackSoundIsFinished (int theNumber)
{
  int aStatus;
  pid_t aID=0;
  ENTER("callbackSoundIsFinished");

  if (myProcess2==0)
    {
      SHOW2( "LEAVE myProcess2=%d\n", myProcess2);
      return;
    }

  aID = waitpid(myProcess2, & aStatus, WNOHANG);

  if ((aID==0) || (aID==-1))
  {
    SHOW3( "LEAVE callbackSoundIsFinished aID=%d, myProcess2=%d\n", aID, myProcess2);
    return;
  }
  myProcess2=0;
  playFile();

  if (myProcess2==0) 
    {
      if (myProcessStopsAfterLastOgg)
	{
	  SHOW( "LEAVE callbackSoundIsFinished with Silence");
	  raise(SIGTERM);
	}
      else if (myModeIsSynchronous)
	{
	  SHOW2( "LEAVE Synchronous Mode (ppid=%d)", getppid());
	  kill(getppid(),SIGUSR1);
	}
    }

  LEAVE("callbackSoundIsFinished");
}

static void callbackProcessMustStopAfterLastOgg (int theNumber)
{
  ENTER("callbackProcessMustStopAfterLastOgg");
  if (myProcess2==0)
    {
      SHOW( "LEAVE callbackProcessMustStopAfterLastOgg because myProcess2==0");
      raise(SIGTERM);
    }
  else
    {
      myProcessStopsAfterLastOgg=1;  
    }
  LEAVE("callbackProcessMustStopAfterLastOgg");
}

static void callbackOggIsFinished (int theNumber)
{
  ENTER("callbackOggIsFinished");
  myModeIsSynchronous=0;
  LEAVE("callbackOggIsFinished");
}

/* Process */

static void Process1(int fd_0_1[])
{
  ENTER("Process1");
  struct messageId aMessage;
  sigset_t aBlockedSet;
  sigset_t aSavedSet;

  close(fd_0_1[1]);

  sigemptyset(&aBlockedSet);
  sigaddset(&aBlockedSet, SIGCHLD);

  signal (SIGTERM, callbackTerminationProcess1);
  signal (SIGCHLD, callbackSoundIsFinished);
  signal (SIGUSR1, callbackProcessMustStopAfterLastOgg);
  while(1)
    {
      read( fd_0_1[0], &aMessage, sizeof(struct messageId));
  
      sigprocmask( SIG_BLOCK, &aBlockedSet, &aSavedSet);
      switch (aMessage.myType)
	{
	case PLAY_UNIQUE_FILE:
	    SHOW( "PLAY_UNIQUE_FILE");
	case PLAY_FILE:
	  {
	    SHOW( "PLAY_FILE");
	    int aDoubleIsFiltered = (aMessage.myType==PLAY_UNIQUE_FILE);
	    myModeIsSynchronous=aMessage.myPlayIsSynchronous;
	    storeOgg(aMessage.myFilename, aMessage.myStringLength, aDoubleIsFiltered);
	    playFile();
	  }
	  break;

	case RESAY_ALL:
	  SHOW( "RESAY_ALL");
	  myFileToPlay=0;
	  initReadID();
	  if (myProcess2)
	    {
	      kill(myProcess2, SIGTERM);
	    }
	  playFile();
	  break;

	case CLEAR_ALL:
	  {
	    SHOW( "CLEAR_ALL");
	    myFileToPlay=0;
	    clearOgg();
	    if (myProcess2)
	      {
		kill(myProcess2, SIGTERM);
	      }
	  }
	  break;
      
	default:
	  break;
	}
      sigprocmask( SIG_SETMASK, &aSavedSet, NULL);
    }

  LEAVE("Process1");
}

/* External services */

static int fd_0_1[2];

static pid_t myProcess1=0;  
void audioFifoQuit(int theSoundMustBeFinished)
{
  ENTER ("audioFifoQuit");
  int i=0;
  SHOW2("myProcess1=%d\n", myProcess1);
  if (myProcess1)
    {
      int aStatus;
      if (theSoundMustBeFinished)
	{
	  SHOW("myProcess1,SIGUSR1");
	  kill (myProcess1,SIGUSR1);
	}
      else
	{
	  SHOW("myProcess1,SIGTERM");
	  kill (myProcess1,SIGTERM);
	}
      //      waitpid(myProcess1,NULL,0);
      wait(NULL);
    }

  for (i=0; i< sizeof(myWavFilename)/sizeof(myWavFilename[0]); i++)
    {
      unlink (myWavFilename[i]);
      SHOW2("myWavFilename[%d]\n", i);
    }
}

void audioFifoInit()
{
  ENTER ("audioFifoInit");
  struct stat aFileInfo;
  pipe(fd_0_1);

  initOgg();

  // /dev/sound/dsp or /dev/dsp ?
  mySoundDeviceIndex=1;
  if (stat(mySoundDevice[0], &aFileInfo)==0)
    {
      mySoundDeviceIndex=S_ISCHR(aFileInfo.st_mode) ? 0:1;
      SHOW3("\ndev=%s index=%d",mySoundDevice[0],mySoundDeviceIndex);
    }

  if ((myProcess1=fork()) == 0) 
    {
      Process1(fd_0_1);
    }
  else
    {
      // Current process
      close(fd_0_1[0]);
      signal (SIGUSR1, callbackOggIsFinished);
    }

  LEAVE("audioFifoInit");
}

void playOggVorbisFile( char* theFilename, int theDoubleIsFiltered, int theCallIsSynchronous)
{
  ENTER ("playOggVorbisFile");
  struct messageId aMessage;
  int aLength=strlen(theFilename);

  if( aLength >= MaxFilenameLength )
    {
      SHOW2("Too long: %s",theFilename);
      return;
    }

  aMessage.myType=theDoubleIsFiltered ? PLAY_UNIQUE_FILE : PLAY_FILE;
  aMessage.myPlayIsSynchronous=theCallIsSynchronous;
  aMessage.myStringLength=aLength;

  strcpy( aMessage.myFilename, theFilename);
  write( fd_0_1[1], &aMessage, sizeof(struct messageId));

  if (theCallIsSynchronous)
    {
      sigset_t aBlockedSet;
      sigset_t aSavedSet;
      sigemptyset(&aBlockedSet);
      sigaddset(&aBlockedSet, SIGUSR1);
      sigprocmask( SIG_BLOCK, &aBlockedSet, &aSavedSet);
      sigdelset(&aSavedSet, SIGUSR1);
      myModeIsSynchronous=1;
      while(myModeIsSynchronous)
	{
	  sigsuspend(&aSavedSet);
	}
      sigprocmask( SIG_SETMASK, &aSavedSet, NULL);
    }

  LEAVE("playOggVorbisFile");
}

void clearAll()
{
  ENTER ("clearAll");
  struct messageId aMessage;
  aMessage.myType=CLEAR_ALL;
  aMessage.myStringLength=0;
  write(fd_0_1[1], &aMessage, sizeof(struct messageId));
  LEAVE("clearAll");
}

void resayAll()
{
  ENTER ("resayAll");
  struct messageId aMessage;
  aMessage.myType=RESAY_ALL;
  aMessage.myStringLength=0;
  write(fd_0_1[1], &aMessage, sizeof(struct messageId));
  LEAVE("resayAll");
}
