/* 
----------------------------------------------------------------------------
links2vox.c
$Id: links2vox.c,v 1.3 2005/09/30 23:27:50 gcasse Exp $
$Author: gcasse $
Description: tengoo plugin for the links2 web browser.
$Date: 2005/09/30 23:27:50 $ |
$Revision: 1.3 $ |
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
#include "pluginAPI.h"
#include "point.h"
#include "frame.h"
#include "action.h"
#include "debug.h"

/* < type */
enum keyIdentifier
  {
    TAB,
    ALT_TAB
  };

const char* TheKeySequence[]=
  {
    "\0x09",
    "\0x1b\0x09",
  };

enum commandIdentifier 
  {
    UNDEFINED_COMMAND,
    JUMP_TO_LINK,
    MUTE_STATUS_LINE,
    SAY_BOX,
  };

enum state 
{
  UNDEFINED,
  SEARCH_LINK,
};

struct plugin
{
  int myState;
  TRANSCODE_PLUGINAPI* myOutputIntermediaryBlockCallback;
  TRANSCODE_PLUGINAPI* myOutputLastBlockCallback;
  int myMaxLength;
  point* myOrigin;
  box* myScreen;
  box* myStatus;
  box* myTitle;
};
typedef struct plugin plugin;
/* > */
/* < Callbacks */
/* < initCallback */
static void initCallback( plugin* this)
{
  ENTER("initCallback");
  this->myOutputIntermediaryBlockCallback = mutePreviouslyHighlightedArea;
  this->myOutputLastBlockCallback = mutePreviouslyHighlightedArea;
}
/* > */
/* < processLastOutputDuringLinkSearch */
static GByteArray* processLastOutputDuringLinkSearch( pluginAPI* thePluginAPI, char* theOutput, int theLength)
{
  plugin* this = (plugin*)(thePluginAPI->myPlugin);
  GByteArray* aByteArray = NULL;

  ENTER("processLastOutputDuringLinkSearch");

  updateScreen( thePluginAPI, theOutput, theLength);

  aByteArray = sayOnlyLinePortionAtCursor( thePluginAPI, theOutput, theLength);

  initCallback(this);

  return aByteArray;
}
/* > */
/* < processSingleOutputDuringLinkSearch */
static GByteArray* processSingleOutputDuringLinkSearch( pluginAPI* thePluginAPI, char* theOutput, int theLength)
{
  plugin* this = (plugin*)(thePluginAPI->myPlugin);
  GByteArray* aByteArray = NULL;

  ENTER("processSingleOutputDuringLinkSearch");

  aByteArray = mutePreviouslyHighlightedArea( thePluginAPI, theOutput, theLength);

  initCallback(this);

  return aByteArray;
}
/* > */
/* < processIntermediaryOutputDuringLinkSearch */
static GByteArray* processIntermediaryOutputDuringLinkSearch( pluginAPI* thePluginAPI, char* theOutput, int theLength)
{
  plugin* this = (plugin*)(thePluginAPI->myPlugin);
  GByteArray* aByteArray = NULL;

  ENTER("processIntermediaryOutputDuringLinkSearch");

  aByteArray = muteDisplayedOutput( thePluginAPI, theOutput, theLength);

  this->myOutputIntermediaryBlockCallback = (TRANSCODE_PLUGINAPI*)processIntermediaryOutputDuringLinkSearch;
  this->myOutputLastBlockCallback = processLastOutputDuringLinkSearch;

  return aByteArray;
}
/* > */
/* > */
/* < convertInputToCommand */
static enum commandIdentifier convertInputToCommand( const char* theInput, int theLength)
{
  enum commandIdentifier anIdentifier = UNDEFINED_COMMAND;

  ENTER("convertInputToCommand");

  switch (theLength)
    {
    case 1:
      if(theInput[0]==0x09)
	{
	  anIdentifier = JUMP_TO_LINK;
	}
      break;

    case 2:
      if((theInput[0]==0x1B) && (theInput[1]==0x09))
	{
	  anIdentifier = JUMP_TO_LINK;
	}
      break;

    default:
      break;
    }
  return anIdentifier;
}
/* > */
/* < manageCommand*/
static GByteArray* manageCommand( plugin* this, enum commandIdentifier theCommandIdentifier, char* theCommand, int theCommandLength)
{
  GByteArray* aByteArray = NULL;

  ENTER("manageCommand");

  SHOW3("theCommandIdentifier=%d, theCommandLength=%d\n",theCommandIdentifier, theCommandLength);

  if(theCommand)
    {
      aByteArray = g_byte_array_new();
      g_byte_array_append( (GByteArray*) aByteArray, 
			   (guint8*) theCommand, 
			   (guint) theCommandLength);
    }

  switch( theCommandIdentifier)
    {
    case JUMP_TO_LINK:
      this->myState = SEARCH_LINK;
      this->myOutputIntermediaryBlockCallback = processIntermediaryOutputDuringLinkSearch;
      this->myOutputLastBlockCallback = processSingleOutputDuringLinkSearch;

      break;

    case MUTE_STATUS_LINE:
      {
/* 	setVolumeBox( ); */
      }
      break;

    case SAY_BOX:
/* 	sayBox( ); */
      break;

    default:
      this->myState = UNDEFINED;
      this->myOutputIntermediaryBlockCallback = mutePreviouslyHighlightedArea;
      this->myOutputLastBlockCallback = mutePreviouslyHighlightedArea;
      break;
    }
  SHOW2("myState=%d\n", this->myState);

  return aByteArray;
}
/* > */
/* < createPlugin */
void* createPlugin( termAPI* theTermAPI, int theInputOutputMaxLength)
{
  plugin* this=NULL;
  point* p;

  ENTER("createPlugin");

  this = malloc(sizeof(plugin));
  memset(this, 0, sizeof(plugin));

  this->myMaxLength = theInputOutputMaxLength;

  p = this->myOrigin = createPoint( 0, 0, 0);
  this->myScreen = createBox( p, 79, 24);
  this->myTitle = createBox( p, 79, 0);

  translatePoint(p, 0, 24);
  this->myStatus = createBox( p, 79, 0);
  
  manageCommand( this, UNDEFINED_COMMAND, NULL, 0);

  return this;
}
/* > */
/* < deletePlugin */
void deletePlugin( void* theplugin)
{
  plugin *this = (plugin*)theplugin;

  ENTER("deletePlugin");

  deletePoint( this->myOrigin);
  deleteBox( this->myScreen);
  deleteBox( this->myStatus);
  deleteBox( this->myTitle);

  free(this);
}
/* > */
/* < transcodeInput */
GByteArray* transcodeInputPlugin( pluginAPI* thePluginAPI, char* theInput, int theLength)
{
  plugin* this=(plugin*)(thePluginAPI->myPlugin);
  enum commandIdentifier anIdentifier = UNDEFINED_COMMAND;

  ENTER("transcodeInputPlugin");

  anIdentifier = convertInputToCommand( theInput, theLength);
  return manageCommand( this, anIdentifier, theInput, theLength);
}
/* > */
/* < transcodeOutput */
GByteArray* transcodeOutputPlugin( pluginAPI* thePluginAPI, char* theOutput, int theLength)
{
  GByteArray* aByteArray = NULL;
  plugin* this=(plugin*)(thePluginAPI->myPlugin);
  TRANSCODE_PLUGINAPI* aCallback = NULL;

  ENTER("transcodeOutputPlugin");

  aCallback = (theLength == this->myMaxLength) ? 
    this->myOutputIntermediaryBlockCallback
    : this->myOutputLastBlockCallback;

  if (aCallback)
    {
      aByteArray = aCallback( thePluginAPI, theOutput, theLength);
    }

  return aByteArray;
}
/* > */


/* 
Local variables:
folded-file: t
folding-internal-margins: nil
*/
