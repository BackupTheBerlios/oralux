/* 
----------------------------------------------------------------------------
mode.c
$Id: mode.c,v 1.2 2005/09/17 23:05:40 gcasse Exp $
$Author: gcasse $
Description: Mode API.
$Date: 2005/09/17 23:05:40 $ |
$Revision: 1.2 $ |
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
#include "debug.h"
#include "mode.h"

/* < type */
struct childMode
{
  void* myIdent;
  CREATE_MODE_HANDLER* create;
  DELETE_MODE_HANDLER* delete;
  MANAGE_BUFFER_MODE_HANDLER* manageInput;
  MANAGE_BUFFER_MODE_HANDLER* manageOutput;
};
typedef struct childMode childMode;

struct mode
{
  GModule* myModule;
  childMode* myChild;
};
typedef struct mode mode;
/* > */
/* < createModule */
static GModule* createModule( char* theName)
{
  gchar* aPath = g_module_build_path( NULL, theName);
  return g_module_open( aPath, 0);
}
/* > */
/* < createChildMode */
static childMode* createChildMode( GModule* theModule, termAPI* theTermAPI)
{
  childMode* p=NULL;

  ENTER("createChildMode");

  if (!theModule || !theTermAPI)
    {
      return NULL;
    }

  p = malloc(sizeof(childMode));
  if (!p)
    {
      return NULL;
    }

  memset(p, 0, sizeof(childMode));
  
  if(g_module_symbol( theModule, "createMode", (gpointer*)&(p->create))
     && g_module_symbol( theModule, "deleteMode", (gpointer*)&(p->delete))
     && g_module_symbol( theModule, "manageInputMode", (gpointer*)&(p->manageInput))
     && g_module_symbol( theModule, "manageOutputMode", (gpointer*)&(p->manageOutput)))
    {
      p->myIdent = p->create( NULL, theTermAPI);
    }
  else
    {
      free(p);
      p = NULL;
    }

  return p;
}
/* > */
/* < createMode */
void* createMode( char* theName, termAPI* theTermAPI)
{
  mode* this=NULL;

  ENTER("createMode");

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
      deleteMode( this);
      return NULL;
    }

  this->myChild = createChildMode( this->myModule, theTermAPI);
  if (this->myChild == NULL)
    { 
      SHOW("Can't create the child mode\n");
      deleteMode( this);
      return NULL;
    }

  return (void*)this;
}
/* > */
/* < deleteMode */
void deleteMode( void* theMode)
{
  mode* this = (mode*)theMode;

  ENTER("deleteMode");

  if (this)
    {
      if (this->myChild)
	{
	  childMode* p = this->myChild;
	  p->delete( p->myIdent);
	  free( p);
	}

      if (this->myModule)
	{
	  if (g_module_close( this->myModule) == FALSE)
	    {
	      SHOW2("%s\n", g_module_error());
	    }
	}

      free( this);
    }
}
/* > */
/* < manageInputMode */
GByteArray* manageInputMode( void* theMode, char* theInput, int theLength)
{
  mode* this = (mode*)theMode;
  GByteArray* aByteArray = NULL;

  ENTER("manageInputMode");

  if (this && this->myChild)
    {
      childMode* p = this->myChild;
      aByteArray = p->manageInput( p, theInput, theLength);
    }
  return aByteArray;
}
/* > */
/* < manageOutputMode */
GByteArray* manageOutputMode( void* theMode, char* theOutput, int theLength)
{
  mode* this = (mode*)theMode;
  GByteArray* aByteArray = NULL;

  ENTER("manageOutputMode");

  if (this && this->myChild)
    {
      childMode* p = this->myChild;
      aByteArray = p->manageOutput( p, theOutput, theLength);
    }

  return aByteArray;
}
/* > */
