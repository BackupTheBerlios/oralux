#!/usr/bin/php
<?php
// ----------------------------------------------------------------------------
// dialog.php
// $Id: dialog.php,v 1.2 2004/10/30 19:40:46 gcasse Exp $
// $Author: gcasse $
// Description: command line based dialog (menu, yes/no question, dialog box,...)
// $Date: 2004/10/30 19:40:46 $ |
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
require_once("terminal.php");

$aValue=0;

// {{{ Constants

define("UNDEFINED",$aValue++);
define("TITLE",$aValue++);
define("NOBUTTON",$aValue++);
define("BACKTITLE",$aValue++);
define("MENU_TEXT",$aValue++);
define("MENU_NUM1",$aValue++);
define("MENU_NUM2",$aValue++);
define("MENU_NUM3",$aValue++);
define("MENU_NAME",$aValue++);
define("MENU_LABEL",$aValue++);
define("INPUT_BOX_TEXT",$aValue++);
define("INPUT_BOX_NUM1",$aValue++);
define("INPUT_BOX_NUM2",$aValue++);
define("INPUT_BOX_INIT",$aValue++); // Option
define("RADIO_TEXT",$aValue++);
define("RADIO_NUM1",$aValue++);
define("RADIO_NUM2",$aValue++);
define("RADIO_NUM3",$aValue++);
define("RADIO_NAME",$aValue++);
define("RADIO_LABEL",$aValue++);
define("RADIO_STATUS",$aValue++);
define("YESNO_TEXT",$aValue++);
define("YESNO_NUM1",$aValue++);
define("YESNO_NUM2",$aValue++);
define("MSGBOX_TEXT",$aValue++);
define("MSGBOX_NUM1",$aValue++);
define("MSGBOX_NUM2",$aValue++);
define("CHECK_TEXT",$aValue++);
define("CHECK_NUM1",$aValue++);
define("CHECK_NUM2",$aValue++);
define("CHECK_NUM3",$aValue++);
define("CHECK_NAME",$aValue++);
define("CHECK_LABEL",$aValue++);
define("CHECK_STATUS",$aValue++);
define("GAUGE_TEXT",$aValue++);
define("GAUGE_NUM1",$aValue++);
define("GAUGE_NUM2",$aValue++);	
define("GAUGE_PERCENT",$aValue++); // Option
define("END",$aValue++);		

// }}}

// {{{ textDialog

  class textDialog {
    var $myText;
    function textDialog( $theText)
      {
	$this->myText=$theText;
      }
  }

// }}}
// {{{ gauge

  class gauge extends textDialog {
    function printDialog( & $theDialog, $theTitle, & $theResult)
      {
	return $theDialog->gauge($this->myText, $theTitle);
      }
  }

// }}}
// {{{ messageBox

  class messageBox extends textDialog {
    function printDialog( & $theDialog, $theTitle, & $theResult)
      {
	return $theDialog->messageBox($this->myText, $theTitle);
      }
  }

// }}}
// {{{ yesNo

  class yesNo extends textDialog {
    function printDialog( & $theDialog, $theTitle, & $theResult)
      {
	return $theDialog->yesNo($this->myText, $theTitle);
      }
  }

// }}}
// {{{ inputBox

  class inputBox extends textDialog {
    protected $myDefault;

    function setDefault( $theDefault)
      {
	$this->myDefault = $theDefault;
      }

    function printDialog( & $theDialog, $theTitle, & $theResult)
      {
	return $theDialog->inputBox( $this->myText, $this->myDefault, $theResult, $theTitle);
      }
  }

// }}}
// {{{ selectList

  class selectList {
    var $myText;
    var $myItem;
    var $mySelectedItem;
    protected $myName;

    function selectList( $theText)
      {
	$this->myText=$theText;
	$this->myItem=array();
	$this->mySelectedItem=array();
      }

    function newItem( $theName)
      {
	$this->myName = $theName;
      }
    
    function setLabel($theLabel)
      {
	$this->myItem[$this->myName] = $theLabel;
      }

    function setSelected( $theItemIsSelected)
      {
	$this->mySelectedItem[ $this->myName] = $theItemIsSelected;
      }
  }

// }}}
// {{{ radio

  class radio extends selectList {

    function printDialog( & $theDialog, $theTitle, & $theResult)
      {
	// Finding the selected option
	$aDefaultItem=NULL;
	foreach($this->mySelectedItem as $aLabel=>$aValue)
	  {
	    if ($aValue==true)
	      {
		$aDefaultItem=$aLabel;
	      }
	  }
	return $theDialog->menu( $theTitle, $this->myItem, $theResult, $this->myText, $aDefaultItem);
      }
  }

// }}}
// {{{ menu

  class menu extends selectList {

    function printDialog( & $theDialog, $theTitle, & $theResult)
      {
	return $theDialog->menu( $theTitle, $this->myItem, $theResult, $this->myText);
      }
  }

// }}}
// {{{ checkbox

  class checkbox extends selectList {
    function printDialog( & $theDialog, $theTitle, & $theResult)
      {
	return $theDialog->checkbox($theTitle, $this->myItem, $theResult, $this->myText, $this->mySelectedItem);
      }
  }

// }}}

// {{{ class dialog
class dialog 
{
  var $myStatus;
  protected $myBackTitle=NULL;
  protected $myTitle=NULL;
  protected $mySelectList=NULL;
  protected $myTextDialog=NULL;
  protected $myDialog=NULL;
  protected $myItem=NULL; // supplies the kind of item (INPUT_BOX_TEXT means input box,...).
  protected $myDialogWithDefaultButton=true; // If true the default buttons (OK and Cancel) are present.

  // {{{ getKeyword

  protected function getKeyword( $theWord)
    {
      switch( $theWord)
	{
	case "--title":
	  $aState=TITLE;
	  break;
	case "--backtitle":
	  $aState=BACKTITLE;
	  break;
	case "--nobutton":
	  $aState=NOBUTTON; // Added for Oralux: no default button
	  break;
	case "--menu":
	  $aState=MENU_TEXT;
	  break;
	case "--inputbox":
	case "--passwordbox":
	  $aState=INPUT_BOX_TEXT;
	  break;
	case "--radiolist":
	  $aState=RADIO_TEXT;
	  break;
	case "--yesno":
	  $aState=YESNO_TEXT;
	  break;
	case "--msgbox":
	  $aState=MSGBOX_TEXT;
	  break;
	case "--checklist":
	  $aState=CHECK_TEXT;
	  break;
	case "--gauge":
	  $aState=GAUGE_TEXT;
	  break;
	default:
	  $aState=UNDEFINED;
	  break;
	}

      if (($a_state != TITLE) && ($a_state != BACKTITLE))
	{
	  $this->myItem=$aState;
	}

      return $aState;
    }

  // }}}
  // {{{ parse

  protected function parse($argc, $argv)
    {
      $aState=UNDEFINED;
      foreach ($argv as $arg)
	{    
	  switch($aState)
	    {
	    case UNDEFINED:
	      $aState=$this->getKeyword($arg);
	      break;

	    case TITLE:
	      $this->myTitle=$arg;
	      $aState=UNDEFINED;
	      break;

	    case NOBUTTON:
	      $this->myDialogWithDefaultButton=false;
	      $aState=UNDEFINED;
	      break;

	    case BACKTITLE:
	      $this->myBackTitle=$arg;
	      $aState=UNDEFINED;
	      break;

	    case MENU_TEXT:
	      $this->mySelectList=new menu($arg);
	      $aState=MENU_NUM1;
	      break;

	    case MENU_NUM1:
	    case MENU_NUM2:
	    case MENU_NUM3:
	      $aState++;
	      break;

	    case MENU_NAME:
	      $this->mySelectList->newItem( $arg);
	      $aState=MENU_LABEL;
	      break;

	    case MENU_LABEL:
	      $this->mySelectList->setLabel( $arg);
	      $aState=MENU_NAME;
	      break;

	    case INPUT_BOX_TEXT:
	      $this->myTextDialog=new inputBox($arg);
	      $aState=INPUT_BOX_NUM1;
	      break;

	    case INPUT_BOX_NUM1:
	    case INPUT_BOX_NUM2:
	      $aState++;
	      break;

	    case INPUT_BOX_INIT:
	      $this->myTextDialog->setDefault( $arg);
	      $aState=END;
	      break;

	    case RADIO_TEXT:
	      $this->mySelectList=new radio( $arg);
	      $aState=RADIO_NUM1;
	      break;

	    case RADIO_NUM1:
	    case RADIO_NUM2:
	    case RADIO_NUM3:
	      $aState++;
	      break;

	    case RADIO_NAME:
	      $this->mySelectList->newItem( $arg);
	      $aState=RADIO_LABEL;
	      break;

	    case RADIO_LABEL:
	      $this->mySelectList->setLabel( $arg);
	      $aState=RADIO_STATUS;
	      break;

	    case RADIO_STATUS:
	      if (strtolower($arg)=="on")
		{
		  $this->mySelectList->setSelected( true );
		}
	      $aState=RADIO_NAME;
	      break;

	    case YESNO_TEXT:
	      $this->myTextDialog=new yesNo($arg);
	      $aState=YESNO_NUM1;
	      break;

	    case YESNO_NUM1:
	      $aState=YESNO_NUM2;
	      break;

	    case YESNO_NUM2:
	      $aState=END;
	      break;

	    case MSGBOX_TEXT:
	      $this->myTextDialog=new messageBox($arg);
	      $aState=MSGBOX_NUM1;
	      break;

	    case MSGBOX_NUM1:
	      $aState=MSGBOX_NUM2;
	      break;

	    case MSGBOX_NUM2:
	      $aState=END;
	      break;

	    case CHECK_TEXT:
	      $this->mySelectList=new checkbox($arg);
	      $aState=CHECK_NUM1;
	      break;

	    case CHECK_NUM1:
	    case CHECK_NUM2:
	    case CHECK_NUM3:
	      $aState++;
	      break;

	    case CHECK_NAME:
	      $this->mySelectList->newItem($arg);
	      $aState=CHECK_LABEL;
	      break;

	    case CHECK_LABEL:
	      $this->mySelectList->setLabel($arg);
	      $aState=CHECK_STATUS;
	      break;

	    case CHECK_STATUS:
	      if (strtolower($arg)=="on")
		{
		  $this->mySelectList->setSelected( true );
		}
	      $aState=CHECK_NAME;
	      break;

	    case GAUGE_TEXT:
	      $this->myTextDialog=new gauge($arg);
	      $aState=GAUGE_NUM1;
	      break;

	    case GAUGE_NUM1:
	    case GAUGE_NUM2:
	      $aState++;
	      break;

	    case GAUGE_PERCENT:
	      $aState=END;
	      break;
	    }

	  if ($aState==END)
	    {
	      break;
	    }
	}  
    }

  // }}}
  // {{{ _displayResult

  protected function _displayResult( $theResult)
    {
      if ($theResult!=NULL)
	{
	  $fd=fopen("php://stderr","w");
	  fprintf($fd, $theResult);
	  fclose($fd);
	}
    }

  // }}}
  // {{{ dialog

  function dialog( $argc, $argv)
    {
      // 0: ok
      // 1: cancel
      // 255: esc
      $this->myStatus=0;
      $aResult=NULL;

      $this->_myTerminal=new enhancedTerminal();
      if (!$this->_myTerminal->isBuild())
	{
	  $this->_myTerminal=new dumbTerminal($theTerminal);
	}

      $this->parse( $argc, $argv);
      $this->myDialog=new cliDialog($this->_myTerminal, $this->myDialogWithDefaultButton);

      if ($this->myBackTitle)
	{
	  printf("%s\n", $this->myBackTitle);
	}

      if ($this->myTextDialog)
	{
	  $this->myStatus=$this->myTextDialog->printDialog( $this->myDialog, $this->myTitle, $aResult);
	}

      else if ($this->mySelectList)
	{
	  $this->myStatus=$this->mySelectList->printDialog( $this->myDialog, $this->myTitle, $aResult);
	}

      else if ($this->myGaugeText)
	{
	}
      $this->_displayResult($aResult);
    }

  // }}}
}
// }}}

$a=new dialog( $argc, $argv);
exit ($a->myStatus);

?>
