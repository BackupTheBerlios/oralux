/* 
----------------------------------------------------------------------------
docAPI.c
$Id: docAPI.c,v 1.2 2005/10/02 20:28:33 gcasse Exp $
$Author: gcasse $
Description: manage document, logical structure of the displayed screen.
$Date: 2005/10/02 20:28:33 $ |
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

/* < include */
#include "docAPI.h"
#include "debug.h"
/* > */

/* < misc */
enum
  {
    defaultVoiceVolume=100,
  };

#define deleteRoot(a) free(a)

/* > */

/* < createDocNode */
static docNode* createDocNode( gpointer theData)
{
  docNode* this = NULL;

  ENTER("createDocNode");

  this = malloc(sizeof(docNode));
  if(this)
    {
      memset (this, 0, sizeof(docNode));      
    }
  return this;
}
/* > */
/* < createRootNode */
static docNode* createRootNode( style* theStyle)
{
  docNode* this=NULL;
  style* aStyle = malloc(sizeof(style));

  if (aStyle)
    {
      copyStyle( aStyle, theStyle);

      this = createDocNode( aStyle);
      if (this)
	{
	  this->myType = rootType;
	}
      else
	{
	  deleteStyle( aStyle);
	}
    }
  return this;
}
/* > */
/* < createTextNode */
docNode* createTextNode( linePortion* theSource)
{
  docNode* this = NULL;
  linePortion* aLinePortion = NULL;

  ENTER("createTextNode");

  aLinePortion = copyLinePortion( theSource);

  if (aLinePortion)
    {
      this = createDocNode( aLinePortion);
      
      if (this)
	{
	  this->myType = textType;
	}
      else
	{
	  deleteLinePortion( aLinePortion);
	}
    }
  return this;
}
/* > */
/* < createFrameNode */
docNode* createFrameNode( frame* theSource)
{
  docNode* this = NULL;
  frame* aFrame = NULL;

  ENTER("createFrameNode");

  aFrame = copyFrame( theSource);

  if (aFrame)
    {
      this = createDocNode( aFrame);
      
      if (this)
	{
	  this->myType = frameType;
	}
      else
	{
	  deleteFrame( aFrame);
	}
    }
  return this;
}
/* > */
/* < deleteDocNode */
static void deleteDocNode(docNode* this)
{
  ENTER("deleteDocNode");

  if (this)
    {
      free( this->myVoiceVolume);
      
      switch( this->myType)
	{
	case rootType:
	  deleteRoot( this->myRoot);
	  break;
	case frameType:
	  deleteFrame( this->myFrame);
	  break;
	case textType:
	  deleteLinePortion( this->myLinePortion);
	  break;
	}
      free(this);
    }
}
/* > */
/* < deleteGNode */
static gboolean deleteGNode( GNode *theNode, gpointer theFoo)
{
  docNode* this = NULL;

  ENTER("deleteGNode");

  this = theNode->data;

  if (this)
    {
      deleteDocNode(this);
      free( this);
    }

  return 0;
}
/* > */
/* < deleteDocAPI */
void deleteDocAPI( GNode* this)
{
  ENTER("deleteDocAPI");

  if (this)
    {
      g_node_traverse (this,
		       G_IN_ORDER,
		       G_TRAVERSE_ALL,
		       -1,
		       (GNodeTraverseFunc)deleteGNode,
		       NULL);
      g_node_destroy( this);
    }
}
/* > */
/* < createDocAPI */
GNode* createDocAPI( style* theStyle, int theVoiceVolume)
{
  GNode* this = NULL;

  ENTER("createDocAPI");

  if (theStyle)
    {
      docNode* aDocNode = createRootNode( theStyle);
    
      if (aDocNode)
	{
	  aDocNode->myVoiceVolume = malloc(sizeof(int));
	  if (aDocNode->myVoiceVolume)
	    {
	      *(aDocNode->myVoiceVolume) = theVoiceVolume;
	      this = g_node_new( aDocNode);
	    }
	  	  
	  if (!this)
	    {
	      deleteDocNode( aDocNode);
	    }
	}
    }

  return this;
}
/* > */

/* < setVoiceVolumeDocAPI */
void setVoiceVolumeDocAPI( GNode* this, int theValue)
{
  if (this && this->data)
    {
      docNode* aDocNode = this->data;
      *(aDocNode->myVoiceVolume) = theValue;
    }
}
/* > */




/* 
Local variables:
folded-file: t
folding-internal-margins: nil
*/


