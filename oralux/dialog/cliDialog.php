<?php
// ----------------------------------------------------------------------------
// cliDialog.php
// $Id: cliDialog.php,v 1.7 2005/04/03 00:36:27 gcasse Exp $
// $Author: gcasse $
// Description: command line based dialog (menu, yes/no question, dialog box,...)
// $Date: 2005/04/03 00:36:27 $ |
// $Revision: 1.7 $ |
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

$BASE=dirname(__FILE__);
require_once("$BASE/../main/helpEmul.php");
require_once("i18n.php");

// {{{ Constants

define ("OkPressedValue", 0);
define ("CancelPressedValue", 1);
define ("EscapePressedValue", 255);
define ("PreviousProcessedValue",1000);
define ("ListProcessedValue",2000); // for internal use
define ("InputProcessedValue",2001); // for internal use
define ("CheckboxProcessedValue",2002); // for internal use

// YesNo dialog
define("DialogYesAnswer",0);
define("DialogNoAnswer",1);

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
define ("areaYesNo", 4);

define ("verbose", 0);
define ("notVerbose", 1);

// }}}

// {{{ cliArea: Area in a dialog. It can be a list, a button. It is possible to jump from 'area' to 'area'.

abstract class cliArea 
{
  protected $myAnnounce; // what is announced when the area has the focus
  protected $myVerbosity=verbose; // concerns how messages will be verbose

  abstract public function getType(); // list, button

  // {{{ announceTypeOfArea
  public function announceTypeOfArea(  $theAnnounceIsRepeated)
  {
    ENTER("cliArea::announceTypeOfArea",__LINE__);
    // By default, we say the announce once.
    // E.g. we do not want to hear "this is a long list" 
    // each time we go from the last to the first item of a menu.
    // So if the current announce is said once again (a repetition) then we forget it.

    if (($theAnnounceIsRepeated == false) && isset($this->myAnnounce))
      {
	foreach ($this->myAnnounce[ $this->myVerbosity ] as $aMessage)
	  {
	    echo $aMessage;
	  }
      }
  }
  // }}}
  // {{{ setVerbosity
  function setVerbosity( $theVerbosity)
    {
      ENTER("cliArea::setVerbosity",__LINE__);
      $this->myVerbosity=$theVerbosity;
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
  protected $myArea=NULL; // array of objects: e.g. a list and a few buttons
  protected $myInput;
  protected $myWrapIsEnabled=true; // If wrap is enabled, then once the last area is reached, we can jump to the first area

  // {{{ addArea
  function addArea( $theArea)
    {
      ENTER("cliAreaManager::addArea",__LINE__);
      $this->myArea[]=$theArea;
    }
  // }}}
  // {{{ setWrap
  function setWrap( $isEnabled)
    {
      $this->myWrapIsEnabled = $isEnabled;
    }
  // }}}

  // {{{ getCurrentArea
  function getCurrentArea()
    {
      ENTER("cliAreaManager::getCurrentArea",__LINE__);
      return current($this->myArea);
    }
  // }}}
  // {{{ gotoNextArea

  function gotoNextArea()
    {
      ENTER("cliAreaManager::gotoNextArea",__LINE__);
      $oldArea=current($this->myArea);

      $aCurrentArea=next($this->myArea);
      if ($aCurrentArea==false)
	{
	  if ($this->myWrapIsEnabled)
	    {
	      reset($this->myArea);
	      $aCurrentArea=current($this->myArea);
	    }
	}

      if ($aCurrentArea!=false)
	{
	  $aRepeatedAnnounce = ($aCurrentArea == $oldArea);
	  $aCurrentArea->announceTypeOfArea( $aRepeatedAnnounce);
	}
      return ($aCurrentArea != false);
    }
  // }}}
  // {{{ gotoPreviousArea
  function gotoPreviousArea()
    {
      ENTER("cliAreaManager::gotoPreviousArea",__LINE__);
      $oldArea=current($this->myArea);
      $aCurrentArea=prev($this->myArea);
      if ($aCurrentArea==false)
	{
	  if ($this->myWrapIsEnabled)
	    {
	      $aCurrentArea=end($this->myArea);
	    }
	}
      if ($aCurrentArea!=false)
	{
	  $aRepeatedAnnounce = ($aCurrentArea == $oldArea);
	  $aCurrentArea->announceTypeOfArea( $aRepeatedAnnounce);
	}
      return ($aCurrentArea != false);
    }
  // }}}
  // {{{ jumpToFirstArea
  function jumpToFirstArea()
    {
      ENTER("cliAreaManager::jumpToFirstArea",__LINE__);
      reset($this->myArea);
      return current($this->myArea);
    }
  // }}}
  // {{{ gotoNextItemInArea, gotoPreviousItemInArea
  // Useful to browse the steps of the area (typically the list options).
  function gotoNextItemInArea()
    {
      ENTER("cliAreaManager::gotoNextItemInArea",__LINE__);
      $aCurrentArea=current($this->myArea);
      return $aCurrentArea->gotoNextItem();
    }
  function gotoPreviousItemInArea()
    {
      ENTER("cliAreaManager::gotoPreviousItemInArea",__LINE__);
      $aCurrentArea=current($this->myArea);
      return $aCurrentArea->gotoPreviousItem();
    }
  function jumpToFirstItemInArea()
    {
      ENTER("cliAreaManager::jumpToFirstItemInArea",__LINE__);
      $aCurrentArea=current($this->myArea);
      return $aCurrentArea->jumpToFirstItem();
    }
  function jumpToLastItemInArea()
    {
      ENTER("cliAreaManager::jumpToLastItemInArea",__LINE__);
      $aCurrentArea=current($this->myArea);
      return $aCurrentArea->jumpToLastItem();
    }
  function announceItemInArea()
    {
      ENTER("cliAreaManager::announceItemInArea",__LINE__);
      $aCurrentArea=current($this->myArea);
      return $aCurrentArea->announceItem();
    }
  // }}}
  // {{{ processKeys
  function processKeys( & $theTerminal, & $theResult)
    {
      ENTER("cliAreaManager::processKeys",__LINE__);
      $aCurrentArea=$this->jumpToFirstArea(); // first element (list)
      $aCurrentArea->announceTypeOfArea( false);
      $this->jumpToFirstItemInArea();
      $aKeyPressedValue=EscapePressedValue;
      $this->myInput="";

      $anInputIsExpected=true;
      while( $anInputIsExpected)
	{
	  $this->announceItemInArea();
	  $anArea=$this->getCurrentArea();

	  $theResult=$anArea->getInput( $theTerminal, $anInput, $this->myInput );
	  $this->myInput=$anInput;
	  $theResult=$anArea->processInput( $theResult, $anInput, $theTerminal);

	  switch($theResult)
	    {
	    case getCharDownArrowKey:
	    if ($this->gotoNextItemInArea() == false)
		{
		  $this->jumpToFirstItemInArea();
		  if ($this->gotoNextArea()==false)
		    {
		      $aKeyPressedValue=EscapePressedValue;
		      $anInputIsExpected=false;
		    }
		}
	      break;

	    case getCharUpArrowKey:
	      if ($this->gotoPreviousItemInArea() == false)
		{ 
		  $this->jumpToLastItemInArea();
		  if ($this->gotoPreviousArea()==false)
		    {
		      $aKeyPressedValue=EscapePressedValue;
		      $anInputIsExpected=false;
		    }
		}
	      break;

	    case getCharNextArea:
	      if ($this->gotoNextArea()==false)
		{
		  $aKeyPressedValue=EscapePressedValue;
		  $anInputIsExpected=false;
		}
	      break;

	    case getCharPreviousArea:
	      if ($this->gotoPreviousArea()==false)
		{
		  $aKeyPressedValue=EscapePressedValue;
		  $anInputIsExpected=false;
		}
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
  protected $myOption;
  protected $myKeyIsDisplayed;

  // {{{ constructor

  function __construct( $theOption, $theAnnounce, $theKeyIsDisplayed=true)
    {
      ENTER("cliList::__construct",__LINE__);
      $this->myOption=$theOption;
      $this->myAnnounce=$theAnnounce;
      $this->myKeyIsDisplayed = $theKeyIsDisplayed;
      reset($this->myOption);
    }

  // }}}
  // {{{ getType
  function getType()
    {
      ENTER("cliList::getType",__LINE__);
      return areaList;
    }
  // }}}
  // {{{ getDisplayedKey
  protected function displayKey()
    {
      $aKey=key($this->myOption);
//       if (is_numeric($aKey))
// 	{
// 	  // The numeric key is incremented because 
// 	  // the first numeric key is supposed to be zero.
// 	  $aKey++;
// 	}
      echo "$aKey. ";
    }
  // }}}

  // {{{ announceItem

  function announceItem()
    {
      ENTER("cliList::announceItem",__LINE__);
      echo "\n";
      if ($this->myKeyIsDisplayed)
	{
	  $this->displayKey();
	}      
      echo current($this->myOption);
    }

  // }}}
  // {{{ jumpToFirstItem
  function jumpToFirstItem()
    {
      ENTER("cliList::jumpToFirstItem",__LINE__);
      reset($this->myOption);
      return true;
    }
  // }}}
  // {{{ jumpToLastItem

  function jumpToLastItem()
    {
      ENTER("cliList::jumpToLastItem",__LINE__);
      end($this->myOption);
      return true;
    }

  // }}}
  // {{{ gotoNextItem
  function gotoNextItem()
    {
      ENTER("cliList::gotoNextItem",__LINE__);
      $aStatus=next($this->myOption);
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
      $aStatus=prev($this->myOption);
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
      $theKey=key($this->myOption);
      return ListProcessedValue;
    }
  // }}}

}

// }}}
// {{{ cliRadio

class cliRadio extends cliList
{
  protected $myDefaultKey;
  protected $mySelectedKey;

  // {{{ constructor
  function __construct( $theOption, $theAnnounce, $theDefaultKey, $theKeyIsDisplayed=true)
    {
      ENTER("cliRadio::__construct",__LINE__);
      parent::__construct($theOption, $theAnnounce, $theKeyIsDisplayed);
      $this->mySelectedKey=$theDefaultKey;
      $this->myDefaultKey=$theDefaultKey;
    }
  // }}}
  // {{{ apply
  function apply( & $theKey)
    {
      ENTER("cliRadio::apply",__LINE__);
      $this->mySelectedKey=key($this->myOption);
      $theKey=$this->mySelectedKey;
      return ListProcessedValue;
    }
  // }}}
  // {{{ announceItem

  function announceItem()
    {
      ENTER("cliRadio::announceItem",__LINE__);
      echo "\n";
      $aKey=key($this->myOption);
      if ($this->mySelectedKey==$aKey)
	{
	  echo gettext("Selected. ");
	}
      
      if ($this->myKeyIsDisplayed)
	{
	  $this->displayKey();
	}
      
      echo " ".current($this->myOption);
    }

  // }}}
}

// }}}
// {{{ cliInputBox

class cliInputBox extends cliArea
{
  protected $myDefaultValue;
  protected $myCurrentValue;

  // {{{ constructor

  function __construct( $theAnnounce, $theDefaultValue)
    {
      ENTER("cliInputBox::__construct",__LINE__);

      $this->myDefaultValue=$theDefaultValue;
      $this->myCurrentValue=$theDefaultValue;
      $this->myAnnounce=$theAnnounce;
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
	      $this->myCurrentValue=$theInput;
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
      $theInput = $this->myCurrentValue;
      return InputProcessedValue;
    }

  // }}}
}
// }}}
// {{{ cliCheckbox

class cliCheckbox extends cliList
{
  protected $myDefaultKey;
  protected $mySelectedKey;

  // {{{ constructor
  function __construct( $theOption, $theAnnounce, $theDefaultKeys)
    {
      ENTER("cliCheckbox::__construct",__LINE__);
      parent::__construct($theOption, $theAnnounce);
      $this->myDefaultKey=$theDefaultKeys;
      $this->mySelectedKey=$theDefaultKeys;
    }
  // }}}
  // {{{ apply
  function apply( & $theKey)
    {
      ENTER("cliCheckbox::apply",__LINE__);    
      $begin = true;
      $theKey="";
      foreach ($this->mySelectedKey as $aLabel=>$aBoolean)
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
      $aKey=key($this->myOption);
      if (isset($this->mySelectedKey[ $aKey]))
	{
	  unset($this->mySelectedKey[ $aKey]);
	}
      else
	{
	  $this->mySelectedKey[ $aKey]=true;
	}
    }
  // }}}
  // {{{ selectItem
  function selectItem()
    {
      ENTER("cliCheckbox::selectItem",__LINE__);
      $aKey=key($this->myOption);
      $this->mySelectedKey[ $aKey]=true;
    }
  // }}}
  // {{{ unselectItem
  function unselectItem()
    {
      ENTER("cliCheckbox::unselectItem",__LINE__);
      $aKey=key($this->myOption);
      unset($this->mySelectedKey[ $aKey]);
    }
  // }}}
  // {{{ announceItem

  function announceItem()
    {
      ENTER("cliCheckbox::announceItem",__LINE__);
      echo "\n";
      $aKey=key($this->myOption);
      if (isset($this->mySelectedKey[$aKey]))
	{
	  echo gettext("Selected. ");
	}

      if ($this->myKeyIsDisplayed)
	{
	  $this->displayKey();
	}

      echo " ".current($this->myOption);
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
  protected $myKey; // An integer. For example myKey=0 for myLabel="OK", 1 for "Cancel",...
  protected $myLabel;

  // {{{ constructor

  function __construct( $theKey, $theValue, $theAnnounce)
    {
      ENTER("cliButton::__construct",__LINE__);
      $this->myKey=$theKey;
      $this->myValue=$theValue;
      $this->myAnnounce=$theAnnounce;
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
      //      echo "\n";
      if ($this->myValue!="")
	{
	  echo  "\n".$this->myValue;
	}
    }
  // }}}
  // {{{ apply
  function apply( & $theLabel)
    {
      ENTER("cliButton::apply",__LINE__);
      $theLabel=NULL;
      return $this->myKey;
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
      $this->myAnnounce=$theAnnounce;
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
  protected $myTerminal=NULL;
  protected $myDialogIsVerbose=true;
  protected $myList=NULL;
  protected $myCheckbox=NULL;
  protected $myInputBox=NULL;
  protected $myMessage=NULL;
  protected $myYesNo=NULL;
  protected $myButton=NULL;
  protected $myDefaultValueAcceptationButton=NULL;
  protected $myDialogWithDefaultButton=true; // If true the default buttons (OK and Cancel) are present.

  protected $myAreaManager=NULL; 

  // {{{ setVerbosity, isUpaArrowkey, isDownArrowkey functions

  function setVerbosity( $theDialogIsVerbose)
    {
      ENTER("cliDialog::setVerbosity",__LINE__);
      $this->myDialogIsVerbose=$theDialogIsVerbose;
    }

  function isUpArrowKey()
    {
      ENTER("cliDialog::isUpArrowKey",__LINE__);
      return ($this->myTerminal->myLastAnswer==PreviousDialog);
    }

  function isDownArrowKey()
    {
      ENTER("cliDialog::isDownArrowKey",__LINE__);
      return ($this->myTerminal->myLastAnswer==NextDialog);
    }
  // }}}
  // {{{ constructor

  function __construct($theTerminal, $theDialogWithDefaultButton=true)
    {
      ENTER("cliDialog::__construct",__LINE__);
      $this->myTerminal=$theTerminal;
      $this->myDialogWithDefaultButton=$theDialogWithDefaultButton;
      $this->myAreaManager=new cliAreaManager();
    }

   // }}}     
  // {{{ menu

  // $theSelectedOption (input) is useful for a radio box. Its value is a label. 
  // $theResult: (string) selected fields
  // Return value: 0 (OK), 1 (Cancel), 255 (Escape), or the value of the pressed button.
  function menu($theTitle, $theOptions, & $theResult, $theText=NULL, $theSelectedOption=NULL, $theKeyIsDisplayed=true)
    {
      ENTER("cliDialog::menu",__LINE__);

      echo "\n$theTitle";
      if ($theText)
	{
	  echo "\n$theText";
	}

      // Setting messages for the list or the menu
      if ((count($theOptions) >= MinimumItemsInALongList))
	{
	  $aMessage[verbose][]=gettext("\nThis is a long list.");
	  $aMessage[notVerbose][]=gettext("\nLong list.");
	}
      else
	{
	  $aMessage[verbose][]=gettext("\nThis is a short list.");
	  $aMessage[notVerbose][]=gettext("\nShort list.");
	}

      // Messages for the buttons
      $aMessage2[verbose][]=gettext("\nThis is a button.");
      $aMessage2[notVerbose][]=gettext("\nButton.");

      if ($theSelectedOption)
	{ // list
	  $this->myTerminal->getMessage( MessageNavigationRadio, $aMessage);
	  $this->myList=new cliRadio( $theOptions, $aMessage, $theSelectedOption, $theKeyIsDisplayed);
	  
	  if ($this->myDialogWithDefaultButton)
	    {
	      $this->myButton[]=new cliButton( OkPressedValue, gettext("OK"), $aMessage2);
	      $this->myButton[]=new cliButton( CancelPressedValue, gettext("Cancel"), $aMessage2);
	    }
	}
      else
	{
	  $this->myTerminal->getMessage( MessageNavigationMenu, $aMessage);
	  $this->myList=new cliList( $theOptions, $aMessage, $theKeyIsDisplayed);
	  if ($this->myDialogWithDefaultButton)
	    {
	      // only the cancel button
	      $this->myButton[]=new cliButton( CancelPressedValue, gettext("Cancel"), $aMessage2);
	    }
	}

      // Area: list (first entry) + buttons
      $this->myAreaManager->addArea( $this->myList);

      for ($i=0; $i<count($this->myButton); $i++)
	{
	  $this->myAreaManager->addArea($this->myButton[ $i]);
	}

      $aKeyPressedValue = $this->myAreaManager->processKeys( $this->myTerminal, $theResult);

      switch( $aKeyPressedValue)
	{
	case OkPressedValue:
	  { // Ok button pressed: the list is concerned
	    $this->myList->apply( $theResult);
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
      echo "\n$theTitle";
      if ($theText)
	{
	  echo "\n$theText";
	}

      // Setting messages for the checkbox
      if ((count($theOptions) >= MinimumItemsInALongList))
	{
	  $aMessage[verbose][]=gettext("\nThere are several checkboxes.");
	  $aMessage[notVerbose][]=gettext("\nSeveral checkboxes.");
	}
      else
	{
	  $aMessage[verbose][]=gettext("\nThere are just a few checkboxes.");
	  $aMessage[notVerbose][]=gettext("\nA few checkboxes.");
	}

      // Messages for the buttons
      $aMessage2[verbose][]=gettext("\nThis is a button.");
      $aMessage2[notVerbose][]=gettext("\nButton.");

      $this->myTerminal->getMessage( MessageNavigationCheckbox, $aMessage);
      $this->myCheckbox=new cliCheckbox( $theOptions, $aMessage, $theSelectedOption);
      
      $this->myButton[]=new cliButton( OkPressedValue, gettext("OK"), $aMessage2);
      $this->myButton[]=new cliButton( CancelPressedValue, gettext("Cancel"), $aMessage2);

      // Area: checkbox + buttons
      $this->myAreaManager->addArea( $this->myCheckbox);

      for ($i=0; $i<count($this->myButton); $i++)
	{
	  $this->myAreaManager->addArea($this->myButton[ $i]);
	}

      $aKeyPressedValue = $this->myAreaManager->processKeys( $this->myTerminal, $theResult);

      switch( $aKeyPressedValue)
	{
	case OkPressedValue:
	  { // in fact, the checkbox is concerned
	    $this->myCheckbox->apply( $theResult);
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

  // {{{ yesno: Return OkPressedValue if yes, CancelPressedValue if no, or EscapePressedValue.
  // theIndex is a prefix to display
  function yesNo($theQuestion, $theTitle=NULL)
    {
      ENTER("cliDialog::yesNo",__LINE__);
      if ($theTitle)
	{
	  echo "\n$theTitle";
	}

      // Message
      $aMessage[verbose][]="\n".$theQuestion;
      $aMessage[notVerbose][]="\n".$theQuestion;

      $aMessage[verbose][]="\n".gettext("Yes or No?")." ";
      $aMessage[notVerbose][]="\n".gettext("Yes or No?")." ";

      $this->myYesNo=new cliYesNo( $aMessage);

      // Area: list (first entry) + buttons
      $this->myAreaManager->addArea( $this->myYesNo);
      
      return $this->myAreaManager->processKeys( $this->myTerminal, $aResult);
    }

  // }}}
  // {{{ messageBox

  function messageBox($theQuestion, $theTitle=NULL)
    {
      ENTER("cliDialog::messageBox",__LINE__);
      if ($theTitle)
	{
	  echo "\n$theTitle";
	}

      // Message
      $aMessage[verbose][]="\n".$theQuestion;
      $aMessage[notVerbose][]="\n".$theQuestion;
      $this->myMessage=new cliMessage( $aMessage);

      // Buttons
      unset($aMessage);
      $aMessage[verbose][]="\n".gettext("Press any key to continue.");
      $aMessage[notVerbose][]="\n".gettext("Press any key to continue.");      
      $this->myButton[]=new cliButton( OkPressedValue, "", $aMessage);

      $this->myAreaManager->SetWrap( false);

      // Area: list (first entry) + buttons
      $this->myAreaManager->addArea( $this->myMessage);

      for ($i=0; $i<count($this->myButton); $i++)
      	{
	  $this->myAreaManager->addArea($this->myButton[ $i]);
	}

      return $this->myAreaManager->processKeys( $this->myTerminal, $aResult);
    }

  // }}}
  // {{{ gauge
  function gauge($theQuestion=NULL, $theTitle=NULL)
    {
      ENTER("cliDialog::gauge",__LINE__);
      $this->_printSentence( $theQuestion, $theIndex, $theTitle);
      while(1)
	{
	  $aResult=$this->myTerminal->getLine( $anInput);
	  //	  $aChoice=fgets($this->myFileDescriptor);
	  if ($anInput==NULL)
	    {
	      break;
	    }
	  else if ($anInput!="\nXXX")
	    {
	      echo $anInput;
	    }
	}
    }

// }}}
  // {{{ inputBox

  // theIndex is a prefix to display
  function inputBox($theQuestion, $theDefaultValue=NULL, & $theResult, $theTitle=NULL)
    {
      ENTER("cliDialog::inputBox",__LINE__);
      if ($theTitle)
	{
	  echo "\n$theTitle";
	}

      if ($this->myDialogWithDefaultButton)
	{
	  // Main buttons (OK/Cancel)
	  $aMessage[verbose][]="\n".gettext("This is a button.");
	  $aMessage[notVerbose][]="\n".gettext("Button.");

	  $this->myButton[]=new cliButton( OkPressedValue, gettext("OK"), $aMessage);
	  $this->myButton[]=new cliButton( CancelPressedValue, gettext("Cancel"), $aMessage);
	}

      // Input box
      unset($aMessage);
      
      // Default Value Acceptation button: useful to accept the default value of the input field
      if ($theDefaultValue != NULL)
	{
	  $aMessage[verbose][]="\n$theQuestion";
	  $aMessage[notVerbose][]="\n$theQuestion";

	  $this->myTerminal->getMessage( MessageNavigationInputBoxDefaultButton, $aMessage, $theDefaultValue);
	  $this->myDefaultValueAcceptationButton=new cliButton( OkPressedValue, " ", $aMessage);
	  unset($aMessage);
	}
      else
	{
	  $aMessage[verbose][]="\n$theQuestion";
	  $aMessage[notVerbose][]="\n$theQuestion";
	}
      
      $this->myTerminal->getMessage( MessageNavigationInputBox, $aMessage);
      $this->myInputBox=new cliInputBox( $aMessage, $theDefaultValue);

      // Building the areas
      if ($this->myDefaultValueAcceptationButton != NULL)
	{
	  $this->myAreaManager->addArea( $this->myDefaultValueAcceptationButton);
	}
      $this->myAreaManager->addArea( $this->myInputBox);

      for ($i=0; $i<count($this->myButton); $i++)
	{
	  $this->myAreaManager->addArea($this->myButton[ $i]);
	}

      $aKeyPressedValue = $this->myAreaManager->processKeys( $this->myTerminal, $theResult);

      switch( $aKeyPressedValue)
	{
	case OkPressedValue:
	  { // in fact, the input field is concerned
	    $this->myInputBox->apply( $theResult);
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
	  echo "\n".$theTitle;
	}
      if ($theIndex)
	{
	  if ($theSentence)
	    {
	      echo sprintf("\n%d. %s", $theIndex, $theSentence);
	    }
	}
      else if ($theSentence)
	{
	  echo sprintf("\n%s", $theSentence);
	}
    }

  // }}}
}

// }}}
// {{{ cliYesNo

class cliYesNo extends cliArea
{
  protected $myCurrentValue;

  // {{{ announceTypeOfArea
  public function announceTypeOfArea(  $theAnnounceIsRepeated)
  {
    ENTER("cliArea::announceTypeOfArea",__LINE__);
    // We say the announce at each try.
    // So we behave as it was the first announce
    parent::announceTypeOfArea( false);
  }
  // }}}

  // {{{ constructor
  function __construct( $theAnnounce )
    {
      ENTER("cliYesNo::__construct",__LINE__);
      $this->myAnnounce=$theAnnounce;
    }
  // }}}

  function getType(){ return areaYesNo;}

  // {{{ processInput
  function processInput( $theResult, & $theInput, $theTerminal)
    {
      ENTER("cliYesNo::processInput",__LINE__);
      $aResult=$theResult;

      if (aResult==getCharEscape)
	{
	  $this->myCurrentValue = EscapePressedValue;
	}
      else if ($aResult==getCharOK)
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
	      if ($this->_isYesOrNo( $theInput, $aYesResult))
		{
		  $aResult = getCharApply;
		  $this->myCurrentValue = $aYesResult ? DialogYesAnswer : DialogNoAnswer;
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

  // {{{ apply
  function apply( & $theInput)
    {
      ENTER("cliYesNo::apply",__LINE__);
      return $this->myCurrentValue;
    }
  // }}}
}
// }}}


?>
