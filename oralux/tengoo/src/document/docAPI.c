/* 
----------------------------------------------------------------------------
docAPI.c
$Id: docAPI.c,v 1.3 2005/10/09 22:43:12 gcasse Exp $
$Author: gcasse $
Description: manage document, logical structure of the displayed screen.
$Date: 2005/10/09 22:43:12 $ |
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

/* < include */
#include <gnode.h>
#include <assert.h>
#include "docAPI.h"
#include "terminfo2list.h"
#include "debug.h"
/* > */

/* < document */
struct document
{
  int* myLinkVoiceVolume;
  GNode* myRootNode;
  GNode* myCurrentTextNode;
};
typedef struct document document;
/* > */
/* < element */
struct element
{
  enum elementType myType;
  int myNodeHasFocus;
  int* myVoiceVolume;
  gpointer myData;
};
typedef struct element element;
/* > */

/* < misc */
enum
  {
    defaultVoiceVolume=100,
  };
/* > */

/* < createElement */
static element* createElement( enum elementType theType, gpointer theData)
{
  element* this = NULL;

  ENTER("createElement");

  this = malloc(sizeof(element));
  if(this)
    {
      memset (this, 0, sizeof(element));
      this->myType = theType;
      this->myData = theData;
    }
  return this;
}
/* > */
/* < deleteElement */
static void deleteElement(element* this)
{
  ENTER("deleteElement");

  if (this)
    {
      free( this->myVoiceVolume);
      
      if( this->myData)
	{
	  switch(this->myType)
	    {
	    case frameType:
	      deleteFrame( this->myData);
	      break;
	    default:
	      /* TBD: text */
	      break;
	    }
	}
      free(this);
    }
}
/* > */
/* < deleteNode */
static gboolean deleteNode( GNode *theNode, gpointer theElementType)
{
  element* this = NULL;
  int aType = (int)theElementType;

  ENTER("deleteNode");

  this = theNode->data;
  
  if (this 
      && ((aType == anyType) 
	  || (aType & this->myType)))
    {
      deleteElement(this);
      free( this);
    }
  
  g_node_destroy( theNode);

  return 0;
}
/* > */
/* < setVoiceVolumeDocAPI */
static int setVoiceVolume( element* this, int theVoiceVolume)
{
  ENTER("setVoiceVolume");

  if (this)
    {
      if (this->myVoiceVolume == NULL)
	{
	  this->myVoiceVolume = malloc(sizeof(int));	
	}
      if (this->myVoiceVolume)
	{
	  *(this->myVoiceVolume) = theVoiceVolume;	
	}
    }
  return (this->myVoiceVolume != NULL);
}
/* > */
/* < clearData */
static void clearData( GNode* theNode, int theElementType)
{
  ENTER("clearData");

  if (theNode)
    {
      g_node_traverse (theNode,
		       G_IN_ORDER,
		       G_TRAVERSE_ALL,
		       -1,
		       (GNodeTraverseFunc)deleteNode,
		       (gpointer)theElementType);
    }
}
/* > */
/* < deleteDocAPI */
void deleteDocAPI( void* theDocAPI)
{
  document* this = theDocAPI;
  ENTER("deleteDocAPI");

  if (this)
    {
      if (this->myRootNode)
	{
	  clearData( this->myRootNode, anyType);
	  this->myRootNode = NULL;
	}
       free( this);
    }
}
/* > */
/* < createDocAPI */
void* createDocAPI( int theVoiceVolume)
{
  document* this = NULL;
  element* anElement = NULL;

  ENTER("createDocAPI");

  /* < create the document */
  this = malloc(sizeof(document));
  if(this)
    {
      memset (this, 0, sizeof(document));
    }
  /* > */
  /* < build the style structure: first the root node */
  anElement = createElement( rootType, NULL);
    
  if (anElement)
    {
      if (setVoiceVolume( anElement, theVoiceVolume))
	{
	  this->myRootNode = g_node_new( anElement);
	}

      if (!this)
	{
	  deleteElement( anElement);
	}
    }
  /* > */

  return (void*) this;
}
/* > */
/* < addFrameStyleDocAPI*/
void addFrameStyleDocAPI( void* theDocAPI, frame* theFrame)
{
  document* this = theDocAPI;
  element* anElement = NULL;
  frame* aFrame = NULL;

  ENTER("addFrameDocAPI");

  if (!this || !this->myRootNode)
    {
      return;
    }

  aFrame = copyFrame( theFrame);

  if (aFrame)
    {
      anElement = createElement( frameType, aFrame);
      
      if (!anElement)
	{
	  deleteFrame( aFrame);
	}
      else
	{
	  g_node_insert_data( this->myRootNode, -1, anElement);
	}
    }
  /*  return anElement;*/
}
/* > */
/* < findFrame */
static GNode* findFrame( GNode* theNode, box* theBoundingBox, enum intersectionType* theType, box* theIntersectionBox)
{
  GNode* aNode = theNode;
  GNode* aFrameNode = NULL;

  ENTER("findFrame");

  if (!theNode || !theBoundingBox || !theType || !theIntersectionBox)
    {
      return NULL;
    }

  while( aNode)
    {
      element* anElement = aNode->data;
      frame* aFrame = NULL;

      /* this must be a frame element */
      assert (anElement && (anElement->myType == frameType));
      aFrame = anElement->myData;

      /* does the frame include a portion of the text? */
      *theType  = isIncluded( theBoundingBox, aFrame->myBox, theIntersectionBox);

      if( *theType == noIntersectionBox)
	{
	  aNode = g_node_next_sibling( aNode);
	}
      else
	{
	  aFrameNode = aNode;
	  break;
	}
    }

  return aFrameNode;
}
/* > */
/* < linkEntryToTextElement */
/* At this stage, the document tree is supposed to only include the root element, the optional frame elements and text elements (no link).
*/
static void linkEntryToTextElement(gpointer theEntry, gpointer theDocument)
{
  document* this = theDocument;
  terminfoEntry* anEntry = theEntry;

  ENTER("linkEntryToTextElement");

  if (anEntry 
      && (anEntry->myCapacity == TEXTFIELD) 
      && this
      && this->myRootNode)
    {
      /* < create the bounding box */
      point* aOrigin = NULL;
      box* aBox;
      GNode* aFrameNode = NULL;
      enum intersectionType aType = noIntersectionBox;
      box anIntersectionBox;
      element* aTextElement = NULL;

      aOrigin = createPoint( anEntry->myStartingPosition.myCol, 
			     anEntry->myStartingPosition.myLine,
			     0);
      aBox = createBox( aOrigin, ((GString*)(anEntry->myData1))->len, 0);

      /* > */

      if (this->myCurrentTextNode == NULL)
	{
	  /* select the first frame element including the text */
	  GNode* aNode = g_node_first_child( this->myRootNode);
	  aFrameNode = findFrame( aNode, aBox, &aType, &anIntersectionBox);
	  aTextElement = createElement( textType, NULL);
	}
      else
	{ /* a text element already exists : check that its containing box includes the new entry */
	  aFrameNode = findFrame( this->myCurrentTextNode, aBox, &aType, &anIntersectionBox);
	  if (aFrameNode && aFrameNode->children)
	    {
	      aTextElement = aFrameNode->children->data;
	    }
	  if (aTextElement == NULL)
	    {
	      aTextElement = createElement( textType, NULL);
	    }
	}

      /* TBD: frame partially including an entry => split */
      
      if (!aFrameNode)
	{ /* No frame node: the text element will be added to the root node */
	  aFrameNode = this->myRootNode;
	}
      
      if (aFrameNode->children == NULL)
	{ /* create the text element */
	  this->myCurrentTextNode = g_node_insert_data( aFrameNode, -1, aTextElement);
	}

      deleteBox( aBox);
      deletePoint( aOrigin);
      deleteElement( aTextElement);
    }
}
/* > */
/* < putListEntryDocAPI */
GList* putListEntryDocAPI( void* theDocAPI, GList* theList)
{
  document* this = theDocAPI;
  GList* aList = theList;

  ENTER("putListEntryDocAPI");

  if (!this || !theList || !(this->myRootNode))
    {
      return NULL;
    }

  clearData( this->myRootNode, textType | linkType);
  this->myCurrentTextNode = NULL;

  /* determine (or create) the text element of each entry.
     At the moment, each text entry displayed in the same frame 
     is associated with the same text element.
   */
  g_list_foreach( theList, (GFunc)linkEntryToTextElement, this);

  
  /* TBD ... */


  return aList;
}
/* > */
/* < getStyledListEntryDocAPI */
GList* getStyledListEntryDocAPI( void* theDocAPI)
{
  ENTER("getStyledListEntryDocAPI");
  /* TBD */
  return NULL;
}
/* > */

/* < setElementTypeDocAPI */
void setElementTypeDocAPI( void* theDocAPI, GList* theElement, enum elementType theType, int theELementHasFocus)
{
  ENTER("setElementTypeDocAPI");

}
/* > */

/* 
Local variables:
folded-file: t
folding-internal-margins: nil
*/


