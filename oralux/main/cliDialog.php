<?php
// ----------------------------------------------------------------------------
// cliDialog.php
// $Id: cliDialog.php,v 1.3 2004/10/24 21:34:34 gcasse Exp $
// $Author: gcasse $
// Description: command line based dialog (menu, yes/no question, dialog box,...)
// $Date: 2004/10/24 21:34:34 $ |
// $Revision: 1.3 $ |
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

require_once("helpEmul.php");

// {{{ Constants

define ("OkPressedValue", 0);
define ("CancelPressedValue", 1);
define ("EscapePressedValue", 255);
define ("PreviousProcessedValue",1000);
define ("ListProcessedValue",2000); // for internal use
define ("InputProcessedValue",2001); // for internal use
define ("CheckboxProcessedValue",2002); // for internal use

define ("OtherAnswer", 0);
define ("PreviousDialog", 1);
define ("NextDialog", 2);

define ("MessageNavigationMenu", 1);
define ("MessageNavigationRadio", 2);
define ("MessageNavigationCheckbox", 3);
define ("MessageNavigationInputBox", 4);
define ("MessageNavigationInputBoxDefaultButton", 5);

define ("MinimumItemsInALongList",10);

// terminal::getChar return status
define("getCharOK", 0);
define("getCharDownArrowKey", 1);
define("getCharUpArrowKey", 2);
define("getCharSelectedOption", 3);
define("getCharUnselectedOption", 4);
define("getCharNextArea", 5);
define("getCharPreviousArea", 6);
define("getCharApply", 7);
define("getCharEscape", 8);

define ("areaList", 0);
define ("areaButton", 1);
define ("areaInputBox", 2);
define ("areaMessage", 3);

define ("verbose", 0);
define ("notVerbose", 1);

// }}}

// {{{ cliArea: Area in a dialog. It can be a list, a button. It is possible to jump from 'area' to 'area'.

abstract class cliArea 
{
  protected $_myAnnounce; // what is announced when the area has the focus
  protected $_myVerbosity=verbose; // concerns how messages will be verbose

  abstract public function getType(); // list, button

  // {{{ announceTypeOfArea

  public function announceTypeOfArea()
  {
    ENTER("cliArea::announceTypeOfArea",__LINE__);
    foreach ($this->_myAnnounce[ $this->_myVerbosity ] as $aMessage)
      {
	echo $aMessage;
      }
  }

  // }}}
  // {{{ setVerbosity
  function setVerbosity( $theVerbosity)
    {
      ENTER("cliArea::setVerbosity",__LINE__);
      $this->_myVerbosity=$theVerbosity;
    }
  // }}}
  // {{{ sub-item: jumpToFirstItem, jumpToLastItem, gotoNextItem, gotoPreviousItem

  function jumpToFirstItem()
    {
      ENTER("cliArea::jumpToFirstItem",__LINE__);
      return true;
    }
  function jumpToLastItem()
    {
      ENTER("cliArea::jumpToLastItem",__LINE__);
      return true;
    }

  // gotoNextItem: will return true for a list and no for a button
  function gotoNextItem(){return false;}
  function gotoPreviousItem(){return false;}

  function announceItem()
    {
    ENTER("cliArea::announceItem",__LINE__);
    }

  // }}}
  // {{{ _isYesOrNo

  // theResult equals true if the string means 'yes' or false if it is 'no'.
  // otherwise the function returns false
  protected function _isYesOrNo( $theString, & $theResult)
    {
      ENTER("cliArea::_isYesOrNo",__LINE__);
      $aStatus=false;
      switch (strtolower( $theString))
	{
	case gettext("yes"):
	case gettext("y"):
	  $aStatus=true;
	  $theResult=true;
	   break;
	   
	case gettext("no"):
	case gettext("n"):
	  $aStatus=true;
	  $theResult=false;
	  break;
	  
	 default:
	   break;
	 }
      return $aStatus;
    }

  // }}}
  // {{{ getInput
  function getInput( $theTerminal, & $theInput, $theLastInput )
    {
      ENTER("cliArea::getInput",__LINE__);
      return $theTerminal->getChar( $theInput);
    }
  // }}}
  // {{{ processInput

  function processInput( $theResult, $theInput, $theTerminal)
    {
      ENTER("cliArea::processInput",__LINE__);
      $aResult=$theResult;

      if ($aResult==getCharOK)
	{
	  switch( $theInput)
	    {
	    case "\n":
	    case "\r":
	      // Note: in a dumb terminal, the return key jumps to the next line.
	      // Whereas otherwise 'return' means 'apply'.
	      if ($theTerminal->IsDumbTerminal())
		{
		  $aResult=getCharDownArrowKey;
		}
	      else
		{
		  $aResult=getCharApply;
		}
	      break;
	      
	    case "\t": 
	      $aResult=getCharNextArea;
	      break;
		  
	    default:
	      if ($this->_isYesOrNo( $theInput, $aYesResult) && $aYesResult) 
		{
		  $aResult=getCharApply;
		}
	      else
		{ // go to next field
		  $aResult=getCharDownArrowKey;
		}
	      break;
	    }
	}
      return $aResult;
    }

  // }}}
}

// }}}
// {{{ cliAreaManager

class cliAreaManager 
{
  protected $_myArea=NULL; // array of objects: e.g. a list and a few buttons
  protected $_myInput;

  // {{{ addArea
  function addArea( $theArea)
    {
      ENTER("cliAreaManager::addArea",__LINE__);
      $this->_myArea[]=$theArea;
    }
  // }}}
  // {{{ getCurrentArea
  function getCurrentArea()
    {
      ENTER("cliAreaManager::getCurrentArea",__LINE__);
      return current($this->_myArea);
    }
  // }}}
  // {{{ gotoNextArea

  function gotoNextArea()
    {
      ENTER("cliAreaManager::gotoNextArea",__LINE__);
      $aCurrentArea=next($this->_myArea);
      if ($aCurrentArea==false)
	{
	  reset($this->_myArea);
	  $aCurrentArea=current($this->_myArea);
	}
      $aCurrentArea->announceTypeOfArea();
    }

  // }}}
  // {{{ gotoPreviousArea
  function gotoPreviousArea()
    {
      ENTER("cliAreaManager::gotoPreviousArea",__LINE__);
      $aCurrentArea=prev($this->_myArea);
      if ($aCurrentArea==false)
	{
	  $aCurrentArea=end($this->_myArea);
	}
      $aCurrentArea->announceTypeOfArea();
    }
  // }}}
  // {{{ jumpToFirstArea
  function jumpToFirstArea()
    {
      ENTER("cliAreaManager::jumpToFirstArea",__LINE__);
      reset($this->_myArea);
      return current($this->_myArea);
    }
  // }}}
  // {{{ gotoNextItemInArea, gotoPreviousItemInArea
  // Useful to browse the steps of the area (typically the list options).
  function gotoNextItemInArea()
    {
      ENTER("cliAreaManager::gotoNextItemInArea",__LINE__);
      $aCurrentArea=current($this->_myArea);
      return $aCurrentArea->gotoNextItem();
    }
  function gotoPreviousItemInArea()
    {
      ENTER("cliAreaManager::gotoPreviousItemInArea",__LINE__);
      $aCurrentArea=current($this->_myArea);
      return $aCurrentArea->gotoPreviousItem();
    }
  function jumpToFirstItemInArea()
    {
      ENTER("cliAreaManager::jumpToFirstItemInArea",__LINE__);
      $aCurrentArea=current($this->_myArea);
      return $aCurrentArea->jumpToFirstItem();
    }
  function jumpToLastItemInArea()
    {
      ENTER("cliAreaManager::jumpToLastItemInArea",__LINE__);
      $aCurrentArea=current($this->_myArea);
      return $aCurrentArea->jumpToLastItem();
    }
  function announceItemInArea()
    {
      ENTER("cliAreaManager::announceItemInArea",__LINE__);
      $aCurrentArea=current($this->_myArea);
      return $aCurrentArea->announceItem();
    }
  // }}}
  // {{{ processKeys
  function processKeys( & $theTerminal, & $theResult)
    {
      ENTER("cliAreaManager::processKeys",__LINE__);
      $aCurrentArea=$this->jumpToFirstArea(); // first element (list)
      $aCurrentArea->announceTypeOfArea();
      $this->jumpToFirstItemInArea();
      $aKeyPressedValue=EscapePressedValue;
      $this->_myInput="";

      $anInputIsExpected=true;
      while( $anInputIsExpected)
	{
	  $this->announceItemInArea();
	  $anArea=$this->getCurrentArea();

	  $theResult=$anArea->getInput( $theTerminal, $anInput, $this->_myInput );
	  $this->_myInput=$anInput;
	  $theResult=$anArea->processInput( $theResult, $anInput, $theTerminal);

	  switch($theResult)
	    {
	    case getCharDownArrowKey:
	      if ($this->gotoNextItemInArea() == false)
		{
		  $this->jumpToFirstItemInArea();
		  $this->gotoNextArea();
		}
	      break;

	    case getCharUpArrowKey:
	      if ($this->gotoPreviousItemInArea() == false)
		{ 
		  $this->jumpToLastItemInArea();
		  $this->gotoPreviousArea();
		}
	      break;

	    case getCharNextArea:
	      $this->gotoNextArea();
	      break;

	    case getCharPreviousArea:
	      $this->gotoPreviousArea();
	      break;

	    case getCharApply:
	      //RAF		      $this->myLastAnswer==NextDialog;
	      $anArea = $this->getCurrentArea();
	      $aKeyPressedValue = $anArea->apply( $theResult);
	      $anInputIsExpected=false;
	      break;

	    case getCharEscape:
	      $theResult=NULL;
	      $aKeyPressedValue=EscapePressedValue;
	      $anInputIsExpected=false;
	      break;
	    }
	}
      return $aKeyPressedValue;
    }
  // }}}
}

// }}}

// {{{ cliList
class cliList extends cliArea
{
  protected $_myOption;
  protected $_myKeyIsDisplayed=true;
  
  // {{{ constructor

  function __construct( $theOption, $theAnnounce)
    {
      ENTER("cliList::__construct",__LINE__);
      $this->_myOption=$theOption;
      $this->_myAnnounce=$theAnnounce;
      reset($this->_myOption);
    }

  // }}}
  // {{{ set KeyIsDisplayed
  function setKeyIsDisplayed($theKeyIsDisplayed)
    {
      ENTER("cliList::setKeyIsDisplayed",__LINE__);
      $this->_myKeyIsDisplayed=$theKeyIsDisplayed;
    }
  // }}}
  // {{{ getType
  function getType()
    {
      ENTER("cliList::getType",__LINE__);
      return areaList;
    }
  // }}}
  // {{{ announceItem

  function announceItem()
    {
      ENTER("cliList::announceItem",__LINE__);
      if ($this->_myKeyIsDisplayed)
	{
	  echo key($this->_myOption)." ";
	}      
      echo current($this->_myOption)."\n";
    }

  // }}}
  // {{{ jumpToFirstItem
  function jumpToFirstItem()
    {
      ENTER("cliList::jumpToFirstItem",__LINE__);
      reset($this->_myOption);
      return true;
    }
  // }}}
  // {{{ jumpToLastItem

  function jumpToLastItem()
    {
      ENTER("cliList::jumpToLastItem",__LINE__);
      end($this->_myOption);
      return true;
    }

  // }}}
  // {{{ gotoNextItem
  function gotoNextItem()
    {
      ENTER("cliList::gotoNextItem",__LINE__);
      $aStatus=next($this->_myOption);
      if ($aStatus!==false)
	{
	  $aStatus=true;
	}
      return $aStatus;
    }

  // }}}
  // {{{ gotoPreviousItem
  function gotoPreviousItem()
    {
      ENTER("cliList::gotoPreviousItem",__LINE__);
      $aStatus=prev($this->_myOption);
      if ($aStatus!==false)
	{
	  $aStatus=true;
	}
      return $aStatus;
    }
  // }}}
  // {{{ apply
  function apply( & $theKey)
    {
      ENTER("cliList::apply",__LINE__);
      $theKey=key($this->_myOption);
      return ListProcessedValue;
    }
  // }}}

}
// }}}
// {{{ cliRadio

class cliRadio extends cliList
{
  protected $_myDefaultKey;
  protected $_mySelectedKey;

  // {{{ constructor
  function __construct( $theOption, $theAnnounce, $theDefaultKey)
    {
      ENTER("cliRadio::__construct",__LINE__);
      parent::__construct($theOption, $theAnnounce);
      $this->_mySelectedKey=$theDefaultKey;
      $this->_myDefaultKey=$theDefaultKey;
    }
  // }}}
  // {{{ apply
  function apply( & $theKey)
    {
      ENTER("cliRadio::apply",__LINE__);
      $this->_mySelectedKey=key($this->_myOption);
      $theKey=$this->_mySelectedKey;
      return ListProcessedValue;
    }
  // }}}
  // {{{ announceItem
  function announceItem()
    {
      ENTER("cliRadio::announceItem",__LINE__);
      $aKey=key($this->_myOption);
      if ($this->_mySelectedKey==$aKey)
	{
	  echo gettext(" Selected.");
	}
      
      if ($this->_myKeyIsDisplayed)
	{
	  echo " $aKey";
	}
      
      echo " ".current($this->_myOption)."\n";
    }
  // }}}
}

// }}}
// {{{ cliInputBox

class cliInputBox extends cliArea
{
  protected $_myDefaultValue;
  protected $_myCurrentValue;

  // {{{ constructor
  function __construct( $theAnnounce, $theDefaultValue)
    {
      ENTER("cliInputBox::__construct",__LINE__);

      $this->_myDefaultValue=$theDefaultValue;
      $this->_myCurrentValue=$theDefaultValue;
      $this->_myAnnounce=$theAnnounce;
    }
  // }}}

  function getType(){ return areaInputBox;}

  // {{{ getInput
  function getInput( $theTerminal, & $theInput, $theLastInput)
    {
      ENTER("cliInputBox::getInput",__LINE__);

      // the previous typed character is included in the string
      if (!ctype_print( $theLastInput))
	{
	  $theLastInput="";
	}

      $aResult=$theTerminal->getLine( $theInput, $theLastInput);

//       if($aResult==getCharOK)
// 	{
// 	  if (($anInput != "\t") && ctype_print( $theLastInput))
// 	    {
// 	      // the previous typed character is included in the string
// 	      $theInput=$theLastInput.$theInput;
// 	    }
// 	}

      return $aResult;
    }
  // }}}
  // {{{ processInput
  function processInput( $theResult, & $theInput, $theTerminal)
    {
      ENTER("cliInputBox::processInput",__LINE__);
      $aResult=$theResult;
      if ($aResult==getCharOK)
	{
	  switch( $theInput)
	    {
	    case "\t":
	      $aResult=getCharNextArea;
	      break;

	    default:
	      $this->_myCurrentValue=$theInput;
	      if ($theTerminalIsDumb)
		{
		  $aResult=getCharDownArrowKey;
		}
	      else
		{
		  $aResult=getCharApply;
		}
	      break;
	    }
	}
      return $aResult;
    }
  // }}}
  // {{{ apply

  function apply( & $theInput)
    {
      ENTER("cliInputBox::apply",__LINE__);
      $theInput = $this->_myCurrentValue;
      return InputProcessedValue;
    }

  // }}}
}
// }}}
// {{{ cliCheckbox

class cliCheckbox extends cliList
{
  protected $_myDefaultKey;
  protected $_mySelectedKey;

  // {{{ constructor
  function __construct( $theOption, $theAnnounce, $theDefaultKeys)
    {
      ENTER("cliCheckbox::__construct",__LINE__);
      parent::__construct($theOption, $theAnnounce);
      $this->_myDefaultKey=$theDefaultKeys;
      $this->_mySelectedKey=$theDefaultKeys;
    }
  // }}}
  // {{{ apply
  function apply( & $theKey)
    {
      ENTER("cliCheckbox::apply",__LINE__);    
      $begin = true;
      $theKey="";
      foreach ($this->_mySelectedKey as $aLabel=>$aBoolean)
	{
	  if ($begin)
	    {
	      $begin = false;
	      $theKey = $aLabel;
	    }
	  else
	    {
	      $theKey = "$theKey $aLabel";
	    }
	}
      return CheckboxProcessedValue;
    }
  // }}}
  // {{{ toggleItem
  function toggleItem()
    {
      ENTER("cliCheckbox::toggleItem",__LINE__);
      $aKey=key($this->_myOption);
      if (isset($this->_mySelectedKey[ $aKey]))
	{
	  unset($this->_mySelectedKey[ $aKey]);
	}
      else
	{
	  $this->_mySelectedKey[ $aKey]=true;
	}
    }
  // }}}
  // {{{ selectItem
  function selectItem()
    {
      ENTER("cliCheckbox::selectItem",__LINE__);
      $aKey=key($this->_myOption);
      $this->_mySelectedKey[ $aKey]=true;
    }
  // }}}
  // {{{ unselectItem
  function unselectItem()
    {
      ENTER("cliCheckbox::unselectItem",__LINE__);
      $aKey=key($this->_myOption);
      unset($this->_mySelectedKey[ $aKey]);
    }
  // }}}
  // {{{ announceItem
  function announceItem()
    {
      ENTER("cliCheckbox::announceItem",__LINE__);
      $aKey=key($this->_myOption);
      if (isset($this->_mySelectedKey[$aKey]))
	{
	  echo gettext(" Selected.");
	}

      if ($this->_myKeyIsDisplayed)
	{
	  echo " $aKey";
	}

      echo " ".current($this->_myOption)."\n";
    }
  // }}}
  // {{{ processInput

  function processInput( $theResult, $theInput, $theTerminal)
    {
      ENTER("cliCheckbox::processInput",__LINE__);
      $aResult=$theResult;
      if ($aResult==getCharOK)
	{
	  switch( $theInput)
	    {
	    case " ":
	      $this->toggleItem();
	      break;

	    case "\r":
	    case "\n":
	      if ($theTerminalIsDumb)
		{
		  $aResult=getCharDownArrowKey;
		}
	      else
		{
		  $this->selectItem();
		}
	      break;

	    default:
	      if ($this->_isYesOrNo( $theInput, $aYesResult)) 
		{
		  if ($aYesResult)
		    {
		      $this->selectItem();
		    }
		  else
		    {
		      $this->unselectItem();
		    }
		}
	      else 
		{
		  $aResult = parent::processInput( $aResult, $theInput, $theTerminal);
		}
	      break;
	    }
	}
      return $aResult;
    }

  // }}}
}

// }}}
// {{{ cliButton
class cliButton extends cliArea
{
  protected $_myKey; // An integer. For example myKey=0 for myLabel="OK", 1 for "Cancel",...
  protected $_myLabel;

  // {{{ constructor
  function __construct( $theKey, $theValue, $theAnnounce)
    {
      ENTER("cliButton::__construct",__LINE__);
      $this->_myKey=$theKey;
      $this->_myValue=$theValue;
      $this->_myAnnounce=$theAnnounce;
    }
  // }}}
  // {{{ getType
  function getType()
    {
      ENTER("cliButton::getType",__LINE__);
      return areaButton;
    }
  // }}}
  // {{{ announceItem
  function announceItem()
    {
      ENTER("cliButton::announceItem",__LINE__);
      echo $this->_myValue."\n";
    }
  // }}}
  // {{{ apply
  function apply( & $theLabel)
    {
      ENTER("cliButton::apply",__LINE__);
      $theLabel=NULL;
      return $this->_myKey;
    }
  // }}}
}
// }}}
// {{{ cliMessage
class cliMessage extends cliArea
{
  // {{{ constructor
  function __construct( $theAnnounce)
    {
      ENTER("cliMessage::__construct",__LINE__);
      $this->_myAnnounce=$theAnnounce;
    }
  // }}}
  // {{{ getType
  function getType()
    {
      ENTER("cliMessage::getType",__LINE__);
      return areaMessage;
    }
  // }}}

  //  function apply(& $theResult){}

  // {{{ getInput
  function getInput( $theTerminal, & $theInput, $theLastInput )
    {
      ENTER("cliMessage::getInput",__LINE__);
      return getCharDownArrowKey;
    }
  // }}}


}
// }}}
// {{{ class cliDialog

class cliDialog
{
  // {{{

  protected $_myTerminal=NULL;
  protected $_myDialogIsVerbose=true;
  protected $_myList=NULL;
  protected $_myCheckbox=NULL;
  protected $_myInputBox=NULL;
  protected $_myButton=NULL;
  protected $_myDefaultButton=NULL;

  protected $_myAreaManager=NULL; 

  // {{{ setVerbosity, isUpaArrowkey, isDownArrowkey functions

  function setVerbosity( $theDialogIsVerbose)
    {
      ENTER("cliDialog::setVerbosity",__LINE__);
      $this->_myDialogIsVerbose=$theDialogIsVerbose;
    }

  function isUpArrowKey()
    {
      ENTER("cliDialog::isUpArrowKey",__LINE__);
      return ($this->_myTerminal->myLastAnswer==PreviousDialog);
    }

  function isDownArrowKey()
    {
      ENTER("cliDialog::isDownArrowKey",__LINE__);
      return ($this->_myTerminal->myLastAnswer==NextDialog);
    }
  // }}}
  // {{{ constructor

  function __construct($theTerminal)
    {
      // RAF
      //global $TheTerminal;

      ENTER("cliDialog::__construct",__LINE__);
      // RAF $this->_myTerminal=new enhancedTerminal();
      //RAF if (!$this->_myTerminal->isBuild())
	{
	  $this->_myTerminal=$theTerminal;
	    //new dumbTerminal($theTerminal);
	}
      $this->_myAreaManager=new cliAreaManager();
    }

   // }}}     
  // {{{ menu

  // $theSelectedOption (input) is useful for a radio box. Its value is a label. 
  // $theResult: (string) selected fields
  // Return value: 0 (OK), 1 (Cancel), 255 (Escape), or the value of the pressed button.
  function menu($theTitle, $theOptions, & $theResult, $theText=NULL, $theSelectedOption=NULL)
    {
      ENTER("cliDialog::menu",__LINE__);
      echo "$theTitle\n";
      if ($theText)
	{
	  echo "$theText\n";
	}

      // Setting messages for the list or the menu
      if ((count($theOptions) >= MinimumItemsInALongList))
	{
	  $aMessage[verbose][]=gettext("This is a long list.\n");
	  $aMessage[notVerbose][]=gettext("Long list.\n");
	}
      else
	{
	  $aMessage[verbose][]=gettext("This is a short list.\n");
	  $aMessage[notVerbose][]=gettext("Short list.\n");
	}

      // Messages for the buttons
      $aMessage2[verbose][]=gettext("This is a button.\n");
      $aMessage2[notVerbose][]=gettext("Button.\n");

      if ($theSelectedOption)
	{ // list
	  $this->_myTerminal->getMessage( MessageNavigationRadio, $aMessage);
	  $this->_myList=new cliRadio( $theOptions, $aMessage, $theSelectedOption);

	  $this->_myButton[]=new cliButton( OkPressedValue, gettext("OK"), $aMessage2);
	  $this->_myButton[]=new cliButton( CancelPressedValue, gettext("Cancel"), $aMessage2);
	}
      else
	{
	  $this->_myTerminal->getMessage( MessageNavigationMenu, $aMessage);
	  $this->_myList=new cliList( $theOptions, $aMessage);
	  // only the cancel button
	  $this->_myButton[]=new cliButton( CancelPressedValue, gettext("Cancel"), $aMessage2);
	}

      // Area: list (first entry) + buttons
      $this->_myAreaManager->addArea( $this->_myList);

      for ($i=0; $i<count($this->_myButton); $i++)
	{
	  $this->_myAreaManager->addArea($this->_myButton[ $i]);
	}

      $aKeyPressedValue = $this->_myAreaManager->processKeys( $this->_myTerminal, $theResult);

      switch( $aKeyPressedValue)
	{
	case OkPressedValue:
	  { // Ok button pressed: the list is concerned
	    $this->_myList->apply( $theResult);
	  }
	  break;

	case ListProcessedValue:
	  $aKeyPressedValue = OkPressedValue;
	  break;

	default:
	  break;
	}
      return $aKeyPressedValue;
    }

  // }}}
  // {{{ checkBox

  // $theSelectedOption in input are the default checked boxes and, in output gives the selected ones.
  function checkbox($theTitle, $theOptions, & $theResult, $theText=NULL, & $theSelectedOption)
    {
      ENTER("cliDialog::checkbox",__LINE__);
      echo "$theTitle\n";
      if ($theText)
	{
	  echo "$theText\n";

	  // }}}
	}

      // Setting messages for the checkbox
      if ((count($theOptions) >= MinimumItemsInALongList))
	{
	  $aMessage[verbose][]=gettext("There are several checkboxes.\n");
	  $aMessage[notVerbose][]=gettext("Several checkboxes.\n");
	}
      else
	{
	  $aMessage[verbose][]=gettext("There are just a few checkboxes.\n");
	  $aMessage[notVerbose][]=gettext("A few checkboxes.\n");
	}

      // Messages for the buttons
      $aMessage2[verbose][]=gettext("This is a button.\n");
      $aMessage2[notVerbose][]=gettext("Button.\n");

      $this->_myTerminal->getMessage( MessageNavigationCheckbox, $aMessage);
      $this->_myCheckbox=new cliCheckbox( $theOptions, $aMessage, $theSelectedOption);
      
      $this->_myButton[]=new cliButton( OkPressedValue, gettext("OK"), $aMessage2);
      $this->_myButton[]=new cliButton( CancelPressedValue, gettext("Cancel"), $aMessage2);

      // Area: checkbox + buttons
      $this->_myAreaManager->addArea( $this->_myCheckbox);

      for ($i=0; $i<count($this->_myButton); $i++)
	{
	  $this->_myAreaManager->addArea($this->_myButton[ $i]);
	}

      $aKeyPressedValue = $this->_myAreaManager->processKeys( $this->_myTerminal, $theResult);

      switch( $aKeyPressedValue)
	{
	case OkPressedValue:
	  { // in fact, the checkbox is concerned
	    $this->_myCheckbox->apply( $theResult);
	  }
	  break;

	case CheckboxProcessedValue:
	  $aKeyPressedValue = OkPressedValue;
	  break;

	default:
	  break;
	}
      return $aKeyPressedValue;
    }

  // }}}
  // {{{ yesno: Return 0 if yes, or 1 otherwise.
  // theIndex is a prefix to display
  function yesNo($theQuestion, $theTitle=NULL)
    {
      ENTER("cliDialog::yesNo",__LINE__);
      if ($theTitle)
	{
	  echo "$theTitle\n";
	}

      // Message
      $aMessage[verbose][]=$theQuestion."\n";
      $aMessage[notVerbose][]=$theQuestion."\n";
      $this->_myMessage=new cliMessage( $aMessage);

      // Buttons
      unset($aMessage);
      $aMessage[verbose][]=gettext("This is a button.\n");
      $aMessage[notVerbose][]=gettext("Button.\n");

      $this->_myButton[]=new cliButton( OkPressedValue, gettext("Yes"), $aMessage);
      $this->_myButton[]=new cliButton( CancelPressedValue, gettext("No"), $aMessage);

      // Area: list (first entry) + buttons
      $this->_myAreaManager->addArea( $this->_myMessage);

      for ($i=0; $i<count($this->_myButton); $i++)
	{
	  $this->_myAreaManager->addArea($this->_myButton[ $i]);
	}

      return $this->_myAreaManager->processKeys( $this->_myTerminal, $theResult);
    }

  // }}}
  // {{{ messageBox

  function messageBox($theQuestion, $theTitle=NULL)
    {
      ENTER("cliDialog::messageBox",__LINE__);
      if ($theTitle)
	{
	  echo "$theTitle\n";
	}

      // Message
      $aMessage[verbose][]=$theQuestion."\n";
      $aMessage[notVerbose][]=$theQuestion."\n";
      $this->_myMessage=new cliMessage( $aMessage);

      // Buttons
      unset($aMessage);
      $aMessage[verbose][]=gettext("This is a button.\n");
      $aMessage[notVerbose][]=gettext("Button.\n");

      $this->_myButton[]=new cliButton( OkPressedValue, gettext("Ok"), $aMessage);

      // Area: list (first entry) + buttons
      $this->_myAreaManager->addArea( $this->_myMessage);

      for ($i=0; $i<count($this->_myButton); $i++)
      	{
	  $this->_myAreaManager->addArea($this->_myButton[ $i]);
	}

      return $this->_myAreaManager->processKeys( $this->_myTerminal, $aResult);
    }

  // }}}
  // {{{ gauge
  function gauge($theQuestion=NULL, $theTitle=NULL)
    {
      ENTER("cliDialog::gauge",__LINE__);
      $this->_printSentence( $theQuestion, $theIndex, $theTitle);
      while(1)
	{
	  $aResult=$this->_myTerminal->getLine( $anInput);
	  //	  $aChoice=fgets($this->_myFileDescriptor);
	  if ($anInput==NULL)
	    {
	      break;
	    }
	  else if ($anInput!="XXX\n")
	    {
	      echo $anInput;
	    }
	}
    }

// }}}
  // {{{ inputBox

  // theIndex is a prefix to display
  function inputBox($theQuestion, $theDefault=NULL, & $theResult, $theTitle=NULL)
    {
      ENTER("cliDialog::inputBox",__LINE__);
      echo "$theTitle\n";
      if ($theText)
	{
	  echo "$theText\n";
	}

      // Main buttons (OK/Cancel)
      $aMessage[verbose][]=gettext("This is a button.\n");
      $aMessage[notVerbose][]=gettext("Button.\n");
      $this->_myButton[]=new cliButton( OkPressedValue, gettext("OK"), $aMessage);
      $this->_myButton[]=new cliButton( CancelPressedValue, gettext("Cancel"), $aMessage);

      // Default button: useful to accept the default value
      if ($theDefault != NULL)
	{
	  unset($aMessage);
	  $aMessage[verbose][]="$theQuestion\n";
	  $aMessage[notVerbose][]="$theQuestion\n";

	  $this->_myTerminal->getMessage( MessageNavigationInputBoxDefaultButton, $aMessage, $theDefault);
	  $this->_myDefaultButton=new cliButton( OkPressedValue, " ", $aMessage);
	}

      // Input box
      unset($aMessage);
      if ($theDefault == NULL)
	{
	  $aMessage[verbose][]="$theQuestion\n";
	  $aMessage[notVerbose][]="$theQuestion\n";
	}

      $this->_myTerminal->getMessage( MessageNavigationInputBox, $aMessage);
      $this->_myInputBox=new cliInputBox( $aMessage, $theDefault);

      // Building the areas
      if ($this->_myDefaultButton != NULL)
	{
	  $this->_myAreaManager->addArea( $this->_myDefaultButton);
	}
      $this->_myAreaManager->addArea( $this->_myInputBox);

      for ($i=0; $i<count($this->_myButton); $i++)
	{
	  $this->_myAreaManager->addArea($this->_myButton[ $i]);
	}

      $aKeyPressedValue = $this->_myAreaManager->processKeys( $this->_myTerminal, $theResult);

      switch( $aKeyPressedValue)
	{
	case OkPressedValue:
	  { // in fact, the input field is concerned
	    $this->_myInputBox->apply( $theResult);
	  }
	  break;

	case InputProcessedValue:
	  $aKeyPressedValue = OkPressedValue;
	  break;

	default:
	  break;
	}
      return $aKeyPressedValue;
    }

  // }}}
  // {{{ _printSentence
  // theIndex is a prefix to display
  protected function _printSentence( $theSentence, $theIndex, $theTitle)
    {
      ENTER("cliDialog::_printSentence",__LINE__);
      if ($theTitle)
	{
	  echo sprintf(gettext("%s\n"), $theTitle);
	}
      if ($theIndex)
	{
	  if ($theSentence)
	    {
	      echo sprintf(gettext("%d. %s\n"), $theIndex, $theSentence);
	    }
	}
      else if ($theSentence)
	{
	  echo sprintf(gettext("%s\n"), $theSentence);
	}
    }

  // }}}
}

// }}}


?>
