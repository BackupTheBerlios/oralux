#!/usr/bin/php
<?php

// raf: utf8

// ----------------------------------------------------------------------------
// netConfig.php
// $Id: netConfig.php,v 1.2 2004/10/25 22:12:02 gcasse Exp $
// $Author: gcasse $
// Description: Menu for internet settings (php5)
// $Date: 2004/10/25 22:12:02 $ |
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
require_once("mailMenu.php");
require_once("terminal.php");

// {{{ netConfig

class netConfig
{
  protected $_myMailMenu;

  protected function _startpppconfig()
    {
      echo "startpppconfig";
    }
  
  protected function _adslMenu()
    {
      $aDialog=new cliDialog($this->_myTerminal);

      $aKeyPressed=$aDialog->menu(gettext("Which is your USB ADSL Modem?"), 
				  array(
					"1" => gettext("Sagem"),
					"2" => gettext("SpeedTouch"),
					"3" => gettext("E C I")
					),
				  $aResult);
      unset($aDialog);
      
      if ($aKeyPressed != OkPressedValue)
	{
	  return;
	  //	  return $aKeyPressed;
	}
      switch( (string)$aResult)
	{
	case "1":
	  system("./eagleConfig.sh");
	  break;
	case "2":
	  break;
	case "3":
	  break;
	default:
	  break;
	}
}

  function netConfig()
    {
      $this->_myTerminal=new enhancedTerminal();
      if (!$this->_myTerminal->isBuild())
	{
	  $this->_myTerminal=new dumbTerminal($theTerminal);
	}

      $aDialog=new cliDialog($this->_myTerminal);

      $aKeyPressed=$aDialog->menu(gettext("Setting up the internet connection"), 
				  array(
					"SERIAL" => gettext("Serial Modem"),
					"ADSL" => gettext("Adsl USB Modem"),
					"MAIL" => gettext("Mail")
					),
				  $aResult);
      unset($aDialog);

      if ($aKeyPressed != OkPressedValue)
	{
	  return;
	  //	  return $aKeyPressed;
	}
      switch( (string)$aResult)
	{
	case "SERIAL":
	  $this->_startpppconfig();
	  break;
	case "ADSL":
	  $this->_adslMenu();
	  break;
	case "MAIL":
	  $aMailMenu=new mailMenu( $this->_myTerminal, "knoppix");
	  $aMailMenu->start();
	  break;
	default:
	  break;
	}
    }
}

// }}}

$a=new netConfig;

?>