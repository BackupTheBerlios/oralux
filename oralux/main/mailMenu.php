#!/usr/bin/php
<?php

// raf: utf8

// ----------------------------------------------------------------------------
// mailMenu.php
// $Id: mailMenu.php,v 1.2 2004/11/06 22:49:33 gcasse Exp $
// $Author: gcasse $
// Description: Menu for mail settings (php5)
// $Date: 2004/11/06 22:49:33 $ |
// $Revision: 1.2 $ |
// Copyright (C) 2004 Gilles Casse (gcasse@oralux.org)
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
require_once("cliDialog.php");
require_once("mailConfig.php");
require_once("emacsConfig.php");
require_once("vmConfig.php");
require_once("terminal.php");
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
      $this->_generalProperties();

      do {
	unset($aMenu);
	$aMenu["ADD"]=gettext("Add a new mailbox");

	// Is there already a mailbox ?
	if ($aNumberOfMailsource=$this->_myConf->getNumberOfMailSource())
	  {
	    $aMenu["MODIFY"]=gettext("Modify a mailbox");
	    $aMenu["DELETE"]=gettext("Delete a mailbox");
	  }

	$aMenu["QUIT"]=gettext("Return to main menu");

	$aDialog= new cliDialog($this->_myTerminal, false);
	$aKeyPressed=$aDialog->menu(gettext("About mail boxes"), $aMenu, $aResult);
	unset($aDialog);

	switch( (string)$aResult)
	  {
	  case "ADD":
	    $aMailbox=array();
	    $this->_inputMailbox( $aMailbox);
	    $this->_myConf->addMailSource( $aMailbox);
	    break;

	  case "MODIFY":
	    $aMailbox=array();
	    $aMailboxIdentifier=NULL;
	    if ($aNumberOfMailsource==1)
	      {
		$this->_myConf->initMailSourceIndex();
		$this->_myConf->getNextMailSource( $aMailbox, $aMailboxIdentifier);
	      }
	    else
	      {
		$this->_selectMailbox( gettext("Which mailbox do you want to modify?\n"), $aMailbox, $aMailboxIdentifier);
	      }
	    $this->_inputMailbox( $aMailbox);
	    $this->_myConf->setMailSource( $aMailboxIdentifier, $aMailbox);
	    break;

	  case "DELETE":
	    $aMailboxIdentifier=NULL;
	    
	    if ($aNumberOfMailsource==1)
	      {
		$this->_myConf->initMailSourceIndex();
		$this->_myConf->getNextMailSource( $aMailbox, $aMailboxIdentifier);
	      }
	    else
	      {
		$this->_selectMailbox( gettext("Which mailbox do you want to delete?\n"), $aMailbox, $aMailboxIdentifier);
	      }
	    
	    $aDialog= new cliDialog($this->_myTerminal, false);
	    $aCorrectData = !$aDialog->yesNo(sprintf(gettext("Do you really want to delete %s?"), $aMailbox["label"]));
	    unset($aDialog);
	    
	    if ($aCorrectData)
	      {
		$this->_myConf->deleteMailSource( $aMailboxIdentifier);
	      }
	    break;
	    
	  case "QUIT":
	  default:
	    $aKeyPressed=EscapePressedValue;
	    break;
	}
      } while($aKeyPressed==OkPressedValue);


      $aDialog= new cliDialog($this->_myTerminal, false);
      $aCorrectData = !$aDialog->yesNo(gettext("Do you want to save your changes"));
      unset($aDialog);
   
      if ($aCorrectData)
	{
	  $this->_myConf->save();
	  $a=new vmConfig($this->_myConf);
	  $a->save();

	  $a=new emacsConfig( $this->_myConf);
	  $a->save();
	  echo gettext("Your changes have been saved\n");
	}
    }

  // }}}
  // {{{ _selectMailbox

  // Display a Mailbox selection list
  protected function _selectMailbox( $theTitle, & $theSelectedMailbox, & $theMailboxIdentifier)
    {
      ENTER("mailMenu::_selectMailbox",__LINE__);
      $aMailboxIsSelected=false;
      $this->_myConf->initMailSourceIndex();
      $i=0;
      $anArray=array();
      while ($this->_myConf->getNextMailSource( $anArray, $aIdentifier))
	{
	  $aMenu[$i]=$anArray["label"];
	  $i++;
	}

      if ($i)
	{	  
	  $aDialog= new cliDialog($this->_myTerminal, false);
	  $aResult=$aDialog->menu($theTitle, $aMenu);
	  unset($aDialog);

	  $aMailboxIsSelected=true;

	  $this->_myConf->initMailSourceIndex();
	  $j=1;
	  while ($j <= $i)
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
	      $aKeyPressed=$aDialog->inputBox(gettext("Enter your full name, for example: Victor Hugo"), 
						 $this->_myConf->getValue($aState),
						 $aValue);
	      unset($aDialog);
	      $this->_myConf->setValue($aState, $aValue);
	      break;

	    case "email":
	      $aDialog= new cliDialog($this->_myTerminal, false);
	      $aKeyPressed=$aDialog->inputBox(gettext("Enter your full mailing address, for example: vhugo@pantheon.org"), 
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
	  $aFollowingKeyExists=$this->_getWishedState( $aKeyPressed);
	}
    }

  // }}}
  // {{{ _getWishedState

  protected function _getWishedState( $theKeyPressed)
    {
      ENTER("mailMenu::_getWishedState",__LINE__);
      $aResult = true;

      switch($theKeyPressed)
	{
	case OkPressedValue:
	  if (NULL == next($this->_myConf->mySimpleFields))
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
      reset( $this->_myConf->myMailSourceFields);

      while ($aFollowingKeyExists)
	{
	  $aState=$this->_myConf->myMailSourceFields[$aKey];

	  switch($aState)
	    {
	    case "label":
	      $aDialog= new cliDialog($this->_myTerminal, false);
	      $aKeyPressed=$aDialog->inputBox(gettext("Enter the name of this mailbox, for example: My Mailbox"),
						      $theArray[$aState], $aValue);
	      unset($aDialog);
	      $theArray[ $aState]=$aValue;
	      break;
	      
	    case "host":
	      $aDialog= new cliDialog($this->_myTerminal, false);
	      $aKeyPressed=$aDialog->inputBox(gettext("Enter the hostname of your ISP, for example: pop.pantheon.org"),
						      $theArray[$aState], $aValue);
	      unset($aDialog);
	      $theArray[ $aState]=$aValue;
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
	      $theArray[ $aState]=$aValue;
	      break;

	    case "login":
	      $aDialog= new cliDialog($this->_myTerminal, false);
	      $aKeyPressed=$aDialog->inputBox(gettext("Enter the login"), $theArray[$aState], $aValue);
	      unset($aDialog);
	      $theArray[ $aState]=$aValue;
	      break;

	    case "password":
	      $aDialog= new cliDialog($this->_myTerminal, false);
	      $aKeyPressed=$aDialog->inputBox(gettext("Enter the password"), $theArray[$aState], $aValue);
	      unset($aDialog);
	      $theArray[ $aState]=$aValue;
	      break;

	    case "keep":
	      $aDialog= new cliDialog($this->_myTerminal, false);
	      $aKeyPressed= (!$aDialog->yesNo(gettext("Do you want to keep the messages in the server once they have been fetched?"), $aValue)) ? 1:0;
	      unset($aDialog);
	      $theArray[ $aState]=$aValue;
	      break;

	    default:
	      exit();
	      break;
	    }
	  $aFollowingKeyExists=$this->_getWishedState($aKey);
	}
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