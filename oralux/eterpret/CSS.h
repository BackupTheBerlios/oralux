#ifndef CSS_H
#define CSS_H 1
// ----------------------------------------------------------------------------
// CSS.h
// $Id: CSS.h,v 1.1 2004/09/27 20:29:58 gcasse Exp $
// $Author: gcasse $
// Description: CSS3 types 
// This header refers to the CSS3 Speech Module
// W3C Working Draft 27 July 2004
// http://www.w3.org/TR/2004/WD-css3-speech-20040727/
//
// $Date: 2004/09/27 20:29:58 $ |
// $Revision: 1.1 $ |
// Copyright (C) 2004 Gilles Casse (gcasse@oralux.org)
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

/* {{{ t_mixingProperties */

struct t_mixingProperties
{
  int myVoiceVolume; // 0 means silence, 100 is the maximum expected level
  int myVoiceBalance; // from -100 (left) to +100 (right)
};

/* }}} */
/* {{{ t_pauseProperties */

struct t_pauseProperties
{
  int myPauseBefore; // in milliseconds
  int myPauseAfter; // in milliseconds
};

/* }}} */
/* {{{ t_speak */

enum t_speak
  {
    noneSpeak,
    normalSpeak,
    spellOutSpeak,
    digitsSpeak,
    literalPunctuationSpeak,
    noPunctuationSpeak,
    inheritSpeak,
  };

/* }}} */
/* {{{ t_cueProperties */

struct t_cue
{
  char* myURI; // let it to NULL if no cue is specified
  int myVolume; // From 0 to 100
};

struct t_cueProperties
{
  struct t_cue myCueBefore;
  struct t_cue myCueAfter;
};

/* }}} */
/* {{{ t_markProperties */

struct t_markProperties
{
  char* myMarkBefore; 
  char* myMarkAfter; 
};

/* }}} */
/* {{{ t_VoiceProperties */

enum t_age
  {
    Child,
    Young,
    Old,
  };

enum t_genericVoice
{
  male,
  female,
};

struct t_voiceFamily 
{
  char* mySpecificVoice;
  enum t_age myAge;
  enum t_genericVoice myGenericVoice;
  int myNumber; 
};

enum t_voiceStress 
  {
    strongStress,
    moderateStress,
    noneStress,
    reducedStress,
    inheritStress,
  };

struct t_VoiceProperties
{
  struct t_voiceFamily myFamily;
  int myVoiceRate; // percentage. 50 means 50 per cent of the normal speech rate
  int myVoicePitch; // in Hertz (Number not percentage)
  int myVoicePitchRangeNumber; // in Hertz (Number not percentage)
  enum t_voiceStress myVoiceStress;
};

/* }}} */
/* {{{ t_phonetics */
struct t_phonetics
{
  char* myPhonemes;
  char* myPhoneticAlphabet;
  char* myContent;
};
/* }}} */

/* {{{ t_CSS */

struct t_CSS
{
  struct t_mixingProperties myMixing;
  enum t_speak mySpeak;
  struct t_pauseProperties myPause;
  struct t_cueProperties myCue;
  struct t_markProperties myMark;
  struct t_VoiceProperties * myVoice;
  int myVoiceDuration; // in milliseconds
  struct t_phonetics myPhonetics;
};

/* }}} */

#endif
