#!/usr/bin/php
<?php

// raf: utf8

// ----------------------------------------------------------------------------
// netConfig.php
// $Id: netConfig.php,v 1.1 2004/09/27 20:30:30 gcasse Exp $
// $Author: gcasse $
// Description: Menu for internet settings (php5)
// $Date: 2004/09/27 20:30:30 $ |
// $Revision: 1.1 $ |
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
      //   GString* aCommand=g_string_new(NULL);
//   g_string_sprintf(aCommand, "--clear --title \"%s\" --menu \" \" 20 50 4 \"EAGLE\" \"Eagle USB\" \"ECI\" \"Eci Adsl\"", 
// 		    gettext("Select the driver"));

//   dialogGetString(aCommand);

//   if (strcmp(aResult, "EAGLE")==0) 
//     {
//       starteagleusb();
//     }
//   else if (strcmp(aResult, "ECI")==0)
//     {
//       startECI();
//     }
//   g_string_free (aCommand, true);
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