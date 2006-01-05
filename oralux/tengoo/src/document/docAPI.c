/* < license */
/* 
----------------------------------------------------------------------------
docAPI.c
$Id: docAPI.c,v 1.12 2006/01/05 23:30:46 gcasse Exp $
$Author: gcasse $
Description: manage document, logical structure of the displayed screen.
$Date: 2006/01/05 23:30:46 $ |
$Revision: 1.12 $ |
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

#define getElementFromNode(x) (element*)(x->data)

/* > */
/* < misc */
enum
  {
    undefinedVoiceVolume=-1,
    silenceVoiceVolume=0,
    defaultVoiceVolume=100,
  };

#ifdef DEBUG
char* displayType( int theType)
{
  static char aResult[2][1000];
  static int i=0;
  char* s=NULL;
  i=(i+1)%2;
  s=aResult[i];
  s[0]=0;

  if (theType == anyNotRootType)
    {
      strcpy(s,"anyNotRootType");
    }
  else
    {
      if (theType & rootType)
	{
	  strcat(s,"root ");
	}
      if (theType & textType)
	{
	  strcat(s,"text ");
	}
      if (theType & frameType)
	{
	  strcat(s,"frame ");
	}
      if (theType & linkType)
	{
	  strcat(s,"link ");
	}
    }
  return s;
}
char* displayFocusState( int theState)
{
  static char aResult[2][1000];
  static int i=0;
  char* s=NULL;
  i=(i+1)%2;
  s=aResult[i];
  s[0]=0;

  if (theState == notHoveredElement)
    {
      strcpy(s,"notHovered");
    }
  else
    {
      if (theState & hoveredElement)
	{
	  strcat(s,"hovered ");
	}
      if (theState & activeElement)
	{
	  strcat(s,"active ");
	}
      if (theState & focusedElement)
	{
	  strcat(s,"focused ");
	}
    }
  return s;
}
#define DISPLAY_TYPE( x) displayType(x)
#define DISPLAY_FOCUS_STATE( x) displayFocusState(x)
#else
#define DISPLAY_TYPE( x) 
#define DISPLAY_FOCUS_STATE( x)
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

      SHOW3("element:%x, type=%s\n", (int)this, DISPLAY_TYPE(theType));
    }
  return this;
}
/* > */
/* < deleteElement */
static void deleteElement(element** theElement)
{
  ENTER("deleteElement");

  if (theElement && *theElement)
    {
      element* this=*theElement;
      SHOW3("element:%x, type=%s\n", (int)this, DISPLAY_TYPE(this->myType));
      if( this->myData)
	{
	  switch(this->myType)
	    {
	    case frameType:
	      deleteFrame( (frame**)&(this->myData));
	      break;
	    default:
	      /* TBD: text */
	      break;
	    }
	}
      free(this);
      *theElement=NULL;
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

  SHOW4("Node %x, type:%s, expected:%s\n", (int)theNode, DISPLAY_TYPE(this->myType), DISPLAY_TYPE( (int)theElementType));
  
  if (this && (aType & this->myType))
    {
      deleteElement(&this);
      g_node_unlink ( theNode);
      g_node_destroy( theNode);
    }
  return FALSE; // traversal must go on
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

  SHOW3("the root Node:%x, expected type:%s\n", (int)theNode, DISPLAY_TYPE( theElementType));
  DISPLAY_TREE( theNode);

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
	{ /* TODO: cleaner */
	  clearData( this->myRootNode, anyNotRootType);
	  g_node_destroy(this->myRootNode); 
	  this->myRootNode = NULL;
	}
/*       deleteTermInfoList( this->myFirstEntry); */
      
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

  SHOW4("element:%x, voice volume:%d, computed voice volume:%d\n", (int)aCurrentElement, aCurrentElement->myVoiceVolume, aCurrentElement->myComputedVoiceVolume);

  return aTraversalMustBeStoppped;
}
/* > */
/* < computeStyle */
/* update the computed style of the node and its descendants */
static void computeStyle( GNode* theNode)
{
  ENTER("computeStyle");
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
      SHOW2("Root node: %x\n", (int)(this->myRootNode));
	    
      computeStyle( this->myRootNode);

      if (!this)
	{
	  deleteElement( &anElement);
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
	  deleteFrame( &aFrame);
	}
      else
	{
	  GNode* aNode = g_node_insert_data( this->myRootNode, -1, anElement);
	  computeStyle( aNode);
	  SHOW4("add under Root %x, node %x, element=%x\n",
	       (int)(this->myRootNode), (int)aNode, (int)anElement);
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

/* < deleteFrameFromList */
static void deleteFrameFromList(gpointer theEntry, gpointer userData)
{
  ENTER("deleteFrameFromList");

  if (theEntry)
    {
      frame* aFrame=theEntry;
      deleteFrame( &aFrame);
    }
}
/* > */

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
  p = createPoint( 0, 24, 0); /* origin = box bottom left */

  aFrame = createFrame( statusFrame, "status", p, 80, 1);
  aList = g_list_append( aList, (gpointer)aFrame);

  translatePoint( p, 0, -1);
  aFrame = createFrame( screenFrame, "screen", p, 80, 23);
  aList = g_list_append( aList, (gpointer)aFrame);

  translatePoint( p, 0, -23);
  aFrame = createFrame( titleFrame, "title", p, 80, 1);
  aList = g_list_append( aList, (gpointer)aFrame);
  /* > */

  addFrameStyleDocAPI( theDocAPI, aList);

  /* < delete temporary objects */
  g_list_foreach( aList, (GFunc)deleteFrameFromList, NULL);
  g_list_free( aList);
  deletePoint( &p);
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

      SHOW4("Node=%x, type=%s, element=%x\n", (int)aNode, DISPLAY_TYPE(anElement->myType), (int)anElement);

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
	  SHOW2("Next node=%x\n", (int)aNode);
	}
      else
	{
	  aNode = g_node_prev_sibling( aNode);
	  SHOW2("Previous node=%x\n", (int)aNode);
	}

      if ((aNode==NULL) && aNextNodeIsSearched)
	{
	  aNextNodeIsSearched = FALSE;
	  aNode = g_node_prev_sibling( theNode);
	  SHOW2("First previous node=%x\n", (int)aNode);
	}
      /* > */
    } while( aNode);

  SHOW2("frame node=%x\n",(int)aFrameNode);

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

      aOrigin = createPoint( anEntry->myStartingPosition.myCol, 
			     anEntry->myStartingPosition.myLine,
			     0);
      aBox = createBox( aOrigin, ((GString*)(anEntry->myData1))->len, 1);
      /* > */

      SHOW2("myCurrentTextNode=%x\n", (int)(this->myCurrentTextNode));

      /* < look for a frame node */
      if (this->myCurrentTextNode == NULL)
	{
	  GNode* aNode = g_node_first_child( this->myRootNode);
	  aFrameNode = findFrame( aNode, aBox, &aType, &anIntersectionBox);
	}
      else
	{ /* just for time saving: start the search from the current frame */
	  aFrameNode = findFrame( this->myCurrentTextNode->parent, aBox, &aType, &anIntersectionBox);
	}
      /* > */

      /* < if no frame node, take the root node */
      if (!aFrameNode)
	{
	  aFrameNode = this->myRootNode;
	}
      SHOW2("aFrameNode=%x\n", (int)aFrameNode);
      /* > */

      /* < look for the expected (single) text node */
      if ((this->myCurrentTextNode == NULL)
	  || (this->myCurrentTextNode != aFrameNode->children))
	{
	  GNode* aNode = g_node_first_child( aFrameNode);
	  while (aNode)
	    {
	      element* anElement = getElementFromNode( aNode);
	      if (anElement && (anElement->myType == textType))
		{
		  SHOW2("New text node found:%x\n", (int)aNode);
		  break;
		}
	      aNode = g_node_next_sibling( aNode);
	    }
	  this->myCurrentTextNode = aNode; /* new value or NULL */
	}
      /* > */

      /* < if no text node, create it */
      if(this->myCurrentTextNode == NULL)
	{
	  element* aTextElement = createElement( textType, NULL);
	  this->myCurrentTextNode = g_node_insert_data( aFrameNode, -1, aTextElement);
	  SHOW4("insert under frame node:%x, new text node:%x, element:%x\n", 
		(int)aFrameNode, 
		(int)(this->myCurrentTextNode), 
		(int)aTextElement);
	  computeStyle( this->myCurrentTextNode);
	}
      SHOW2("myCurrentTextNode=%x\n", (int)(this->myCurrentTextNode));
      /* > */
      
      /* TBD: frame partially including an entry => split */

      anEntry->myNode = this->myCurrentTextNode;

      deleteBox( &aBox);
      deletePoint( &aOrigin);
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
	  SHOW2("Node: %x\n",(int)theNode);
	}
    }
}
/* > */
/* < appendListEntryDocAPI */
void appendListEntryDocAPI( void* theDocAPI, GList* theList)
{
  document* this = theDocAPI;

  ENTER("appendListEntryDocAPI");

  if (!this || !theList || !(this->myRootNode))
    {
      return;
    }

  this->myFirstEntry = g_list_concat( this->myFirstEntry, theList);

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
  GNode* aHoveredNode = NULL;
  GNode* aParent = NULL;

  ENTER("getStyledListEntryDocAPI");

  if (!this || !(this->myFirstEntry))
    {
      return NULL;
    }

  aList = this->myFirstEntry;

  do 
    {    
      GNode* aNode = NULL;
      int aComputedVolume = undefinedVoiceVolume;
      aNode = getNodeFromList( aList);
      if (!aNode)
	{
	  continue;
	}
      assert(aNode->data);

      SHOW5("list element:%x, node:%x, parent:%x, element:%x\n", 
	    (int)aList,
	    (int)aNode,
	    (int)aNode->parent,
	    (int)(aNode->data));

      /* < look for the hovered element */
      if (aNode->parent && (aNode->parent != aParent))
	{
	  aParent = aNode->parent;
	  SHOW2("aParent:%x\n", (int)aParent);

	  aHoveredNode = NULL;
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
	  element* anElement = (element*)(aNode->data);      
	  aComputedVolume = anElement->myComputedVoiceVolume;
	}
      SHOW2("voice volume:%d\n", aComputedVolume);

      if (aVolume != aComputedVolume)
	{
	  terminfoEntry* anEntry = NULL;
	  aVolume = aComputedVolume;
	  anEntry = get_TSAR_Sequence( aVolume, 1);
	  this->myFirstEntry = g_list_insert_before( this->myFirstEntry, aList, anEntry);
	  SHOW4("insert in list:%x, list element:%x (entry:%x)\n",
		(int)(this->myFirstEntry), 
		(int)aList, 
		(int)anEntry);
	}
    } while( (aList = g_list_next( aList)));

  return this->myFirstEntry;
}
/* > */
/* < setElementTypeDocAPI */
void setElementTypeDocAPI( void* theDocAPI, GList* theEntry, enum elementType theType)
{
  document* this = theDocAPI;
  element* anElement = NULL;
  terminfoEntry* anEntry = NULL;
  GNode* aNode = getNodeFromList( theEntry);

  ENTER("setElementTypeDocAPI");

  if (!this || !aNode || !(aNode->parent))
    {
      return;
    }

  anEntry = theEntry->data;

  switch(theType)
    {
    case linkType:
      {
	/* a link element is added */
	GNode* aLinkNode = NULL;
	anElement= createElement( linkType, NULL);
	aLinkNode = g_node_new( anElement);
	    
	/* insert the new node into the document tree */
	aLinkNode = g_node_insert_after( aNode->parent, aNode, aLinkNode);
	SHOW5("list entry:%x, add under node %x, new node=%x, element:%x\n", 
	      (int)anEntry, 
	      (int)aNode->parent, 
	      (int)aLinkNode, 
	      (int)(aLinkNode->data));
	computeStyle( aLinkNode);

	/* the previous text element is left out, 
	   no particular issue if the link was its only element. 
	*/
	anEntry->myNode = aLinkNode;
      }
      break;
    default:
      break;
    }
  return;
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
      SHOW5("node:%x, element:%x, type:%s, focus state:%s\n",
	   (int)theNode,
	   (int)anElement,
	   DISPLAY_TYPE( anElement->myType),
	   DISPLAY_FOCUS_STATE( anElement->myFocusState));
    }
}
/* > */
/* < setFocusStateDocAPI */
void setFocusStateDocAPI( GList* theEntry, enum elementFocusState theState)
{
  GNode* aNode = getNodeFromList( theEntry);

  ENTER("setFocusStateDocAPI");

  if (!aNode || !(aNode->parent))
    {
      return;
    }

  /* < if the node is hovered, its siblings are not hovered */
  if (theState & hoveredElement)
    {
      g_node_children_foreach( aNode->parent,
			       G_TRAVERSE_ALL,
			       setElementFocusState,
			       (gpointer)notHoveredElement);
    }
  /* > */
  
  setElementFocusState( aNode, (gpointer)theState);
}
/* > */
/* < clearContentDocAPI */
void clearContentDocAPI( void* theDocAPI)
{
  document* this = theDocAPI;

  ENTER("clearContentDocAPI");
  if (this)
    {
      SHOW2("this=%x\n", (int)this);
      clearData( this->myRootNode, textType | linkType);
      DISPLAY_TREE( this->myRootNode);
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
