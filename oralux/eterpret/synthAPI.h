#ifndef SYNTHAPI_H
#define SYNTHAPI_H
// ----------------------------------------------------------------------------
// synthAPI.h
// $Id: synthAPI.h,v 1.1 2004/09/27 20:29:58 gcasse Exp $
// $Author: gcasse $
// Description: interprets the Emacspeak commands 
// $Date: 2004/09/27 20:29:58 $ |
// $Revision: 1.1 $ |
// Copyright (C) 2003, 2004 Gilles Casse (gcasse@oralux.org)
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

#include "CSS.h"

/*   Paul, */
/*   Harry, */
/*   Frank, */
/*   Dennis, */
/*   Betty, */
/*   Ursula, */
/*   Wendy, */
/*   Rita, */
/*   Kit, */

//   int myRichness;

/* {{{ types */


enum t_typeSynth 
  {
    DECtalkSoftware,
  };

enum t_languageSynth {English, French, German, Spanish};
enum t_portSynth {ttyS0, ttyS1};

enum t_boundarySynth
  {
    SpeakOnEndOfClause, // speak after period, comma, exclamation point or question mark followed by a space
    SpeakOnEndOfWord, // speak after a word followed by a space 
    SpeakOnEndOfLetter, 
  };

struct t_styleSynth
{
  struct t_mixingProperties myMixing;
  enum t_speak mySpeak;
  struct t_pauseProperties myPause;
  //  struct t_cueProperties myCue;
  struct t_markProperties myMark;
  struct t_VoiceProperties myVoice;
  //  int myVoiceDuration; // in milliseconds
  //  struct t_phonetics myPhonetics;
  enum t_boundarySynth myBoundary;
};

struct synthesizer
{
  enum t_typeSynth myType;
  struct t_styleSynth myStyle;
  enum t_languageSynth myLanguage;
  enum t_portSynth myPort;
};

/* }}} */


int setStyleSynth( struct synthesizer* this, struct t_styleSynth theStyle);
int shutdownSynth( struct synthesizer* this);
struct synthesizer* initSynth();
int saySynth( struct synthesizer* this, char* theString, int theOutputIsForced);
int synchronizeSynth( struct synthesizer* this);
int stopSynth( struct synthesizer* this);
int pauseSynth( struct synthesizer* this, int theDelayInMilliseconds);
int resumeSynth( struct synthesizer* this);
int setLanguageSynth( struct synthesizer* this);
int flushSynth( struct synthesizer* this);
int resetSynth( struct synthesizer* this);

// if theRate=50, it means 50 percent of the normal rate
int setSpeechRateSynth( struct synthesizer* this, int theRate);
int setBoundarySynth( struct synthesizer* this, enum t_boundarySynth theBoundary);  

#endif

