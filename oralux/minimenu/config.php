<?php
// ----------------------------------------------------------------------------
// config.php
// $Id: config.php,v 1.2 2005/01/30 21:43:51 gcasse Exp $
// $Author: gcasse $
// Description: minimenu configuration: manages the XML configuration file. 
// $Date: 2005/01/30 21:43:51 $ |
// $Revision: 1.2 $ |
// Copyright (C) 2004, 2005 Gilles Casse (gcasse@oralux.org)
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

require_once("../main/helpEmul.php");
require_once("element.php");

define("MAINDIR","/usr/share/oralux/minimenu");
define ("CONFIGNAME","oraluxminimenu.xml");

class config extends DomDocument
{
  var $myFields = array("categories");

  var $myMailSourceFieldName = array("label", "host", "port", "login", "password", "keep");

  protected $myFilename;
  protected $myUser;
  protected $configPath="/minimenu";
  protected $myXpath;
  protected $myXPathQuery;
  protected $my_object_is_build=false;

  protected $_mailsourcePath="mailsource";
  protected $myMailSources; // DOMNodelist
  protected $myMailconfigContext; 
  protected $myCurrentMailSource=-1;


  // {{{ config
  public function config($theUser, & $theElement)
    {
      ENTER("config::config",__LINE__);
      parent::__construct();
      $this->$myUser=$theUser;
      $this->myFilename="/home/$theUser/".CONFIGNAME;
      if (!is_file ( $this->myFilename))
	{
	  $aSourceFilename=MAINDIR."/".CONFIGNAME;
	  if (copy( $aSourceFilename, $this->myFilename) == false)
	    {
	      $aError=sprintf("Error: can't copy file: %s to %s\n", $aSourceFilename, $this->myFilename);
	      ErrorMessage($aError, __LINE__, __FILE__, '$Revision: 1.2 $');
	      return;
	    }
	}
      if (!is_readable( $this->myFilename))
	{
	      $aError=sprintf("Error: file unreadable: %s\n", $this->myFilename);
	      ErrorMessage($aError, __LINE__, __FILE__, '$Revision: 1.2 $');
	      return;
	}

      $this->load( $this->myFilename);

      $this->myXpath = new domXPath($this);
      $this->myXPathQuery = $this->myXpath->query( $this->configPath."/*");

      $aNumberOfItems = $this->myXPathQuery->length;
      for ($i=0; $i<$aNumberOfItems; $i++)
	{
	  $aNode=$this->myXPathQuery->item($i);
	  
	  unset($anElement);
	  $this->getElement( $aNode, & $anElement);


	  $theElement[]=$anElement;
	}

      $this->my_object_is_build=true;
    }

  // }}}

  // {{{ getElement
  protected function getElement( & $theNode, & $theElement)
    {
      ENTER("config::getElement",__LINE__);
      SHOW("====== nodeName=".$theNode->nodeName." ======\n");

      $theElement=NULL;
      $anArray=NULL;
      $aName="";
      $aElementIsHidden=false;
      if ($theNode->nodeType!==XML_ELEMENT_NODE) 
	{
	  return;
	}

      if ($theNode->hasAttributes())
	{
	  foreach ($theNode->attributes as $a) 
	    {
	      $anArray[$a->name]=$a->value;
	    }
	}

      if ($theNode->hasChildNodes())
	{
	  $a=$theNode->firstChild;
	  while( $a!=NULL)
	    {
	      if ($a->nodeType===XML_ELEMENT_NODE)
		{
		  $b=$a->firstChild;
		  if (($b!=NULL) && $b->isSameNode($a->lastChild))
		    {
		      $anArray[$a->nodeName]=$b->nodeValue;
		    }
		}
	      $a=$a->nextSibling;
	    }
	}

      switch($theNode->nodeName)
	{
	case MINIMENU_CATEGORY:
	  $theElement = new category($anArray);
	  break;
	case MINIMENU_PROGRAM:
	  $theElement = new program($anArray);
	  break;
	case MINIMENU_DOCUMENT:	  
	  $theElement = new document($anArray);
	  break;
	default:
	  $aError=sprintf("Error: unknown element: %s\n", $aNode->nodeName);
	  ErrorMessage($aError, __LINE__, __FILE__, '$Revision: 1.2 $');
	  break;
	}
    }

    // }}}


  function isBuild()
    {
      return $this->my_object_is_build;
    }

  function getUser()
    {
      return $this->$myUser;
    }

//   // getValue of one simple node (see mySimpleFields)
//   function getValue( $theField)
//     {
//       $aNodeList = $this->myXpath->query( $theField, $this->myMailconfigContext);
//       return $aNodeList->item(0)->textContent;
//     }

//   // getValue of one simple node
//   function setValue( $theField, $theValue)
//     {
//       $this->_setText( $this->myMailconfigContext, $theField, $theValue);
//     }

//   // Get the next mailsource node, and its identifier
//   // Return true if the next mailsource has been found
//   function getNextMailSource( & $theArray, & $theIdentifier)
//     {
//       $theIdentifier=0;
//       $this->myCurrentMailSource++;
//       $aNode= & $this->myMailSources->item ( $this->myCurrentMailSource);

//       if ($aNode != NULL)
// 	{
// 	  foreach($this->myMailSourceFieldName as $aField)
// 	    {
// 	      $aNodeList = $this->myXpath->query( $aField, $aNode);
// 	      $theArray[$aField] = $aNodeList->item(0)->textContent;
// 	    }
// 	  $theIdentifier=$this->myCurrentMailSource; 
// 	}

//       return ($aNode != NULL);
//     }

//   function getNumberOfMailSource()
//     {
//       $i=0;
//       $this->initMailSourceIndex();
//       $aMailbox=array();
//       while ($this->getNextMailSource( $aMailbox, $aIdentifier))
// 	{
// 	  $i++;
// 	}
//       return $i;
//     }

//   function setMailSource( $theMailboxIdentifier, & $theArray)
//     {
//       if ((theMailboxIdentifier > $this->myCurrentMailSource)||(theMailboxIdentifier<0))
// 	{
// 	  return;
// 	}

//       $aMailsourceNode = & $this->myMailSources->item ( $theMailboxIdentifier);

//       foreach($theArray as $aField=>$aValue)
// 	{
// 	  $this->_setText( $aMailsourceNode, $aField, $aValue);
// 	}
//     }

//   function deleteMailSource( $theMailboxIdentifier)
//     {
//       if (($theMailboxIdentifier>$this->myCurrentMailSource)||($theMailboxIdentifier<0))
// 	{
// 	  return;
// 	}
//       $this->myMailconfigContext->removeChild ( $this->myMailSources->item ( $theMailSourceIndex));
//       $this->initMailSourceIndex();
//     }

//   function addMailSource( & $theArray)
//     {
//       $aNewSource = $this->createElement( $this->_mailsourcePath);
//       $this->myMailconfigContext->appendChild ( $aNewSource);

//       foreach($theArray as $aField=>$aValue)
// 	{
// 	  $aNewText = $this->createTextNode( $aValue);
// 	  $aNewField = $this->createElement( $aField);
// 	  $aNewField->appendChild( $aNewText);
// 	  $aNewSource->appendChild( $aNewField);
// 	}
//       $this->initMailSourceIndex();
//     }

//   // Init to point to the first mailsource node
//   function initMailSourceIndex()
//     {
//       $this->myMailSources = $this->myXpath->query( $this->_mailsourcePath, $this->myMailconfigContext);
//       $this->myCurrentMailSource=-1;
//     }

//   // Save the configuration file
//   function save()
//     {
//       if (unlink($this->myFilename)==FALSE)
// 	{
// 	  $aError=sprintf("Error concerning file: %s\n", $this->myFilename);
// 	  ErrorMessage($aError, __LINE__, __FILE__, '$Revision: 1.2 $');
// 	  return;
// 	}
//       if (touch($this->myFilename)==FALSE)
// 	{
// 	  $aError=sprintf("Error concerning file: %s\n", $this->myFilename);
// 	  ErrorMessage($aError, __LINE__, __FILE__, '$Revision: 1.2 $');
// 	  return;
// 	}
//       if (chmod ( $this->myFilename, 0600)==FALSE)
// 	{
// 	  $aError=sprintf("Error concerning file: %s\n", $this->myFilename);
// 	  ErrorMessage($aError, __LINE__, __FILE__, '$Revision: 1.2 $');
// 	  return;
// 	}

//       parent::save($this->myFilename);
//     }

//   // Set a text node
//   protected function _setText( &$theParentNode, $theField, $theNewText)
//     {
//       $aNodeList = $this->myXpath->query( $theField, $theParentNode);

//       if (($aNodeList!=NULL) && ($aNodeList->item(0)!=NULL))
// 	{
// 	  $theParentNode->removeChild ( $aNodeList->item(0));
// 	}

//       $aNewText=$this->createTextNode ( $theNewText);
//       $aNewName = $this->createElement( $theField);
//       $aNewName->appendChild($aNewText);

//       $theParentNode->appendChild ( $aNewName);
//     }
}
?>