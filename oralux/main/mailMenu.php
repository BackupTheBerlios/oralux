#! /usr/bin/php
<?php

// raf: utf8

// ----------------------------------------------------------------------------
// mailMenu.php
// $Id: mailMenu.php,v 1.8 2005/04/03 00:36:28 gcasse Exp $
// $Author: gcasse $
// Description: Menu for mail settings (php5)
// $Date: 2005/04/03 00:36:28 $ |
// $Revision: 1.8 $ |
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
require_once("../dialog/cliDialog.php");
require_once("mailConfig.php");
require_once("emacsConfig.php");
require_once("vmConfig.php");
require_once("../dialog/terminal.php");
require_once("helpEmul.php");

class mailMenu
{
  protected $_myConf;
  protected $_myTerminal;
  protected $_myUser; // e.g. "knoppix"

  // {{{ mailMenu

  function mailMenu( $theTerminal, $theUser="knoppix")
    {
      ENTER("mailMenu::mailMenu",__LINE__);
      $this->_myConf=new mailConfig($theUser);
      $this->_myTerminal=$theTerminal;
      $this->_myUser=$theUser;
    }

  // }}}
  // {{{ start

  function start()
    {
      ENTER("mailMenu::start",__LINE__);

      if (!$this->_myConf->isBuild())
	{
	  return;
	}

      $this->_generalProperties();

      do {
	unset($aMenu);

	$aIndex=-1;

	$aAction[ ++$aIndex]="ADD";
	$aMenu[ $aIndex]=gettext("Add a mailbox");

	// Is there already a mailbox ?
	if ($aNumberOfMailsource=$this->_myConf->getNumberOfMailSource())
	  {
	    $aAction[ ++$aIndex]="MODIFY";
	    $aMenu[ $aIndex]=gettext("Modify a mailbox");

	    $aAction[ ++$aIndex]="DELETE";
	    $aMenu[ $aIndex]=gettext("Delete a mailbox");
	  }

	$aAction[ ++$aIndex]="QUIT";
	$aMenu[ $aIndex]=gettext("Return to main menu");

	$aDialog= new cliDialog($this->_myTerminal, false);
	$aText=NULL;
	$aSelectedOption=NULL;
	$aKeyIsDisplayed=true;
	$aKeyPressed=$aDialog->menu(gettext("About mail boxes"), $aMenu, $aResult, 
				    $aText, $aSelectedOption, 
				    $aKeyIsDisplayed);
	unset($aDialog);

	if ($aKeyPressed==OkPressedValue)
	  {
	    switch( $aAction[$aResult])
	      {
	      case "ADD":
		$aKeyPressed=$this->_addMailbox();
		break;

	      case "MODIFY":
		$aKeyPressed=$this->_modifyMailbox();
		break;

	      case "DELETE":
		$aKeyPressed=$this->_deleteMailbox();
		break;
	 
	      case "QUIT":
	      default:
		$aKeyPressed=EscapePressedValue;
		break;
	      }
	  }
      } while($aKeyPressed==OkPressedValue);

      $aDialog= new cliDialog($this->_myTerminal, false);
      $aKeyPressed = $aDialog->yesNo(gettext("Do you want to save your changes"));
      unset($aDialog);
   
      if ($aKeyPressed==OkPressedValue)
	{
	  $this->_myConf->save();
	  $a=new vmConfig($this->_myConf);
	  $a->save();

	  $a=new emacsConfig( $this->_myConf);
	  if ($a->save() == TRUE)
	    {
	      echo gettext("\nYour changes have been saved");
	    }
	}
    }

  // }}}
  // {{{ _addMailbox
  protected function _addMailbox()
    {
      $aMailbox=array();
      $aKeyPressed=$this->_inputMailbox( $aMailbox);
      if ($aKeyPressed == OkPressedValue)
	{
	  $this->_myConf->addMailSource( $aMailbox);
	}
      return $aKeyPressed;
    }
  // }}}
  // {{{ _modifyMailbox
  protected function _modifyMailbox()
    {
      $aMailbox=array();
      $aMailboxIdentifier=NULL;
      $aKeyPressed=OkPressedValue;
      
      if ($this->_myConf->getNumberOfMailSource()==1)
	{
	  $this->_myConf->initMailSourceIndex();
	  $this->_myConf->getNextMailSource( $aMailbox, $aMailboxIdentifier);
	}
      else
	{
	  $aKeyPressed=$this->_selectMailbox( gettext("\nWhich mailbox do you want to modify?"), $aMailbox, $aMailboxIdentifier);
	}
      
      if (($aKeyPressed==OkPressedValue)
	  && ($this->_inputMailbox( $aMailbox) == OkPressedValue))
	{
	  $this->_myConf->setMailSource( $aMailboxIdentifier, $aMailbox);
	}
      
      return $aKeyPressed;
    }
  // }}}
  // {{{ _deleteMailbox
  protected function _deleteMailbox()
    {
      $aMailboxIdentifier=NULL;
      $aKeyPressed=OkPressedValue;
      
      if ($this->_myConf->getNumberOfMailSource()==1)
	{
	  $this->_myConf->initMailSourceIndex();
	  $this->_myConf->getNextMailSource( $aMailbox, $aMailboxIdentifier);
	}
      else
	{
	  $aKeyPressed=$this->_selectMailbox( gettext("\nWhich mailbox do you want to delete?"), $aMailbox, $aMailboxIdentifier);
	}
      
      if ($aKeyPressed==OkPressedValue)
	{
	  $aDialog= new cliDialog($this->_myTerminal, false);
	  $aKeyPressed = $aDialog->yesNo(sprintf(gettext("Do you really want to delete %s?"), $aMailbox["label"]));
	  unset($aDialog);
	  
	  if ($aKeyPressed==OkPressedValue)
	    {
	      $this->_myConf->deleteMailSource( $aMailboxIdentifier);
	    }
	}
      
      return $aKeyPressed;
    }
  // }}}

  // {{{ _selectMailbox

  // Display a Mailbox selection list
  protected function _selectMailbox( $theTitle, & $theSelectedMailbox, & $theMailboxIdentifier)
    {
      ENTER("mailMenu::_selectMailbox",__LINE__);
      $aMailboxIsSelected=false;
      $aKeyPressed=EscapePressedValue;

      // Building the menu from the known mailboxes
      $this->_myConf->initMailSourceIndex();
      $i=0;
      $anArray=array();
      while ($this->_myConf->getNextMailSource( $anArray, $aIdentifier))
	{
	  $aMenu[$i]=$anArray["label"];
	  $i++;
	}

      if ($i>0)
	{ 
	  // Select a mailbox from the menu
	  $aDialog= new cliDialog($this->_myTerminal, false);
	  $aText=NULL;
	  $aSelectedOption=NULL;
	  $aKeyIsDisplayed=true;
	  $aKeyPressed=$aDialog->menu($theTitle, $aMenu, $aResult, 
				      $aText, $aSelectedOption, 
				      $aKeyIsDisplayed);

	  if ($aKeyPressed==OkPressedValue)
	    {
	      unset($aDialog);      
	      $aMailboxIsSelected=true;
	      $this->_myConf->initMailSourceIndex();
	      $j=0;
	      while ($j < $i)
		{
		  $this->_myConf->getNextMailSource( $theSelectedMailbox, $theMailboxIdentifier);
		  if ($aResult==$theMailboxIdentifier)
		    {
		      break;
		    }
		  $j++;
		}
	    }
	}
      return $aKeyPressed;
    }

  // }}}
  // {{{ _generalProperties

  protected function _generalProperties()
    {
      ENTER("mailMenu::_generalProperties",__LINE__);
      $aFollowingKeyExists=true;
      reset( $this->_myConf->mySimpleFields);

      while ($aFollowingKeyExists)
	{
	  $aState=current($this->_myConf->mySimpleFields);
	  $aKeyPressed=OkPressedValue;
	  switch($aState)
	    {
	    case "name":
	      $aDialog= new cliDialog($this->_myTerminal, false);
	      $aKeyPressed=$aDialog->inputBox(gettext("Enter your full name, for example: Paul Smith"), 
						 $this->_myConf->getValue($aState),
						 $aValue);
	      unset($aDialog);
	      $this->_myConf->setValue($aState, $aValue);
	      break;

	    case "email":
	      $aDialog= new cliDialog($this->_myTerminal, false);
	      $aKeyPressed=$aDialog->inputBox(gettext("Enter your full mailing address, for example: paul@smith.com"), 
						      $this->_myConf->getValue($aState),
						      $aValue);
	      unset($aDialog);

	      $this->_myConf->setValue($aState, $aValue);
	      break;

	    case "delay":
	      $aDialog= new cliDialog($this->_myTerminal, false);
	      $aKeyPressed=$aDialog->inputBox(gettext("Enter how often your mailboxes must be checked. For example, 20 means 20 minutes"), 
						      $this->_myConf->getValue($aState),
						      $aValue);
	      unset($aDialog);
	      $this->_myConf->setValue($aState, $aValue);
	      break;

	    default:
	      exit();
	      break;
	    }
	  $aFollowingKeyExists=$this->_getWishedState( $this->_myConf->mySimpleFields, $aKeyPressed);
	}
    }

  // }}}
  // {{{ _getWishedState

  protected function _getWishedState( & $theArray, $theKeyPressed)
    {
      ENTER("mailMenu::_getWishedState",__LINE__);
      $aResult = true;

      switch($theKeyPressed)
	{
	case OkPressedValue:
	  if (NULL == next($theArray))
	    {
	      $aResult = false;
	    }
	  break;

	default:
	case CancelPressedValue:
	case EscapePressedValue:
	  $aResult = false;
	  break;
	}

      return $aResult;
    }

  // }}}
  // {{{ _inputMailbox

  protected function _inputMailbox( & $theArray)
    {
      ENTER("mailMenu::_inputMailbox",__LINE__);
      $aFollowingKeyExists=true;
      reset( $this->_myConf->myMailSourceFieldName);

      while ($aFollowingKeyExists)
	{
	  //	  $aState=$this->_myConf->myMailSourceFieldName[$aKey];
	  $aState=current($this->_myConf->myMailSourceFieldName);
	  $aKeyPressed=OkPressedValue;

	  switch($aState)
	    {
	    case "label":
	      $aDialog= new cliDialog($this->_myTerminal, false);
	      $aKeyPressed=$aDialog->inputBox(gettext("Enter the name of this mailbox, for example: My Mailbox"),
					      $theArray[$aState], $aValue);
	      unset($aDialog);
	      break;
	      
	    case "host":
	      $aDialog= new cliDialog($this->_myTerminal, false);
	      $aKeyPressed=$aDialog->inputBox(gettext("Enter the name of the POP server, for example: pop.smith.com"),
						      $theArray[$aState], $aValue);
	      unset($aDialog);
	      break;

	    case "port":
	      $aPort=$theArray[ $aState];
	      if ((strlen($aPort)==0) || !is_numeric($aPort))
		{
		  $aPort="110";
		}
	      $aDialog= new cliDialog($this->_myTerminal, false);
	      $aKeyPressed=$aDialog->inputBox(gettext("Enter the port or press return"), $aPort, $aValue);
	      unset($aDialog);
	      break;

	    case "login":
	      $aDialog= new cliDialog($this->_myTerminal, false);
	      $aKeyPressed=$aDialog->inputBox(gettext("Enter the login"), $theArray[$aState], $aValue);
	      unset($aDialog);
	      break;

	    case "password":
	      $aDialog= new cliDialog($this->_myTerminal, false);
	      $aKeyPressed=$aDialog->inputBox(gettext("Enter the password"), $theArray[$aState], $aValue);
	      unset($aDialog);
	      break;

	    case "keep":
	      $aDialog= new cliDialog($this->_myTerminal, false);
	      $aValue= $aDialog->yesNo(gettext("Do you want to keep the messages in the server once they have been fetched?"));
	      unset($aDialog);
	      if ($aKeyPressed!=EscapePressedValue)
		{
		  $theArray[ $aState]=$aValue;
		}
	      break;

	    case "smtp":
	      $aDialog= new cliDialog($this->_myTerminal, false);
	      $aKeyPressed=$aDialog->inputBox(gettext("Enter the name of the S M T P server, for example: smtp.smith.com"),
					      $theArray[$aState], $aValue);
	      unset($aDialog);
	      break;

	    default:
	      exit();
	      break;
	    }

	  if ($aKeyPressed==OkPressedValue)
	    {
	      $theArray[ $aState]=$aValue;
	    }

	  $aFollowingKeyExists=$this->_getWishedState( $this->_myConf->myMailSourceFieldName, $aKeyPressed);
	}
      return $aKeyPressed;
    }

  // }}}
}

$aTerminal=new enhancedTerminal();
if (!$aTerminal->isBuild())
{
  $aTerminal=new dumbTerminal();
}

$aMailMenu=new mailMenu( $aTerminal, "knoppix");
$aMailMenu->start();


?>