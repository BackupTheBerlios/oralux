<?php
// raf: utf8
// ----------------------------------------------------------------------------
// mailConfig.php
// $Id: mailConfig.php,v 1.6 2005/04/03 00:36:28 gcasse Exp $
// $Author: gcasse $
// Description: Mail settings (php5)
// $Date: 2005/04/03 00:36:28 $ |
// $Revision: 1.6 $ |
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

class mailConfig extends DomDocument
{
  var $mySimpleFields = array("name", "email", "delay");
  var $myMailSourceFieldName = array("label", "host", "port", "login", "password", "keep", "smtp");

  protected $_myFilename;
  protected $_myUser;
  protected $_myTemplateFilename="/home/%s/oraluxmail.xml";
  protected $_mailconfigPath="/mailconfig";
  protected $_mailsourcePath="mailsource";
  protected $_myXpath;
  protected $_myMailSources; // DOMNodelist
  protected $_myMailconfigContext; 
  protected $_myCurrentMailSource=-1;
  protected $_my_object_is_build=false;


  protected $_myInitialFile="<?xml version=\"1.0\"?>\n<mailconfig version=\"1.0\">\n<name></name><email></email><delay></delay>\n</mailconfig>";

  function mailConfig($theUser)
    {
      parent::__construct();
      
      $this->$_myUser=$theUser;

      $this->_myFilename=sprintf( $this->_myTemplateFilename, $theUser);
      if (!is_file ( $this->_myFilename))
	{
	  $fd=fopen( $this->_myFilename, "w");
	  if ($fd==NULL)
	    {
	      $aError=sprintf("Error: can't open file: %s\n", $this->_myFilename);
	      ErrorMessage($aError, __LINE__, __FILE__, '$Revision: 1.6 $');
	      return;
	    }
	  fwrite ( $fd, $this->_myInitialFile);
	  fclose( $fd);
	}
      if (!is_readable( $this->_myFilename))
	{
	      $aError=sprintf("Error: file unreadable: %s\n", $this->_myFilename);
	      ErrorMessage($aError, __LINE__, __FILE__, '$Revision: 1.6 $');
	      return;
	}

      $this->load( $this->_myFilename);

      $this->_myXpath = new domxpath($this);
      $this->_myMailconfigContext = $this->_myXpath->query( $this->_mailconfigPath)->item(0);
      $this->initMailSourceIndex();
      $this->_my_object_is_build=true;
    }

  function isBuild()
    {
      return $this->_my_object_is_build;
    }

  function getUser()
    {
      return $this->$_myUser;
    }

  // getValue of one simple node (see mySimpleFields)
  function getValue( $theField)
    {
      $aNodeList = $this->_myXpath->query( $theField, $this->_myMailconfigContext);
      return $aNodeList->item(0)->textContent;
    }

  // getValue of one simple node
  function setValue( $theField, $theValue)
    {
      $this->_setText( $this->_myMailconfigContext, $theField, $theValue);
    }

  // Get the next mailsource node, and its identifier
  // Return true if the next mailsource has been found
  function getNextMailSource( & $theArray, & $theIdentifier)
    {
      $theIdentifier=0;
      $this->_myCurrentMailSource++;
      $aNode= & $this->_myMailSources->item ( $this->_myCurrentMailSource);

      if ($aNode != NULL)
	{
	  foreach($this->myMailSourceFieldName as $aField)
	    {
	      $aNodeList = $this->_myXpath->query( $aField, $aNode);
	      $theArray[$aField] = $aNodeList->item(0)->textContent;
	    }
	  $theIdentifier=$this->_myCurrentMailSource; 
	}

      return ($aNode != NULL);
    }

  function getNumberOfMailSource()
    {
      $i=0;
      $this->initMailSourceIndex();
      $aMailbox=array();
      while ($this->getNextMailSource( $aMailbox, $aIdentifier))
	{
	  $i++;
	}
      return $i;
    }

  function setMailSource( $theMailboxIdentifier, & $theArray)
    {
      if ((theMailboxIdentifier > $this->_myCurrentMailSource)||(theMailboxIdentifier<0))
	{
	  return;
	}

      $aMailsourceNode = & $this->_myMailSources->item ( $theMailboxIdentifier);

      foreach($theArray as $aField=>$aValue)
	{
	  $this->_setText( $aMailsourceNode, $aField, $aValue);
	}
    }

  function deleteMailSource( $theMailboxIdentifier)
    {
      if (($theMailboxIdentifier>$this->_myCurrentMailSource)||($theMailboxIdentifier<0))
	{
	  return;
	}
      $this->_myMailconfigContext->removeChild ( $this->_myMailSources->item ( $theMailSourceIndex));
      $this->initMailSourceIndex();
    }

  function addMailSource( & $theArray)
    {
      $aNewSource = $this->createElement( $this->_mailsourcePath);
      $this->_myMailconfigContext->appendChild ( $aNewSource);

      foreach($theArray as $aField=>$aValue)
	{
	  $aNewText = $this->createTextNode( $aValue);
	  $aNewField = $this->createElement( $aField);
	  $aNewField->appendChild( $aNewText);
	  $aNewSource->appendChild( $aNewField);
	}
      $this->initMailSourceIndex();
    }

  // Init to point to the first mailsource node
  function initMailSourceIndex()
    {
      $this->_myMailSources = $this->_myXpath->query( $this->_mailsourcePath, $this->_myMailconfigContext);
      $this->_myCurrentMailSource=-1;
    }

  // Save the configuration file
  function save()
    {
      if (unlink($this->_myFilename)==FALSE)
	{
	  $aError=sprintf("Error concerning file: %s\n", $this->_myFilename);
	  ErrorMessage($aError, __LINE__, __FILE__, '$Revision: 1.6 $');
	  return;
	}
      if (touch($this->_myFilename)==FALSE)
	{
	  $aError=sprintf("Error concerning file: %s\n", $this->_myFilename);
	  ErrorMessage($aError, __LINE__, __FILE__, '$Revision: 1.6 $');
	  return;
	}
      if (chmod ( $this->_myFilename, 0600)==FALSE)
	{
	  $aError=sprintf("Error concerning file: %s\n", $this->_myFilename);
	  ErrorMessage($aError, __LINE__, __FILE__, '$Revision: 1.6 $');
	  return;
	}

      parent::save($this->_myFilename);
    }

  // Set a text node
  protected function _setText( &$theParentNode, $theField, $theNewText)
    {
      $aNodeList = $this->_myXpath->query( $theField, $theParentNode);

      if (($aNodeList!=NULL) && ($aNodeList->item(0)!=NULL))
	{
	  $theParentNode->removeChild ( $aNodeList->item(0));
	}

      $aNewText=$this->createTextNode ( $theNewText);
      $aNewName = $this->createElement( $theField);
      $aNewName->appendChild($aNewText);

      $theParentNode->appendChild ( $aNewName);
    }
}

?>