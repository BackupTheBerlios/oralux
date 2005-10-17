/* 
----------------------------------------------------------------------------
mode.c
$Id: modeAPI.c,v 1.5 2005/10/17 14:12:25 gcasse Exp $
$Author: gcasse $
Description: Mode API.
$Date: 2005/10/17 14:12:25 $ |
$Revision: 1.5 $ |
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

#include <stdlib.h>
#include <string.h>
#include <gmodule.h>
#include "action.h"
#include "docAPI.h"
#include "debug.h"
#include "modeAPI.h"
#include "termapi.h"
#include "pluginAPI.h"

/* < struct mode */
struct mode
{
  GModule* myModule;
  pluginAPI* myPluginAPI;
};
typedef struct mode mode;
/* > */

/* < createModule */
static GModule* createModule( char* theName)
{
  gchar* aPath = NULL;
  GModule* this = NULL;

  ENTER("createModule");
  aPath = g_module_build_path( NULL, theName);
  this = g_module_open( aPath, 0);
  SHOW2("New module:%x\n",(int)this);
  return this;
}
/* > */
/* < deleteModule */
static void deleteModule( GModule** theModule)
{
  ENTER("deleteModule");
  if (theModule && *theModule)
    {
      GModule* this = *theModule;
      SHOW2("Old module:%x\n",(int)this);
      if (g_module_close( this) == FALSE)
	{
	  SHOW2("%s\n", g_module_error());
	}
      *theModule = NULL;
    }
}
/* > */
/* < createPluginAPI */
static pluginAPI* createPluginAPI( mode* this, int theInputOutputMaxLength, termAPI* theTermAPI)
{
  pluginAPI* p=NULL;

  ENTER("createPluginAPI");

  if (!this || !this->myModule)
    {
      return NULL;
    }

  p = malloc(sizeof(pluginAPI));
  if (!p)
    {
      return NULL;
    }

  memset(p, 0, sizeof(pluginAPI));
  
  if(g_module_symbol( this->myModule, "createPlugin", (gpointer*)&(p->create))
     && g_module_symbol( this->myModule, "deletePlugin", (gpointer*)&(p->delete))
     && g_module_symbol( this->myModule, "transcodeInputPlugin", (gpointer*)&(p->transcodeInput))
     && g_module_symbol( this->myModule, "transcodeOutputPlugin", (gpointer*)&(p->transcodeOutput)))
    {
      p->myTermAPI = theTermAPI;
      p->myBuffer = NULL;
      p->myPlugin = p->create( theTermAPI, theInputOutputMaxLength, &(p->myDocument));
    }
  else
    {
      free(p);
      p = NULL;
    }
  SHOW2("New pluginAPI:%x\n",(int)this);

  return p;
}
/* > */
/* < deletePluginAPI */
static void deletePluginAPI( pluginAPI** thePluginAPI)
{
  ENTER("deletePluginAPI");

  if (thePluginAPI && *thePluginAPI)
    {
      pluginAPI* this = *thePluginAPI;
      SHOW2("Old pluginAPI:%x\n",(int)this);
      this->delete( this->myPlugin);
      if (this->myBuffer)
	{
	  g_byte_array_free( this->myBuffer, 1);
	}
      deleteDocAPI( this->myDocument);
      free( this);
      *thePluginAPI = NULL;
    }
}
/* > */
/* < createModeAPI */
void* createModeAPI( char* theName, termAPI* theTermAPI, int theInputOutputMaxLength)
{
  mode* this=NULL;

  ENTER("createModeAPI");

  if (g_module_supported() == FALSE)
    {
      SHOW2("%s\n", g_module_error());
      exit(1);
    }

  this = malloc(sizeof(mode));
  if (this == NULL)
    {
      return NULL;
    }
  memset(this, 0, sizeof(mode));

  this->myModule = createModule( theName);
  if (this->myModule == NULL)
    { 
      SHOW2("%s\n", g_module_error());
      deleteModeAPI( this);
      return NULL;
    }

  this->myPluginAPI = createPluginAPI( this, theInputOutputMaxLength, theTermAPI);
  if (this->myPluginAPI == NULL)
    { 
      SHOW("Can't create the child mode\n");
      deleteModeAPI( this);
      return NULL;
    }
  SHOW2("New modeAPI:%x\n",(int)this);
  return (void*)this;
}
/* > */
/* < deleteModeAPI */
void deleteModeAPI( void* theMode)
{
  mode* this = (mode*)theMode;

  ENTER("deleteModeAPI");

  if (this)
    {
      SHOW2("Old modeAPI:%x\n",(int)this);
      deletePluginAPI ( &(this->myPluginAPI));
      deleteModule( &(this->myModule));
      free( this);
    }
}
/* > */
/* < transcodeInputModeAPI */
GByteArray* transcodeInputModeAPI( void* theMode, char* theInput, int theLength)
{
  mode* this = (mode*)theMode;
  GByteArray* aByteArray = NULL;

  ENTER("transcodeInputModeAPI");

  if (this && this->myPluginAPI)
    {
      pluginAPI* p = this->myPluginAPI;
      aByteArray = p->transcodeInput( p, theInput, theLength);
    }
  return aByteArray;
}
/* > */
/* < transcodeOutputModeAPI */
GByteArray* transcodeOutputModeAPI( void* theMode, char* theOutput, int theLength)
{
  mode* this = (mode*)theMode;
  GByteArray* aByteArray = NULL;

  ENTER("transcodeOutputModeAPI");

  if (this && this->myPluginAPI)
    {
      pluginAPI* p = this->myPluginAPI;
      aByteArray = p->transcodeOutput( p, theOutput, theLength);
    }

  return aByteArray;
}
/* > */
/* 
Local variables:
folded-file: t
folding-internal-margins: nil
*/
