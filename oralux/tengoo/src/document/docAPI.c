/* < licence */
/* 
----------------------------------------------------------------------------
docAPI.c
$Id: docAPI.c,v 1.6 2005/10/15 21:49:45 gcasse Exp $
$Author: gcasse $
Description: manage document, logical structure of the displayed screen.
$Date: 2005/10/15 21:49:45 $ |
$Revision: 1.6 $ |
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
/* > */
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
  GNode* myRootNode;
  GNode* myCurrentTextNode;
  GList* myFirstEntry;
};
typedef struct document document;
/* > */
/* < element */
struct element
{
  enum elementType myType;
  enum elementFocusState myFocusState;
  int myVoiceVolume;
  int myComputedVoiceVolume;
  gpointer myData;
};
typedef struct element element;
/* > */
/* < misc */
enum
  {
    undefinedVoiceVolume=-1,
    silenceVoiceVolume=-1,
    defaultVoiceVolume=100,
  };

#ifdef DEBUG
char* displayType( int theType)
{
  static char aResult[1000];
  aResult[0]=0;
  if (theType == anyType)
    {
      strcpy(aResult,"any");
    }
  else
    {
      if (theType & rootType)
	{
	  strcat(aResult,"root,");
	}
      if (theType & textType)
	{
	  strcat(aResult,"text,");
	}
      if (theType & frameType)
	{
	  strcat(aResult,"frame,");
	}
      if (theType & linkType)
	{
	  strcat(aResult,"link,");
	}
    }
  return aResult;
}
#define DISPLAY_TYPE( x) displayType(x)
#else
#define DISPLAY_TYPE( x)
#endif

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
      this->myVoiceVolume = this->myComputedVoiceVolume = undefinedVoiceVolume;

      SHOW3("element:%x, type=%s\n", this, DISPLAY_TYPE(theType));
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
      SHOW3("element:%x, type=%s\n", this, DISPLAY_TYPE(this->myType));
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
  gboolean aTraversalMustBeStopped = FALSE;

  ENTER("deleteNode");

  this = theNode->data;
  
  if (this && ((aType == anyType) || (aType & this->myType)))
    {
      SHOW2("delete node %x\n", theNode);

      deleteElement(this);
      g_node_destroy( theNode);
    }

  return aTraversalMustBeStopped;
}
/* > */
/* < setVoiceVolume */
static void setVoiceVolume( element* this, int theVoiceVolume)
{
  ENTER("setVoiceVolume");

  if (this)
    {
      this->myVoiceVolume = theVoiceVolume;
    }
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
      deleteTermInfoList( this->myFirstEntry);
      
      free( this);
    }
}
/* > */
/* < computeStyleSingleNode */
static gboolean computeStyleSingleNode( GNode *theNode, gpointer theFoo)
{
  gboolean aTraversalMustBeStoppped = FALSE; 
  element* aCurrentElement = NULL;

  ENTER("computeStyleSingleNode");

  if (!theNode || !(theNode->data))
    {
      return aTraversalMustBeStoppped;
    }

  aCurrentElement = theNode->data;

  aCurrentElement->myComputedVoiceVolume = undefinedVoiceVolume;

  if (aCurrentElement->myVoiceVolume != undefinedVoiceVolume)
    {
      aCurrentElement->myComputedVoiceVolume = aCurrentElement->myVoiceVolume;
    }
  else if (theNode->parent && theNode->parent->data)
    {
      element* aParentElement = theNode->parent->data;
      aCurrentElement->myComputedVoiceVolume = aParentElement->myComputedVoiceVolume;
    }
  return aTraversalMustBeStoppped;
}
/* > */
/* < computeStyle */
static void computeStyle( GNode* theNode)
{
  /* update the computed style of the node and its descendants */

  g_node_traverse (theNode,
		   G_LEVEL_ORDER,
		   G_TRAVERSE_ALL,
		   -1,
		   (GNodeTraverseFunc)computeStyleSingleNode,
		   (gpointer)NULL);
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
      setVoiceVolume( anElement, theVoiceVolume);
      this->myRootNode = g_node_new( anElement);
      SHOW2("Root node: %x\n", this->myRootNode);
	    
      computeStyle( this->myRootNode);

      if (!this)
	{
	  deleteElement( anElement);
	}
    }
  /* > */

  return (void*) this;
}
/* > */
/* < addFrameElement */
static void addFrameElement (gpointer theEntry, gpointer theDocument)
{
  document* this = theDocument;
  frame* aFrame = NULL;

  ENTER("addFrameElement");

  if (!theEntry || !this || !(this->myRootNode))
    {
      return;
    }

  aFrame = copyFrame( (frame*)theEntry);

  if (aFrame)
    {
      element* anElement = createElement( frameType, aFrame);
      
      if (!anElement)
	{
	  deleteFrame( aFrame);
	}
      else
	{
	  GNode* aNode = g_node_insert_data( this->myRootNode, -1, anElement);
	  computeStyle( aNode);
	  SHOW4("add under Root %x, node %x, element=%x\n",
	       this->myRootNode, aNode, anElement);
	}
    }
}
/* > */
/* < addFrameStyleDocAPI */
void addFrameStyleDocAPI( void* theDocAPI, GList* theFrame)
{
  document* this = theDocAPI;

  ENTER("addFrameDocAPI");

  g_list_foreach( theFrame, (GFunc)addFrameElement, this);
}
/* > */
/* < loadStyle */
enum {
  screenFrame,
  statusFrame,
  titleFrame,
}; /* TBD */
void loadStyleDocAPI( void* theDocAPI, char* theFilename)
{
  frame* aFrame = NULL;
  GList* aList = NULL;
  point* p = NULL;

  /* take in account the stored styles */

  ENTER("loadStyleDocAPI");

  if (!theDocAPI || !theFilename)
    {
      return;
    }

  /* TBD: parse the style */

  /* < create frames */
  p = createPoint( 0, 0, 0);

  aFrame = createFrame( screenFrame, "screen", p, 79, 24);
  aList = g_list_append( aList, (gpointer)aFrame);

  aFrame = createFrame( titleFrame, "title", p, 79, 0);
  aList = g_list_append( aList, (gpointer)aFrame);

  translatePoint( p, 0, 24);
  aFrame = createFrame( statusFrame, "status", p, 79, 0);
  aList = g_list_append( aList, (gpointer)aFrame);

  addFrameStyleDocAPI( theDocAPI, aList);

  g_list_free( aList);
  /* > */
}
/* > */
/* < findFrame */
static GNode* findFrame( GNode* theNode, box* theBoundingBox, enum intersectionType* theType, box* theIntersectionBox)
{
  GNode* aNode = theNode;
  GNode* aFrameNode = NULL;
  gboolean aNextNodeIsSearched = TRUE;

  ENTER("findFrame");

  if (!theNode || !theBoundingBox || !theType || !theIntersectionBox)
    {
      return NULL;
    }

  do 
    {
      element* anElement = aNode->data;
      assert (anElement);

      if (anElement->myType == frameType)
	{
	  frame* aFrame = anElement->myData;

	  /* does the frame include a portion of the text? */
	  *theType  = isIncludedBox( theBoundingBox, aFrame->myBox, theIntersectionBox);

	  if( *theType != noIntersectionBox)
	    {
	      aFrameNode = aNode; /* TBD: partial intersection */
	      break;
	    }
	}

      /* < following node */
      if (aNextNodeIsSearched)
	{
	  aNode = g_node_next_sibling( aNode);
	}
      else
	{
	  aNode = g_node_prev_sibling( aNode);
	}

      if ((aNode==NULL) && aNextNodeIsSearched)
	{
	  aNextNodeIsSearched = FALSE;
	  aNode = g_node_prev_sibling( theNode);
	}
      /* > */
    } while( aNode);

  SHOW2("frame node=%x\n",aFrameNode);

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
      element* aTextElement = NULL; /* will be given to the node, no delete required */

      aOrigin = createPoint( anEntry->myStartingPosition.myCol, 
			     anEntry->myStartingPosition.myLine,
			     0);
      aBox = createBox( aOrigin, ((GString*)(anEntry->myData1))->len, 0);
      /* > */

      SHOW2("myCurrentTextNode=%x\n", this->myCurrentTextNode);

      if (this->myCurrentTextNode == NULL)
	{
	  /* select the first frame element including the text */
	  GNode* aNode = g_node_first_child( this->myRootNode);
	  aFrameNode = findFrame( aNode, aBox, &aType, &anIntersectionBox);
	  aTextElement = createElement( textType, NULL);
	}
      else
	{ /* a text element already exists : check that its containing box includes the new entry */
	  aFrameNode = findFrame( this->myCurrentTextNode->parent, aBox, &aType, &anIntersectionBox);
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
	  SHOW4("insert under frame node:%x, new text node:%x, element:%x\n", 
		aFrameNode, this->myCurrentTextNode, aTextElement);
	  computeStyle( this->myCurrentTextNode);
	}

      deleteBox( aBox);
      deletePoint( aOrigin);
    }
}
/* > */
/* < searchHoveredNode */
static void searchHoveredNode( GNode* theNode, gpointer theHoveredNodePointer)
{
  GNode** aHoveredNode = theHoveredNodePointer;

  ENTER("searchHoveredNode");

  if (theNode && theNode->data && aHoveredNode)
    {
      element* anElement = theNode->data;
      if (anElement->myFocusState & hoveredElement )
	{
	  *aHoveredNode = theNode;
	  SHOW2("Node: %x\n",theNode);
	}
    }
}
/* > */
/* < putListEntryDocAPI */
void putListEntryDocAPI( void* theDocAPI, GList* theList)
{
  document* this = theDocAPI;

  ENTER("putListEntryDocAPI");

  if (!this || !theList || !(this->myRootNode))
    {
      return;
    }
  assert(this->myFirstEntry == NULL);

  this->myFirstEntry = theList;

  /* determine (or create) the text element of each entry.
     At the moment, each text entry displayed in the same frame 
     is associated with the same text element.
  */
  g_list_foreach( theList, (GFunc)linkEntryToTextElement, this);

}
/* > */
/* < getStyledListEntryDocAPI */
GList* getStyledListEntryDocAPI( void* theDocAPI)
{
  document* this = theDocAPI;
  GList* aList = NULL;
  int aVolume = defaultVoiceVolume;

  ENTER("getStyledListEntryDocAPI");

  if (!this || !(this->myFirstEntry))
    {
      return NULL;
    }

  aList = this->myFirstEntry;

  do 
    {
      int aComputedVolume = undefinedVoiceVolume;
      element* anElement = NULL;
      GNode* aHoveredNode = NULL;
      GNode* aNode = getNodeFromList( aList);
      GNode* aParent = NULL;
      
      if (!aNode)
	{
	  continue;
	}
      assert(aNode->data);

      /* < look for the hovered element */
      if (aNode->parent && (aNode->parent != aParent))
	{
	  g_node_children_foreach( aNode->parent,
				   G_TRAVERSE_ALL,
				   searchHoveredNode,
				   (gpointer)&aHoveredNode);
	}
      /* > */

      if (aHoveredNode && (aHoveredNode != aNode))
	{
	  aComputedVolume = silenceVoiceVolume;
	}
      else
	{
	  anElement = (element*)(aNode->data);      
	  aComputedVolume = anElement->myComputedVoiceVolume;
	}

      if (aVolume != aComputedVolume)
	{
	  terminfoEntry* anEntry = NULL;
	  aVolume = aComputedVolume;
	  anEntry = get_TSAR_Sequence( aVolume, 1);
	  this->myFirstEntry = g_list_insert_before( this->myFirstEntry, aList, anEntry);
	}
    } while( (aList = g_list_next( aList)));

  return NULL;
}
/* > */
/* < setElementTypeDocAPI */
void* setElementTypeDocAPI( void* theDocAPI, void* theNode, enum elementType theType)
{
  document* this = theDocAPI;
  GNode* aNode = theNode; 
  element* anElement = NULL;

  ENTER("putListEntryDocAPI");

  if (!this || !aNode || !(aNode->data))
    {
      return NULL;
    }
  
  anElement = aNode->data;

  switch(theType)
    {
    case linkType:
      {
	if( anElement->myType == textType)
	  {
	    /* a link element is added 
	       the previous text element is left out, 
	       no particular issue if the link was its only element. 
	    */
	    GNode* aLinkNode = NULL;
	    anElement= createElement( linkType, NULL);
	    aLinkNode = g_node_new( anElement);
	    
	    /* insert the new node into the document tree */
	    aNode = g_node_insert_after( aNode->parent, aNode, aLinkNode);
	    SHOW3("add under node %x, new node=%x\n", (int)aNode->parent, (int)aNode);
	    computeStyle( aNode);
	  }
      }
      break;
    default:
      break;
    }
  return aNode;
}
/* > */
/* < setElementFocusState */
static void setElementFocusState( GNode* theNode, gpointer theData)
{
  ENTER("setElementFocusState");

  if (theNode && theNode->data)
    {
      element* anElement = theNode->data;
      anElement->myFocusState = (enum elementFocusState)theData;
    }
}
/* > */
/* < setFocusStateDocAPI */
void setFocusStateDocAPI( void* theNode, enum elementFocusState theState)
{
  GNode* aNode = theNode;

  ENTER("setFocusStateDocAPI");

  if (aNode)
    {
      element* anElement = aNode->data;

      /* < if the node is hovered, its siblings are not hovered */
      if ((theState & hoveredElement) && (aNode->parent))
	{
	  g_node_children_foreach( aNode->parent,
				   G_TRAVERSE_ALL,
				   setElementFocusState,
				   (gpointer)notHoveredElement);
	}
      /* > */

      anElement->myFocusState = theState;
    }
}
/* > */
/* < clearContentDocAPI */
void clearContentDocAPI( void* theDocAPI)
{
  document* this = theDocAPI;

  ENTER("clearContentDocAPI");
  if (this)
    {
      clearData( this->myRootNode, textType | linkType);
      this->myCurrentTextNode = NULL;

      /*      deleteTermInfoList( this->myFirstEntry);*/
      this->myFirstEntry = NULL;
    }
}
/* > */

/* 
Local variables:
folded-file: t
folding-internal-margins: nil
*/
